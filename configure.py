#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys
from pathlib import Path

sys.path.append('tools')

from project_settings import *
from slicelib import *
from utils.ninja_syntax_ex import Writer as NinjaWriter

####################
# Helper Functions #
####################

def get_build_path(slice_file: SliceFile, suffix: str) -> Path:
    return (BUILDDIR / slice_file.meta.fileName).with_suffix(suffix)


def ld_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if slice.source and not slice.nonMatching:
            files.append((BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.o'))
        else:
            files.append((BUILDDIR_SLICED / slice_file.unit_name() / slice.sliceName).with_suffix('.o'))
    return files


def sliced_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if not slice.source or slice.nonMatching:
            files.append((BUILDDIR_SLICED / slice_file.unit_name() / slice.sliceName).with_suffix('.o'))
    return files


def files_with_suffix(files: list[Path], suffix: str) -> list[Path]:
    return [f.with_suffix(suffix) for f in files]

##############################
# Step 1: Source Compilation #
##############################

def gen_compile_build_statements(writer: NinjaWriter, slice_file: SliceFile):
    compiled_files = []
    for slice in slice_file.parsed_slices:
        if not slice.source:
            continue

        compiled_o_file = (BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.o')
        compiled_files.append(compiled_o_file)
        ccflags = slice_file.meta.defaultCompilerFlags if slice.ccFlags == '' else slice.ccFlags
        writer.build('cw',
                        compiled_o_file,
                        (SRCDIR / slice.source).as_posix(),
                        ccflags=ccflags,
                        depfile=compiled_o_file.with_suffix('.dep').name)

###########################
# Step 2: EXE build steps #
###########################

def gen_exe_build_statements(writer: NinjaWriter, slice_file: SliceFile):
    assert slice_file.meta.type == SliceType.EXE

    # Slice EXE
    writer.build('slice_exe',
                    sliced_o_files(slice_file),
                    ORIGDIR / slice_file.meta.fileName,
                    symbols=SYMBOL_FILE,
                    implicit_inputs=[SYMBOL_FILE, slice_file.path],
                    out_dir = BUILDDIR_SLICED)

    # Objdiff
    writer.build('gen_objdiff',
                    'objdiff.json',
                    files_with_suffix(sliced_o_files(slice_file), '.o'),
                    implicit_inputs=files_with_suffix(sliced_o_files(slice_file), '.o'))

    # Linked EXE
    writer.build('link',
                    get_build_path(slice_file, '.exe'),
                    ld_o_files(slice_file),
                    ldflags='')

######################
# Build Script Setup #
######################

# Ninja build variables
writer = NinjaWriter()
writer.variable('builddir', BUILDDIR)
writer.newline()
writer.variable('python', f'"{sys.executable}"')
writer.variable('cc', f'$python {CW_WRAPPER} "{CC}"' if sys.platform != 'win32' else CC)
writer.variable('ld', f'{WINDOWS_WRAPPER} "{LD}"' if sys.platform != 'win32' else LD)
writer.newline()

# Ninja build rules
inc_dir_str = ' '.join([f'-i "{i}"' for i in INCDIRS])
writer.rule('cw',
            command=f'$cc -c $ccflags $in -o $out -MD -I- {inc_dir_str}',
            deps='gcc',
            depfile='$depfile',
            description='Compile $in')

writer.rule('slice_exe',
            command=f'$python {SLICE_EXE} $in -s $symbols -o $out_dir',
            description='Slice $in')

writer.rule('link',
            command='$python -c "exit(0)"', # Skip for now
            # command='$ld $ldflags $in -o $out',
            description='Link $out')

writer.rule('gen_objdiff',
            command=f'$python {GEN_OBJDIFF}',
            description='Generate objdiff')

writer.rule('configure',
            command=f'$python {sys.argv[0]}',
            generator=True,
            description='Configure $out')

# Load slice files and ensure correct slice order
slice: SliceFile = load_slice_file(SLICE_FILE)

# Generate build statements
gen_compile_build_statements(writer, slice)
gen_exe_build_statements(writer, slice)

# Regenerate build.ninja on changes to the slices
writer.build('configure',
             NINJA_BUILD_FILE,
             slice.path)

# Default targets (final EXE)
writer.default([BUILDDIR / slice.meta.fileName, 'objdiff.json'])

# Flush the created file
writer.flush(NINJA_BUILD_FILE)
