#!/usr/bin/env python3
# Slices a DOL file into .o files

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

def extract_slice(pe_file: PE, slice_file: SliceFile, slice: Slice, syms: dict[str, int]) -> COFF:
    coff_file = COFF()

    # .drectve section
    linker_flags = '-defaultlib:MSL_All_x86 -defaultlib:gdi32 -defaultlib:kernel32 -defaultlib:user32 '
    drectve_sec = COFFSection()
    drectve_sec.sec_name = '.drectve'
    drectve_sec.flags = 0x100a00
    drectve_sec.data = bytearray(linker_flags.encode())
    coff_file.sections.append(drectve_sec)

    addr_to_sym: dict[int, str] = {v: k for k, v in syms.items()}

    dbg = cast(DebugDirectoryEntry, pe_file.data_directory[ImageDirectoryType.IMAGE_DIRECTORY_ENTRY_DEBUG])
    for s in dbg.codeview.modules:
        for (sec_idx, sym_addr, name) in s.symbols:
            virt_addr = pe_file.sections[sec_idx].virt_addr + sym_addr + slice_file.meta.baseAddr
            addr_to_sym[virt_addr] = name

    sorted_sym_addrs = sorted(addr_to_sym.keys())

    actual_sec_idx = 0

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True

    for sec in slice.sliceSecs:
        pe_sec = pe_file.sections[sec.sec_idx]
        sec_data = pe_sec.data[sec.start_offs:sec.end_offs]
        coff_sec = COFFSection()
        coff_sec.sec_name = sec.sec_name
        coff_sec.data = bytearray(sec_data)
        coff_sec.size = sec.end_offs - sec.start_offs
        coff_sec.flags = pe_sec.flags
        coff_file.sections.append(coff_sec)

        sym = COFFSymbol()
        sym.name = sec.sec_name
        sym.value = 0
        sym.section_number = actual_sec_idx + 2
        sym.storage_class = 3
        coff_file.symbols.append(sym)

        for addr in addr_to_sym:
            if sec.start_offs <= addr - slice_file.meta.baseAddr - pe_sec.virt_addr < sec.end_offs:
                sym = COFFSymbol()
                sym.name = addr_to_sym[addr]
                sym.value = addr - slice_file.meta.baseAddr - pe_sec.virt_addr - sec.start_offs
                sym.type = 0x20 if coff_sec.flags & 0x00000020 != 0 else 0x0 # 0x00000020 = IMAGE_SCN_CNT_CODE
                sym.section_number = actual_sec_idx + 2
                sym.storage_class = 2
                coff_file.symbols.append(sym)

        def add_relocation(offset: int, addend: int, target: int, is_absolute: bool) -> None:
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
            coff_sec.data[offset:offset+4] = addend.to_bytes(4, byteorder='little', signed=True)

        if not slice.sliceName.startswith('filler') and sec.sec_name == '.text':
            for insn in md.disasm(sec_data, pe_sec.virt_addr + sec.start_offs + slice_file.meta.baseAddr):
                found = False
                for reloc in slice.addRelocations:
                    if insn.address == reloc.location:
                        is_absolute = not insn.group(capstone.x86.X86_GRP_BRANCH_RELATIVE)
                        section_offset = insn.address - pe_sec.virt_addr - sec.start_offs - slice_file.meta.baseAddr
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
                        section_offset = insn.address - pe_sec.virt_addr - sec.start_offs - slice_file.meta.baseAddr
                        add_relocation(section_offset + 1, 0, target, is_absolute)

                if not insn.group(capstone.x86.X86_GRP_BRANCH_RELATIVE):
                    for op in insn.operands:
                        if op.type == capstone.x86.X86_OP_MEM and slice_file.meta.baseAddr < op.value.mem.disp < 0x1000000 \
                        or op.type == capstone.x86.X86_OP_IMM and slice_file.meta.baseAddr < op.value.imm < 0x1000000:
                            val = op.value.mem.disp if op.type == capstone.x86.X86_OP_MEM else op.value.imm
                            offset = insn.disp_offset if op.type == capstone.x86.X86_OP_MEM else insn.imm_offset

                            next_addr_below = sorted_sym_addrs[bisect_left(sorted_sym_addrs, val + 1) - 1]
                            name = addr_to_sym[next_addr_below]
                            section_offset = insn.address - pe_sec.virt_addr - sec.start_offs - slice_file.meta.baseAddr
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

        for slice in slice_file.parsed_slices:
            if not slice.source or slice.nonMatching:
                slice_coff = extract_slice(pe, slice_file, slice, syms)
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
