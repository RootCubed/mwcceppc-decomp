#!/usr/bin/env python3
# Slices a DOL file into .o files

import struct
from pathlib import Path
from typing import cast

from color_term import *
from coff import COFF, COFFSection, COFFSymbol, COFFRelocation
from format_symbols import format_symbols
from peconsts import ImageDirectoryType
from pefile import PE, DebugDirectoryEntry
from project_settings import *
from slicelib import *
from bisect import bisect_left
import capstone

IMAGE_SCN_ALIGN_1BYTES = 0x00100000


def read_base_relocations(pe_file: PE) -> dict[int, int]:
    directory = pe_file.data_directory[ImageDirectoryType.IMAGE_DIRECTORY_ENTRY_BASERELOC]
    relocations = {}
    pos = 0

    while pos < len(directory.data):
        page_rva, block_size = struct.unpack_from('<II', directory.data, pos)
        if block_size == 0:
            break
        for entry, in struct.iter_unpack('<H', directory.data[pos + 8:pos + block_size]):
            if entry >> 12 != 3:  # IMAGE_REL_BASED_HIGHLOW
                continue
            rva = page_rva + (entry & 0xfff)
            for section in pe_file.sections:
                if section.virt_addr <= rva < section.virt_addr + len(section.data):
                    offset = rva - section.virt_addr
                    relocations[pe_file.image_base + rva] = int.from_bytes(section.data[offset:offset + 4], 'little')
                    break
        pos += block_size

    return relocations


