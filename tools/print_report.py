#!/usr/bin/env python3

import argparse
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description='Print objdiff progress percentages.')
    parser.add_argument('report', type=Path)
    args = parser.parse_args()

    measures = json.loads(args.report.read_text())['measures']
    fuzzy = measures.get('fuzzy_match_percent', 0.0)
    complete = measures.get('matched_code_percent', 0.0)
    linked = measures.get('complete_code_percent', 0.0)
    print(f'Progress: {fuzzy:.2f}% fuzzy, {complete:.2f}% complete, {linked:.2f}% linked')


if __name__ == '__main__':
    main()
