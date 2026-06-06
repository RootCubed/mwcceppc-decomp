#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys
from pathlib import Path

from tools.project_settings import *
from tools.slicelib import *
from tools.utils.ninja_syntax_ex import Writer as NinjaWriter

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

def gen_compile_commands(slice_file: SliceFile):
    commands = []
    directory = Path.cwd()
    for slice in slice_file.parsed_slices:
        if not slice.source:
            continue
        
        inc_dir_args = [f'-I{i}' for i in INCDIRS]
        output = (BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.o')
        flags = slice_file.meta.defaultCompilerFlags if slice.ccFlags == '' else slice.ccFlags
        file = slice.source
        flags = mwcc_to_clang(flags)
        arguments = ["/usr/bin/clang", "-c", str(file), "-o", str(output), "-D__INTEL__", "-Wno-ignored-attributes", "-fdeclspec", *flags, *inc_dir_args]

        command = {
            "directory": str(directory),
            "file": str(file),
            "output": str(output),
            "arguments": arguments,
        }
        commands.append(command)
    
    encoder = json.encoder.JSONEncoder()
    compile_commands = encoder.encode(commands)
    Path("compile_commands.json").write_text(compile_commands)

def mwcc_to_clang(flags: str) -> list[str]:
    PASSTHROUGH = ["-O4", "-O3", "-O2", "-O1", "-O0"]
    ONE_STEP = {
    }
    TWO_STEP_LAMBDA = {
        "-align": lambda x : f'-fpack-struct={x}',
    }
    TWO_STEP_DICT = {
        "-enum": {
            "min": "-fshort-enums",
            "int": "-fno-short-enums"
        },
        "-inline": {
            "auto": "-finline-functions",
            "none": "-fno-inline-functions",
            "off": "-fno-inline-functions",
        },
        "-Cpp_exceptions": {
            "on": "-fcxx-exceptions",
            "off": "-fno-cxx-exceptions",
        }
    }

    out = []
    flags_split = flags.split()
    i = 0
    while True:
        if i >= len(flags_split):
            break

        flag = flags_split[i]
        if flag in PASSTHROUGH:
            out.append(flag)
            i += 1
            continue
        
        if flag in TWO_STEP_LAMBDA:
            out.append(TWO_STEP_LAMBDA[flag](flags_split[i + 1]))
            i += 2
            continue
        
        if flag in TWO_STEP_DICT:
            out.append(TWO_STEP_DICT[flag][flags_split[i + 1]])
            i += 2
            continue
        
        i += 1
            

    return out

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
            command=f'$python {FORMAT_SYMBOLS} $symbols && $python {SLICE_EXE} $in -s $symbols -o $out_dir',
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

# Generate compile_commands.json
gen_compile_commands(slice)

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