def extract_slice(pe_file: PE, slice_file: SliceFile, slice: Slice, syms: dict[str, int], base_relocations: dict[int, int]) -> COFF:
    baseAddr = slice_file.meta.baseAddr

    coff_file = COFF()

    # .drectve section
    linker_flags = '-defaultlib:MSL_All_x86 -defaultlib:gdi32 -defaultlib:kernel32 -defaultlib:user32 '
    drectve_sec = COFFSection()
    drectve_sec.sec_name = '.drectve'
    drectve_sec.flags = 0x100a00
    drectve_sec.data = bytearray(linker_flags.encode())
    coff_file.sections.append(drectve_sec)

    addr_to_sym: dict[int, str] = {v: k for k, v in syms.items()}
    used_names = set(addr_to_sym.values())

    dbg = cast(DebugDirectoryEntry, pe_file.data_directory[ImageDirectoryType.IMAGE_DIRECTORY_ENTRY_DEBUG])
    for s in dbg.codeview.modules:
        for (sec_idx, sym_addr, name) in s.symbols:
            virt_addr = pe_file.sections[sec_idx].virt_addr + sym_addr + baseAddr
            if virt_addr in addr_to_sym:
                continue
            if name in used_names or name.startswith('.'):
                name = f'__slice_{virt_addr:08x}'
            addr_to_sym[virt_addr] = name
            used_names.add(name)

    for target in base_relocations.values():
        addr_to_sym.setdefault(target, f'__reloc_{target:08x}')

    sorted_sym_addrs = sorted(addr_to_sym.keys())

    actual_sec_idx = 0

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True

    for sec in slice.sliceSecs:
        if sec.sec_name == '.reloc':
            continue

        pe_sec = pe_file.sections[sec.sec_idx]
        start_offs = sec.start_offs - pe_sec.virt_addr - baseAddr
        end_offs = sec.end_offs - pe_sec.virt_addr - baseAddr
        sec_data = pe_sec.data[start_offs:end_offs]
        coff_sec = COFFSection()
        coff_sec.sec_name = sec.sec_name
        coff_sec.data = bytearray(sec_data)
        coff_sec.size = sec.end_offs - sec.start_offs
        coff_sec.flags = pe_sec.flags | IMAGE_SCN_ALIGN_1BYTES
        coff_file.sections.append(coff_sec)

        sym = COFFSymbol()
        sym.name = sec.sec_name
        sym.value = 0
        sym.section_number = actual_sec_idx + 2
        sym.storage_class = 3
        coff_file.symbols.append(sym)

        if sec.sec_name == '.bss' and coff_sec.size % 4:
            # mwld aligns section contributions; a common symbol preserves the odd BSS tail.
            common = COFFSymbol()
            common.name = f'__bss_tail_{sec.start_offs:08x}'
            common.value = coff_sec.size % 8
            common.storage_class = 2
            coff_sec.size -= common.value
            coff_file.symbols.append(common)

        for addr in addr_to_sym:
            if sec.start_offs <= addr < sec.end_offs:
                sym = COFFSymbol()
                sym.name = addr_to_sym[addr]
                sym.value = addr - sec.start_offs
                sym.type = 0x20 if coff_sec.flags & 0x00000020 != 0 else 0x0 # 0x00000020 = IMAGE_SCN_CNT_CODE
                sym.section_number = actual_sec_idx + 2
                sym.storage_class = 2
                coff_file.symbols.append(sym)

        relocation_offsets = set()

        def add_relocation(offset: int, addend: int, target: int, is_absolute: bool) -> None:
            if offset in relocation_offsets:
                return

            name = addr_to_sym[target]
            coff_symbol = COFFSymbol()
            coff_symbol.name = name
            coff_symbol.type = 0x20 if coff_sec.flags & 0x00000020 != 0 else 0x0 # 0x00000020 = IMAGE_SCN_CNT_CODE
            coff_symbol.storage_class = 0x2

            if not coff_symbol in coff_file.symbols:
                coff_file.symbols.append(coff_symbol)
            symbol_idx = coff_file.symbols.index(coff_symbol)

            coff_relocation = COFFRelocation()
            coff_relocation.address = offset
            coff_relocation.sym_index = symbol_idx
            coff_relocation.type = 0x0006 if is_absolute else 0x0014
            coff_sec.relocations.append(coff_relocation)
            relocation_offsets.add(offset)
            coff_sec.data[offset:offset+4] = addend.to_bytes(4, byteorder='little', signed=True)

        for location, target in base_relocations.items():
            if sec.start_offs <= location < sec.end_offs:
                add_relocation(location - sec.start_offs, 0, target, True)

        if not slice.sliceName.startswith('filler') and sec.sec_name == '.text':
            for insn in md.disasm(sec_data, sec.start_offs):
                found = False
                for reloc in slice.addRelocations:
                    if insn.address == reloc.location:
                        is_absolute = not insn.group(capstone.x86.X86_GRP_BRANCH_RELATIVE)
                        section_offset = insn.address - sec.start_offs
                        addend = reloc.offset
                        add_relocation(section_offset + 1, addend, reloc.symbol, is_absolute)
                        found = True
                        break

                if found:
                    continue

                # print(f'{insn.address:x}:\t{insn.mnemonic}\t{insn.op_str}')
                if insn.group(capstone.x86.X86_GRP_CALL):
                    target = insn.operands[0].value.imm
                    if target in addr_to_sym:
                        is_absolute = not insn.group(capstone.x86.X86_GRP_BRANCH_RELATIVE)
                        section_offset = insn.address - sec.start_offs
                        add_relocation(section_offset + 1, 0, target, is_absolute)

                if not insn.group(capstone.x86.X86_GRP_BRANCH_RELATIVE):
                    for op in insn.operands:
                        if op.type == capstone.x86.X86_OP_MEM and baseAddr < op.value.mem.disp < 0x1000000:
                            val = op.value.mem.disp
                            offset = insn.disp_offset

                            next_addr_below = sorted_sym_addrs[bisect_left(sorted_sym_addrs, val + 1) - 1]
                            name = addr_to_sym[next_addr_below]
                            section_offset = insn.address - sec.start_offs
                            addend = val - next_addr_below
                            is_absolute = True
                            if addend < 0x8000:
                                add_relocation(section_offset + offset, addend, next_addr_below, is_absolute)

        actual_sec_idx += 1

    return coff_file


def slice_exe(exe_file: Path, out_path: Path, symbol_file: Path) -> None:
    assert exe_file.is_file()

    syms = format_symbols(symbol_file)

    # Read slices
    with open(exe_file, 'rb') as f:
        slice_file: SliceFile = load_slice_file((CONFIGDIR / exe_file.stem).with_suffix('.json'))
        pe = PE(file=f)
        base_relocations = read_base_relocations(pe)

        for slice in slice_file.parsed_slices:
            if not slice.source or slice.nonMatching:
                slice_coff = extract_slice(pe, slice_file, slice, syms, base_relocations)
                out_filepath = out_path / slice_file.unit_name() / slice.sliceName
                out_filepath.parent.mkdir(parents=True, exist_ok=True)
                with open(out_filepath, 'wb') as f:
                    slice_coff.write(f)


if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Slices EXE files.')
    parser.add_argument('exe_file', type=Path, help='EXE file to be sliced.')
    parser.add_argument('-s', '--symbol_file', type=Path, required=True, help='Symbol file to be used.')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Path the slices will be stored to.')
    args = parser.parse_args()
    slice_exe(args.exe_file, args.output, args.symbol_file)
