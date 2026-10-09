#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char symname[64] ; // symbol name, ex: PC
    int value ;        // the value, usually be its addr
    int length ;       // length of the value
    int number ;       // the register number, only standrad register has this field
    int type ;         // 0 for immediate value, 1 for address
    int belong ;       // the symbol belog which code segment, -1 for globle
} SYMBOL ;

typedef struct {
    SYMBOL *symboltab ;
    int SYM_size ;
} SYMTAB ;

int Init_symtab( SYMTAB *symtab )
{
    symtab->SYM_size = 11 ;
    symtab->symboltab = ( SYMBOL* )calloc( symtab->SYM_size , sizeof( SYMBOL ) ) ;
    if ( !symtab->symboltab )
        return 0 ;

    strcpy( symtab->symboltab[0].symname  , "A"  ) ; symtab->symboltab[0].length  = 2 ; symtab->symboltab[0].value  = 0 ; symtab->symboltab[0].number  = 0x0 ; symtab->symboltab[0].type  = 1 ; symtab->symboltab[0].belong  = -1 ;
    strcpy( symtab->symboltab[1].symname  , "X"  ) ; symtab->symboltab[1].length  = 2 ; symtab->symboltab[1].value  = 0 ; symtab->symboltab[1].number  = 0x1 ; symtab->symboltab[1].type  = 1 ; symtab->symboltab[1].belong  = -1 ;
    strcpy( symtab->symboltab[2].symname  , "L"  ) ; symtab->symboltab[2].length  = 2 ; symtab->symboltab[2].value  = 0 ; symtab->symboltab[2].number  = 0x2 ; symtab->symboltab[2].type  = 1 ; symtab->symboltab[2].belong  = -1 ;
    strcpy( symtab->symboltab[3].symname  , "B"  ) ; symtab->symboltab[3].length  = 2 ; symtab->symboltab[3].value  = 0 ; symtab->symboltab[3].number  = 0x3 ; symtab->symboltab[3].type  = 1 ; symtab->symboltab[3].belong  = -1 ;
    strcpy( symtab->symboltab[4].symname  , "S"  ) ; symtab->symboltab[4].length  = 2 ; symtab->symboltab[4].value  = 0 ; symtab->symboltab[4].number  = 0x4 ; symtab->symboltab[4].type  = 1 ; symtab->symboltab[4].belong  = -1 ;
    strcpy( symtab->symboltab[5].symname  , "T"  ) ; symtab->symboltab[5].length  = 2 ; symtab->symboltab[5].value  = 0 ; symtab->symboltab[5].number  = 0x5 ; symtab->symboltab[5].type  = 1 ; symtab->symboltab[5].belong  = -1 ;
    strcpy( symtab->symboltab[6].symname  , "F"  ) ; symtab->symboltab[6].length  = 4 ; symtab->symboltab[6].value  = 0 ; symtab->symboltab[6].number  = 0x6 ; symtab->symboltab[6].type  = 1 ; symtab->symboltab[6].belong  = -1 ;
    strcpy( symtab->symboltab[7].symname  , ""   ) ; symtab->symboltab[7].length  = 0 ; symtab->symboltab[7].value  = 0 ; symtab->symboltab[7].number  = 0x0 ; symtab->symboltab[7].type  = 1 ; symtab->symboltab[7].belong  = -1 ;
    strcpy( symtab->symboltab[8].symname  , "PC" ) ; symtab->symboltab[8].length  = 2 ; symtab->symboltab[8].value  = 0 ; symtab->symboltab[8].number  = 0x8 ; symtab->symboltab[8].type  = 1 ; symtab->symboltab[8].belong  = -1 ;
    strcpy( symtab->symboltab[9].symname  , "SW" ) ; symtab->symboltab[9].length  = 2 ; symtab->symboltab[9].value  = 0 ; symtab->symboltab[9].number  = 0x9 ; symtab->symboltab[9].type  = 1 ; symtab->symboltab[9].belong  = -1 ;
    strcpy( symtab->symboltab[10].symname , "*"  ) ; symtab->symboltab[10].length = 0 ; symtab->symboltab[10].value = 0 ; symtab->symboltab[10].number = 0xA ; symtab->symboltab[10].type = 1 ; symtab->symboltab[10].belong = -1 ;
    return 1 ;
}

SYMBOL FindSymbol( const char *symbol , SYMTAB symtab )
{
    char tpsym[128] = "" ;
    strcpy( tpsym , symbol ) ;
    char *temp = tpsym ;
    if ( strchr( temp , '@' ) || strchr( temp , '#' ) )
        temp += 1 ;
    SYMBOL null = { .symname = "" , .value = 0X00 , .length = -1 } ;
    for ( int i = 0 ; i < symtab.SYM_size ; ++i )
        if ( strcmp( symtab.symboltab[i].symname , temp ) == 0 )
            return symtab.symboltab[i] ;
    return null ;
}

SYMTAB Add_symbol( SYMTAB *symtab , SYMBOL add )
{
    symtab->symboltab = ( SYMBOL * )realloc( symtab->symboltab , (symtab->SYM_size+1)*sizeof( SYMBOL ) ) ;
    if ( !symtab->symboltab )
    {
        fprintf( stderr , "System error, type : out of memeory.\n" ) ;
        free( symtab->symboltab ) ;
        exit( EXIT_FAILURE ) ;
    }
    SYMBOL tp_sy = FindSymbol( add.symname , *symtab ) ;
    if ( tp_sy.length < 0 )
    {
        symtab->symboltab[symtab->SYM_size++] = add ;
    }
    return *symtab ;
}

void Modifysymbol( SYMTAB *symtab , const char *origin , SYMBOL new_sym  )
{
    if ( strcmp( origin , new_sym.symname ) )
    {
        fprintf( stderr , "System error, type : not valid modification in the symbol table.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    
    for ( int i = 0 ; i < symtab->SYM_size ; ++i )
    {
        if( strcmp( symtab->symboltab[i].symname , origin ) == 0 )
        {
            symtab->symboltab[i] = new_sym ;
            return ;
        }
    }
    fprintf( stderr , "System error, type : not valid modification in the symbol table.\n" ) ;
    exit( EXIT_FAILURE ) ;
}

void PrintSymbol( SYMTAB symtab )  // for debugging
{
    for( int i = 0 ; i < symtab.SYM_size ; ++i )
        printf( "%s\t%X\t%.6X\t%d\ttype = %s, belong : %d\n", symtab.symboltab[i].symname , symtab.symboltab[i].length , symtab.symboltab[i].value , symtab.symboltab[i].number , symtab.symboltab[i].type == 0 ? "numeric" : "address" , symtab.symboltab[i].belong ) ;
    return ;
}

#endif