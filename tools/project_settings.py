#!/usr/bin/env python3
# Project settings

import shutil
from pathlib import Path

#######################
# Project directories #
#######################

# Config directory
CONFIGDIR = Path('config')

# Build artifact directories
BUILDDIR = Path('bin')
BUILDDIR_COMPILED = Path(BUILDDIR, 'compiled')
BUILDDIR_SLICED = Path(BUILDDIR, 'sliced')
BUILDDIR_DECOMPCTX = Path(BUILDDIR, 'decompctx')

# Original binaries directory
ORIGDIR = Path('original')

# Compiler directory
COMPILERDIR = Path('compilers')

# Tool directory
TOOLDIR = Path('tools')

# Directory for decompiled code
SRCDIR = Path('source')

# Include directory
INCDIRS = [
    Path('include'),
    Path(COMPILERDIR, 'CodeWarrior', 'MSL', 'MSL_C', 'MSL_Common', 'Include'),
    Path(COMPILERDIR, 'CodeWarrior', 'MSL', 'MSL_C', 'MSL_Win32', 'Include'),
    Path(COMPILERDIR, 'CodeWarrior', 'MSL', 'MSL_C', 'MSL_X86'),
    Path(COMPILERDIR, 'CodeWarrior', 'MSL', 'MSL_Extras', 'MSL_Common', 'Include'),
    Path(COMPILERDIR, 'CodeWarrior', 'MSL', 'MSL_Extras', 'MSL_Win32', 'Include'),
    Path(COMPILERDIR, 'CodeWarrior', 'Win32-x86 Support', 'Headers', 'Prefix Files'),
    Path(COMPILERDIR, 'CodeWarrior', '(CodeWarrior SDK)', 'Headers'),
]

#########
# Files #
#########

# Codewarrior
CODEWARRIOR = Path(COMPILERDIR, 'CodeWarrior', 'Other Metrowerks Tools', 'Command Line Tools')
CC = Path(CODEWARRIOR, 'mwcc.exe')
LD = Path(CODEWARRIOR, 'mwld.exe')

# Tools and scripts
CW_WRAPPER = Path(TOOLDIR, 'mwcceppc_wrapper.py')
SLICE_EXE = Path(TOOLDIR, 'slice_exe.py')
FORMAT_SYMBOLS = Path(TOOLDIR, 'format_symbols.py')
GEN_OBJDIFF = Path(TOOLDIR, 'generate_objdiff.py')

# Slices
SLICE_FILE = Path(CONFIGDIR, 'mwcceppc.json')

# Symbols
SYMBOL_FILE = Path(CONFIGDIR, 'symbols.txt')

# Ninja build script
NINJA_BUILD_FILE = Path('build.ninja')

# Windows wrapper for non-Windows OSes
WINDOWS_WRAPPER = 'wibo' if shutil.which('wibo') else 'wine'
