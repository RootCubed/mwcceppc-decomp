from pathlib import Path

from color_term import *

def format_symbols(symbol_file: Path):
    # TODO: use an actual symbol map file
    syms: list[tuple[int, str]] = []

    orig = symbol_file.read_text()
    for line in orig.splitlines():
        if line.strip():
            sym, addr = line.split('=')
            if sym in syms:
                print_warn('Warning: symbol', sym, 'defined multiple times in', symbol_file.name, end='!\n')
            syms.append((int(addr, 16), sym.strip()))
    
    syms.sort(key=lambda val: val[0])
    lines = [f'{sym}={hex(addr)}' for (addr, sym) in syms]
    formatted = "\n".join(lines)

    if orig != formatted:
        symbol_file.write_text(formatted)

if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Formats symbol files.')
    parser.add_argument('symbol_file', type=Path, help='Symbol file to be formatted.')
    args = parser.parse_args()
    format_symbols(args.symbol_file)