#!/usr/bin/env python3
# Setup project for use with objdiff

import argparse
import json
from pathlib import Path
import sys

sys.path.append('tools')

from color_term import *
from project_settings import *
from slicelib import SliceFile, load_slice_file

parser = argparse.ArgumentParser(description='Sets up the project for use with objdiff.')
args = parser.parse_args()

slice_file: SliceFile = load_slice_file(SLICE_FILE)

objdiff_json = {}

objdiff_json['custom_make'] = 'ninja'
objdiff_json['ignore_patterns'] = ['bin/**/*']
objdiff_json['watch_patterns'] = [
    '*.c', '*.cpp',
    '*.h', '*.hpp',
    '*.s', '*.S',
    '*.py',
    '*.json'
]
objdiff_json['units'] = []

for slice in slice_file.parsed_slices:
    stem = Path(slice_file.meta.fileName).stem
    unit = {
        'name': slice.sliceName,
        'metadata': {}
    }

    if slice.sliceName.startswith('filler_'):
        unit['metadata']['auto_generated'] = True
    elif (SRCDIR / slice.source).exists():
        unit['metadata']['source_path'] = f'source/{slice.source}'
        unit['base_path'] = (BUILDDIR_COMPILED / stem / slice.sliceName).as_posix()

    unit['target_path'] = (BUILDDIR_SLICED / stem / slice.sliceName).as_posix()

    objdiff_json['units'].append(unit)

with open(f'objdiff.json', 'w') as f:
    f.write(json.dumps(objdiff_json, indent=2))
