#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path


def pe_offsets(data: bytearray) -> tuple[int, int, int]:
    pe_offset = struct.unpack_from('<I', data, 0x3c)[0]
    if data[pe_offset:pe_offset + 4] != b'PE\0\0':
        raise ValueError('not a PE file')

    optional_offset = pe_offset + 24
    if struct.unpack_from('<H', data, optional_offset)[0] != 0x10b:
        raise ValueError('only PE32 files are supported')

    timestamp_offset = pe_offset + 8
    debug_directory_offset = optional_offset + 96 + 6 * 8
    return pe_offset, timestamp_offset, debug_directory_offset


def rva_to_offset(data: bytearray, pe_offset: int, rva: int) -> int:
    section_count = struct.unpack_from('<H', data, pe_offset + 6)[0]
    optional_size = struct.unpack_from('<H', data, pe_offset + 20)[0]
    sections_offset = pe_offset + 24 + optional_size

    for index in range(section_count):
        section_offset = sections_offset + index * 40
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
            '<IIII', data, section_offset + 8
        )
        if virtual_address <= rva < virtual_address + max(virtual_size, raw_size):
            return raw_offset + rva - virtual_address
    raise ValueError(f'RVA 0x{rva:x} does not belong to a section')


def restore(original_path: Path, linked_path: Path) -> None:
    original = bytearray(original_path.read_bytes())
    linked = bytearray(linked_path.read_bytes())
    original_pe, original_timestamp, original_debug = pe_offsets(original)
    _, linked_timestamp, linked_debug = pe_offsets(linked)

    linked[linked_timestamp:linked_timestamp + 4] = original[original_timestamp:original_timestamp + 4]
    linked[linked_debug:linked_debug + 8] = original[original_debug:original_debug + 8]

    debug_rva, debug_size = struct.unpack_from('<II', original, original_debug)
    debug_offset = rva_to_offset(original, original_pe, debug_rva)
    for entry_offset in range(debug_offset, debug_offset + debug_size, 28):
        debug_type, data_size = struct.unpack_from('<II', original, entry_offset + 12)
        data_offset = struct.unpack_from('<I', original, entry_offset + 24)[0]
        if debug_type == 2:  # IMAGE_DEBUG_TYPE_CODEVIEW
            linked[data_offset:] = original[data_offset:]
            linked_path.write_bytes(linked)
            return

    raise ValueError('original executable has no CodeView debug data')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Restore non-reproducible PE data after linking.')
    parser.add_argument('original', type=Path)
    parser.add_argument('linked', type=Path)
    args = parser.parse_args()
    restore(args.original, args.linked)
