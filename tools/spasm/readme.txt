nv-SPASM 0.34.1
(c) 2014-2015 Giuseppe Gatta, a.k.a. nextvolume <tails92@gmail.com>
-------------------------

1. What is this?
2. Features
3. How to use
4. License

1. What is this?
--------------------------

nv-SPASM is a stand-alone MIPS assembler that can output either a little-endian flat binary file
or Sony PlayStation PS-X EXE executable (the default).

It is simple and lightweight, and it is ideal for people who just want to play around
with assembly without bothering with too many details which would arise with
more advanced tools, such as those provided by the GNU project.

nv-SPASM is also meant to be compatible with the Hitmen group's SPASM assembler,
which was an early tool for PlayStation homebrew programmers who had no access
to more advanced software development kits, such as Net Yaroze or PsyQ.

While to most a tool like SPASM isn't as useful as it was back in the day, due to the
availability of freely usable software development kits like the PSXSDK, as already
mentioned before there are still some use cases for which it's more feasible
to use nv-SPASM instead of more advanced tools.

And, why not? nv-SPASM is easier to hack and to learn from than more advanced,
complex designs. 

You could use it as the assembler for your virtual machine, or even embed this assembler inside it.

1. What is this not?
--------------------------

nv-SPASM isn't a complex and full-fledged macro assembler; in fact it does not
even support macros currently.

You can't easily mix code written in a high-level language with assembly code
with nv-SPASM, as nv-SPASM is unable to output object files that can be linked
against other object files.

nv-SPASM does not currently support complex expressions, just very simple ones.

2. Features
--------------------------

- Complete MIPS R3000 instruction set support, with the following pseudo-instructions
  implemented: 
  
  - B - unconditional branch
  - LA, LI - load label/integer inside a register (LA and LI are the same thing in nv-SPASM)
  - NOP - No operation
  - MOVE - Copy the contents of a register into another
  - SUBI - Subtract immediate from register
  - SUBIU - Subtract immediate unsigned from register
  - BEQZ - Branch if register is Equal to Zero
  - BNEZ - Branch if register is Not Equal to Zero
  - BAL - unconditional branch-and-link
  - ORG - move start address to the desired address
             it relocates every instruction, and must be the first instruction.
  - INCLUDE - include a source file 
  - INCBIN - include a binary file
  - DCB - declare a block of bytes
  - DB - declare bytes and/or strings
  - DH - declare half-words and/or strings
  - DW - declare words and/or strings
  - ALIGN - align incoming code to the specified boundary
  
- Compatibility with the Hitmen group's SPASM assembler and thus the possibility
  of assembling software developed with it, barring bugs in nv-SPASM or the fact
  that the original source code may depend on bugs in the Hitmen's SPASM assembler
  in order to assemble correctly.

- Very simple expression support, single operator, two operands.
  For laymen, this means that while for instance 3+3 is rendered as 6,
  3+3+2 is rendered as 5, as if it were 3+2.
  This is coherent with the original SPASM assembler.
  Supported operators are +, -, >>, <<, & and |
  
- Automatic generation of Sony PlayStation PS-X EXE executables.

- Symbol names (labels) can be treated just like normal numerical values,
  the numerical value being either the program counter value at the moment
  the label was met by the assembler, or a value set with an EQU or = statement.
  

3. How to use
--------------------------

nv-SPASM is really simple to use, and does not need any special explanation.
Therefore, it will suffice to report its usage banner.

nv-spASM version 0.34.1 (c) 2014-2015 nextvolume
Assembler for the Sony PlayStation

Usage: ./spasm [options] infile outfile

Options:   -b      create binary file instead of PS-X EXE
           -N      do not pad executable
           -S      be case sensitive on file paths

4. License
--------------------------

It is permitted to do (almost) anything with the nv-SPASM code and documentation.
The only restriction is that all existing copyright notices must remain intact, and
modifed versions must ship with the corresponding modified source code.

fcaseopen.c is (C) 2009 Keith Bauer. Read fcaseopen.c for the license (BSD-style)

5. Useful information
--------------------------

Hitmen's official website <http://www.hitmen-console.org>
- The README file for Hitmen's original SPASM assembler is a good read,
  as this assembler tries to closely follow it.
- The GreenTro source code is another good resource, as it was developed with SPASM,
  and thus it can be considered an example to write nv-SPASM assembly source code.
