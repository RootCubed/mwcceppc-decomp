# MWCCEPPC decompilation

This repository aims to decompile the Metrowerks C/C++ compiler for Embedded PowerPC for GameCube and Wii.

It currently targets mwcceppc for Wii 1.1.

## Setup

1. Find a copy of CodeWarrior for Windows 9.2 and install it.

1. Find a copy of the Wii 1.1 `mwcceppc` compiler and copy the executable into `original`.

1. Copy the entire `CodeWarrior` directory into `compilers`.

1. Install capstone with `pip install capstone`.

1. Run `configure.py` to set everything up.

1. Run `ninja` to build the project. You should now be able to open the folder in objdiff.

## Relinking

The default build relinks the sliced COFF objects into `bin/mwcceppc.exe` and verifies it against the original. `mwld` cannot reproduce the original build timestamp or CodeView data, so `tools/restore_pe.py` copies those fields from `original/mwcceppc.exe` before comparison.
