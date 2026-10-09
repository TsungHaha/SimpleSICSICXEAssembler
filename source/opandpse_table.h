#ifndef _OPPSE_H
#define _OPPSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char text[64] ;
    int num ;
    int length ;
} OPCODE ;
typedef struct {
    char text[20] ;
    int function ;
} PSEUDO ;

const int Offset[26] =
                    /*A   B   C   D   E    F    G    H    I    J    K    L    M    N    O    P    Q    R    S    T    U    V    W    X    Y    Z */
                    { 0 , 4 , 4 , 8 , 11 , 11 , 13 , 13 , 14 , 14 , 19 , 19 , 28 , 31 , 32 , 33 , 33 , 33 , 36 , 54 , 58 , 58 , 58 , 59 , 59 , 59 } ;

const OPCODE optab[] = {
    { .text = "ADD"    , .num = 0X18 , .length = 3 } , // 0
    { .text = "ADDF"   , .num = 0X58 , .length = 3 } ,
    { .text = "ADDR"   , .num = 0X90 , .length = 2 } ,
    { .text = "AND"    , .num = 0X40 , .length = 3 } ,
    { .text = "CLEAR"  , .num = 0XB4 , .length = 2 } , // 4
    { .text = "COMP"   , .num = 0X28 , .length = 3 } ,
    { .text = "COMPF"  , .num = 0X88 , .length = 3 } ,
    { .text = "COMPR"  , .num = 0XA0 , .length = 2 } ,
    { .text = "DIV"    , .num = 0X24 , .length = 3 } , // 8
    { .text = "DIVF"   , .num = 0X64 , .length = 3 } ,
    { .text = "DIVR"   , .num = 0X9C , .length = 2 } ,
    { .text = "FIX"    , .num = 0XC4 , .length = 1 } , // 11
    { .text = "FLOAT"  , .num = 0XC0 , .length = 1 } ,
    { .text = "HIO"    , .num = 0XF4 , .length = 1 } , // 13
    { .text = "J"      , .num = 0X3C , .length = 3 } , // 14
    { .text = "JEQ"    , .num = 0X30 , .length = 3 } ,
    { .text = "JGT"    , .num = 0X34 , .length = 3 } ,
    { .text = "JLT"    , .num = 0X38 , .length = 3 } ,
    { .text = "JSUB"   , .num = 0X48 , .length = 3 } ,
    { .text = "LDA"    , .num = 0X00 , .length = 3 } , // 19
    { .text = "LDB"    , .num = 0X68 , .length = 3 } ,
    { .text = "LDCH"   , .num = 0X50 , .length = 3 } ,
    { .text = "LDF"    , .num = 0X70 , .length = 3 } ,
    { .text = "LDL"    , .num = 0X08 , .length = 3 } ,
    { .text = "LDS"    , .num = 0X6C , .length = 3 } ,
    { .text = "LDT"    , .num = 0X74 , .length = 3 } ,
    { .text = "LDX"    , .num = 0X04 , .length = 3 } ,
    { .text = "LPS"    , .num = 0XD0 , .length = 3 } ,
    { .text = "MUL"    , .num = 0X20 , .length = 3 } , // 28
    { .text = "MULF"   , .num = 0X60 , .length = 3 } ,
    { .text = "MULR"   , .num = 0X98 , .length = 2 } ,
    { .text = "NORM"   , .num = 0XC8 , .length = 1 } , // 31
    { .text = "OR"     , .num = 0X44 , .length = 3 } , // 32
    { .text = "RD"     , .num = 0XD8 , .length = 3 } , // 33
    { .text = "RMO"    , .num = 0XAC , .length = 2 } ,
    { .text = "RSUB"   , .num = 0X4C , .length = 3 } ,
    { .text = "SHIFTL" , .num = 0XA4 , .length = 2 } , // 36
    { .text = "SHIFTR" , .num = 0XA8 , .length = 2 } ,
    { .text = "SIO"    , .num = 0XF0 , .length = 1 } ,
    { .text = "SSK"    , .num = 0XEC , .length = 3 } ,
    { .text = "STA"    , .num = 0X0C , .length = 3 } ,
    { .text = "STB"    , .num = 0X78 , .length = 3 } ,
    { .text = "STCH"   , .num = 0X54 , .length = 3 } ,
    { .text = "STF"    , .num = 0X80 , .length = 3 } ,
    { .text = "STI"    , .num = 0XD4 , .length = 3 } ,
    { .text = "STL"    , .num = 0X14 , .length = 3 } ,
    { .text = "STS"    , .num = 0X7C , .length = 3 } ,
    { .text = "STSW"   , .num = 0XE8 , .length = 3 } ,
    { .text = "STT"    , .num = 0X84 , .length = 3 } ,
    { .text = "STX"    , .num = 0X10 , .length = 3 } ,
    { .text = "SUB"    , .num = 0X1C , .length = 3 } ,
    { .text = "SUBF"   , .num = 0X5C , .length = 3 } ,
    { .text = "SUBR"   , .num = 0X94 , .length = 2 } ,
    { .text = "SVC"    , .num = 0XB0 , .length = 2 } ,
    { .text = "TD"     , .num = 0XE0 , .length = 3 } , // 54
    { .text = "TIO"    , .num = 0XF8 , .length = 1 } ,
    { .text = "TIX"    , .num = 0X2C , .length = 3 } ,
    { .text = "TIXR"   , .num = 0XB8 , .length = 2 } ,
    { .text = "WD"     , .num = 0XDC , .length = 3 }  // 58
} ;

PSEUDO pseudotab[] = {
    { .text = "START"  , .function = 1  } ,
    { .text = "END"    , .function = 2  } ,
    { .text = "BYTE"   , .function = 3  } ,
    { .text = "WORD"   , .function = 4  } ,
    { .text = "RESB"   , .function = 5  } ,
    { .text = "RESW"   , .function = 6  } ,
    { .text = "LTORG"  , .function = 7  } ,
    { .text = "EQU"    , .function = 8  } ,
    { .text = "ORG"    , .function = 9  } ,
    { .text = "BASE"   , .function = 10 } ,
    { .text = "EXTREF" , .function = 11 } ,
    { .text = "USE"    , .function = 12 }
} ;

int FindPseudo( const char *pseudo )
{
    if ( !pseudo )
        return -1 ;
    int end = sizeof( pseudotab )/sizeof( PSEUDO ) ;
    for ( int i = 0 ; i < end ; ++i )
        if ( strcmp( pseudo , pseudotab[i].text ) == 0 )
            return pseudotab[i].function ;
    return -1 ;
}

OPCODE FindOPcode( const char *op )
{
    OPCODE result = { .text = "" , .num = 0XFF , .length = -1 } ;
    if ( !op )
        return result ;
    int form4 = 0 ;
    const char *temp = op ;
    if ( temp[0] == '+' )
    {
        temp += 1 ;
        form4 = 1 ;
    }
    int i = Offset[ temp[0] - 'A' ] , end = Offset[ temp[0] - 'A' + 1 ] ;
    for ( ; i < end ; ++i )
    {
        if ( strcmp( temp , optab[i].text ) == 0 )
        {
            result = optab[i] ;
            result.length += form4 ;
            return result ;
        }
    }
    return result ;
}

#endif