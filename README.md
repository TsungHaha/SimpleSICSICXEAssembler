#SimpleSIC/SICXE Assembler
This project implements the SIC/SICXE Two-pass Assembler.

## Functions
The Assembler can:
1. support `SIC` and `SIC/XE` mode, but you shall name your program with following rules:
   * `.sic` for SIC mode
   * `.xe` for SIC/XE mode
2. output an human readable assembly list

## Hoe to Use
You can run the program like this:
   * .\sictomac [file_name.sic|.xe] [output_file_name]
Your output will appear in the `ans` directory.

## Supported Instructions:
* **Opcode**: SIC/XE standrad opcode
* **Pseudo instruction**: ex. `START`, `END`, `BYTE`, `WORD`, `RESB`, `RESW`, `LTORG`, `EQU`, `BASE`, `USE`
  - `EQU` support simple expression composed from `+ - * /`
  - `USE` can switch program section( use it with `END` )
* **Literal**: `=C'...'` and `=X'...'` expression.

## Limitation
I haven't do the following function:
* `ORG`, `EXTREF`
* `EXTDEF`, `CSECT` and Control Section Link
* `EQU` only support simple expression
* limited error response
