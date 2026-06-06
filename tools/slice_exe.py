#!/usr/bin/env python3
# Slices a DOL file into .o files

from pathlib import Path
from typing import cast

from color_term import *
from coff import COFF, COFFSection, COFFSymbol, COFFRelocation
from peconsts import ImageDirectoryType
from pefile import PE, DebugDirectoryEntry
from project_settings import *
from slicelib import *

def extract_slice(pe_file: PE, slice: Slice, syms: dict[str, int]) -> COFF:
    coff_file = COFF()

    # .drectve section
    linker_flags = '-defaultlib:MSL_All_x86 -defaultlib:gdi32 -defaultlib:kernel32 -defaultlib:user32 '
    drectve_sec = COFFSection()
    drectve_sec.sec_name = '.drectve'
    drectve_sec.flags = 0x100a00
    drectve_sec.data = bytearray(linker_flags.encode())
    coff_file.sections.append(drectve_sec)

    for sec in slice.sliceSecs:
        pe_sec = pe_file.sections[sec.sec_idx]
        sec_data = pe_sec.data[sec.start_offs:sec.end_offs]
        coff_sec = COFFSection()
        coff_sec.sec_name = sec.sec_name
        coff_sec.data = bytearray(sec_data)
        coff_sec.flags = pe_sec.flags
        coff_file.sections.append(coff_sec)

        sym = COFFSymbol()
        sym.name = sec.sec_name
        sym.value = 0
        sym.section_number = sec.sec_idx + 1
        sym.storage_class = 3
        coff_file.symbols.append(sym)

        dbg = cast(DebugDirectoryEntry, pe_file.data_directory[ImageDirectoryType.IMAGE_DIRECTORY_ENTRY_DEBUG])
        for s in dbg.codeview.modules:
            for (sec_idx, sym_addr, name) in s.symbols:
                if sec.sec_idx == sec_idx and sec.start_offs <= sym_addr < sec.end_offs:
                    sym = COFFSymbol()
                    sym.name = name
                    sym.value = sym_addr - sec.start_offs
                    sym.type = 0x20
                    sym.section_number = sec.sec_idx + 2
                    sym.storage_class = 2
                    coff_file.symbols.append(sym)

        for name in syms:
            addr = syms[name]
            if sec.start_offs <= addr - 0x400000 - pe_sec.virt_addr < sec.end_offs:
                sym = COFFSymbol()
                sym.name = name
                sym.value = addr - 0x400000 - pe_sec.virt_addr - sec.start_offs
                sym.type = 0x20
                sym.section_number = sec.sec_idx + 2
                sym.storage_class = 2
                coff_file.symbols.append(sym)

        inverse_syms = {v: k for k, v in syms.items()}

        for offset in range(0, len(sec_data) - 4):
            ptr = int.from_bytes(
                sec_data[offset : offset + 4], byteorder="little"
            )

            def add_relocation(ptr, absolute):
                name = inverse_syms[ptr]
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
                coff_relocation.type = 0x0006 if absolute else 0x0014
                coff_sec.relocations.append(coff_relocation)

                coff_sec.data[offset : offset + 4] = bytes(0x4)

            if ptr in inverse_syms:
                add_relocation(ptr, True)

            displacement = int.from_bytes(
                coff_sec.data[offset : offset + 4], byteorder="little", signed=True
            )

            relative_ptr = offset + sec.start_offs + pe_sec.virt_addr + 0x400000 + 4 + displacement

            if relative_ptr in inverse_syms:
                add_relocation(relative_ptr, False)

    return coff_file


def slice_exe(exe_file: Path, out_path: Path, symbol_file: Path) -> None:
    assert exe_file.is_file()

    # TODO: use an actual symbol map file
    syms: dict[str, int] = {}

    for line in symbol_file.read_text().splitlines():
        if line.strip():
            sym, addr = line.split('=')
            if sym in syms:
                print_warn('Warning: symbol', sym, 'defined multiple times in', symbol_file.name, end='!\n')
            syms[sym.strip()] = int(addr, 16)

    # Read slices
    with open(exe_file, 'rb') as f:
        slice_file: SliceFile = load_slice_file((CONFIGDIR / exe_file.stem).with_suffix('.json'))
        pe = PE(file=f)

        for slice in slice_file.parsed_slices:
            if not slice.source or slice.nonMatching:
                slice_coff = extract_slice(pe, slice, syms)
                out_filepath = out_path / slice_file.unit_name() / slice.sliceName
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
