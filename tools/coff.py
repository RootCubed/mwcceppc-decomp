#!/usr/bin/env python3

# cofffile.py
# COFF definitions and helper functions
# See https://wiki.osdev.org/COFF

import struct
from typing import BinaryIO, Optional

def align(value: int, alignment: int) -> int:
    return (value + alignment - 1) & ~(alignment - 1)

class COFFSymbol:
    symbol_struct = struct.Struct('<8sIhHBB')
    symbol_ext_struct = struct.Struct('<IIIhHBB')

    def __init__(self) -> None:
        self.name: str = ''
        self.value: int = 0
        self.section_number: int = 0
        self.type: int = 0
        self.storage_class: int = 0
        self.aux_symbols: list[bytearray] = []

    def get_sym_count(self) -> int:
        return 1 + len(self.aux_symbols)

    def write(self, string_table_offset: int) -> tuple[bytearray, bytearray]:
        sym_sec_data = bytearray()
        string_sec_data = bytearray()

        if len(self.name) <= 8:
            name_bytes = self.name.encode().ljust(8, b'\0')
            sym_sec_data.extend(self.symbol_struct.pack(name_bytes, self.value, self.section_number, self.type, self.storage_class, len(self.aux_symbols)))
        else:
            name_bytes = self.name.encode() + b'\0'
            sym_sec_data.extend(self.symbol_ext_struct.pack(0, string_table_offset, self.value, self.section_number, self.type, self.storage_class, len(self.aux_symbols)))
            string_sec_data.extend(name_bytes)

        for aux in self.aux_symbols:
            sym_sec_data.extend(aux.ljust(18, b'\0'))

        return sym_sec_data, string_sec_data

    @staticmethod
    def from_coff_file(file: bytearray, info_offset: int) -> 'COFFSymbol':
        sym = COFFSymbol()

        hdr = COFFSymbol.symbol_struct.unpack_from(file, info_offset)
        s_name, s_value, s_section_number, s_type, s_storage_class, s_num_aux_symbols = hdr

        if not s_name.startswith(b'\0\0\0\0'):
            sym.name = s_name.rstrip(b'\0').decode()
        else:
            offset = struct.unpack_from('<I', file, info_offset + 4)[0]
            s_name = file[offset:].split(b'\0', 1)[0]
            sym.name = s_name.decode()

        sym.value = s_value
        sym.section_number = s_section_number
        sym.type = s_type
        sym.storage_class = s_storage_class
        for i in range(s_num_aux_symbols):
            aux_offset = info_offset + COFFSymbol.symbol_struct.size + i * 18
            sym.aux_symbols.append(file[aux_offset:aux_offset+18])

        print(f'Loaded symbol {sym.name} with value 0x{sym.value:x} in section {sym.section_number}')
        return sym

class COFFRelocation:
    relocation_struct = struct.Struct('<IIH')

    address: int
    sym_index: int
    type: int

    pass

class COFFSection:
    section_header_struct = struct.Struct('<8sIIIIIIHHI')

    def __init__(self) -> None:
        self.sec_name: str = ''
        self.data: bytearray = bytearray()
        self.size: int | None = None
        self.flags: int = 0
        self.relocations: list[COFFRelocation] = []

    def __repr__(self) -> str:
        return f'<Section {self.sec_name}: length {len(self.data):0x}>'

    @staticmethod
    def from_coff_file(file: bytearray, info_offset: int) -> 'COFFSection':
        sec = COFFSection()

        hdr = COFFSection.section_header_struct.unpack_from(file, info_offset)
        s_name, s_paddr, s_vaddr, s_size, s_scnptr, s_relptr, s_lnnoptr, s_nreloc, s_nlnno, s_flags = hdr

        sec.sec_name = s_name.rstrip(b'\0').decode()
        sec.flags = s_flags

        sec.data = file[s_scnptr:s_scnptr+s_size]
        print(f'Loaded section {sec.sec_name} with length 0x{s_size:x}')
        return sec


class COFF:
    file_header = struct.Struct('<HHIIIHH')
    section_header = struct.Struct('<8sIIIIIIHHI')

    def __init__(self, file: Optional[BinaryIO]=None) -> None:
        self.sections: list[COFFSection] = []
        self.symbols: list[COFFSymbol] = []

        if file:
            self.read(file)

    def read(self, file: BinaryIO) -> None:
        bytes = bytearray(file.read())
        hdr = self.file_header.unpack_from(bytes, 0)
        f_magic, f_nscns, f_timdat, f_symptr, f_nsyms, f_opthdr, f_flags = hdr

        for i in range(f_nscns):
            offset = self.file_header.size + i * 40
            self.add_section(COFFSection.from_coff_file(bytes, offset))

        self.bss_addr, self.bss_len, self.entry = struct.unpack_from('>III', bytes, 0xD8)

    def add_section(self, section: COFFSection) -> None:
        self.sections.append(section)

    def write(self, file: BinaryIO) -> None:
        sec_offsets: list[int] = []
        reloc_offsets: list[int] = []
        reloc_counts: list[int] = []
        sec_lengths: list[int] = []

        pos = self.file_header.size + len(self.sections) * self.section_header.size
        for sec in self.sections:
            sec_offsets.append(pos)
            sec_lengths.append(sec.size or len(sec.data))
            pos += len(sec.data)
            reloc_offsets.append(pos)
            reloc_counts.append(len(sec.relocations))
            pos += len(sec.relocations) * COFFRelocation.relocation_struct.size

        sym_sec_offset = pos
        true_sym_count = sum(s.get_sym_count() for s in self.symbols)
        string_table_offset = sym_sec_offset + true_sym_count * COFFSymbol.symbol_struct.size + 4

        sym_sec_data = bytearray()
        string_sec_data = bytearray()
        string_sec_data.extend(b'\0' * 4) # Replace later

        for sym in self.symbols:
            sym_sec, string_sec = sym.write(len(string_sec_data))
            sym_sec_data.extend(sym_sec)
            string_sec_data.extend(string_sec)
            string_table_offset += len(string_sec)

        file.write(self.file_header.pack(0x14c, len(self.sections), 0, sym_sec_offset, true_sym_count, 0, 0))
        for i, sec in enumerate(self.sections):
            sec_name = sec.sec_name.encode()
            sec_hdr_data = self.section_header.pack(sec_name, 0, 0, sec_lengths[i], sec_offsets[i], reloc_offsets[i], reloc_offsets[i], reloc_counts[i], 0, sec.flags)
            file.write(sec_hdr_data)

        for i, sec in enumerate(self.sections):
            file.seek(sec_offsets[i])
            file.write(sec.data)

        for i, sec in enumerate(self.sections):
            file.seek(reloc_offsets[i])
            for relocation in sec.relocations:
                reloc_data = relocation.relocation_struct.pack(relocation.address, relocation.sym_index, relocation.type)
                file.write(reloc_data)

        file.seek(sym_sec_offset)
        file.write(sym_sec_data)
        string_sec_data[0:4] = struct.pack('<I', len(string_sec_data))
        file.write(string_sec_data)

