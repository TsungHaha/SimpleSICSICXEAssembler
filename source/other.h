#ifndef _OTHER_H
#define _OTHER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BYTE 1
#define WORD 3
#define TRECLEN 60

long int Stohex( const char *str )
{
    char buffer[64] = "" ;
    strcpy( buffer , str ) ;
    char *tp_str = buffer ;
    if ( strchr( buffer , '\'' ) )
    {
        tp_str = strchr( tp_str , '\'' )+1 ;
        char *end = strrchr( tp_str , '\'' ) ;
        *end = 0 ;
    }
    
    int ans = 0 ;
    if ( strstr( str , "C\'" ) )
    {
        int length = strlen( tp_str ) ;
        for ( int i = 0 ; i < length ; ++i )
        {
            ans <<= 8 ; // 1 Byte = 8 bits
            ans |= tp_str[i] & 0xFF ;
        }
    }
    else if ( strstr( str , "X\'" ) )
        ans = strtol( tp_str , NULL , 16 ) ;
    //printf( "\ttranslate : %s => %06X\n", str , ans ) ; //test
    return ans ;
}

typedef struct MODREC {
    int addr ;
    char sign ;
    int length ;
} MODREC ;

typedef struct {
    int specify ;
    int start ;
    int end ;
    int program_length ;
    char program_name[64] ;
    int Locctr ;
    MODREC *modRecord ;
    int modCount ;
} Program ;

typedef struct {
    int mask ;
    int last_bit ;
    int new_bit ;
    int len ;
} BitMask ;

Program Init_program()
{
    Program new_pg = {
        .specify = 0 ,
        .start = 0 ,
        .end = 0 ,
        .program_length = 0 ,
        .program_name = "HelloWorld" ,
        .Locctr = 0 ,
        .modRecord = NULL ,
        .modCount = 0
    } ;
    
    return new_pg ;
}



void Add_modrec( Program *pg , MODREC modinstruction )
{
    if ( !pg->modCount )
    {
        pg->modRecord = ( MODREC * )calloc( 1 , sizeof(MODREC) ) ;
        if ( !pg->modRecord )
        {
            fprintf( stderr , "System error, type : modRec list overflow system.\n" ) ;
            exit(1) ;
        }
        ++pg->modCount ;
        pg->modRecord[0] = modinstruction ;
        return ;
    }

    int len = pg->modCount ;
    pg->modRecord = ( MODREC * )realloc( pg->modRecord , (len+1)*sizeof( MODREC ) ) ;
    if ( !pg->modRecord )
    {
        fprintf( stderr , "System error, type : modRec list overflow system.\n" ) ;
        exit(1) ;
    }
    pg->modRecord[len] = modinstruction ;
    ++pg->modCount ;
    return ;
}

void Put_modrec( FILE *fp , Program pg )
{
    if ( !fp || !pg.modRecord )
        return ;
    int len = pg.modCount ;
    for( int i = 0 ; i < len ; ++i )
    {
        fprintf( fp , "M%06X%02X", pg.modRecord[i].addr+1 , pg.modRecord[i].length ) ;
        if ( pg.modRecord[i].sign != 0 )
            fprintf( fp , "%c", pg.modRecord[i].sign ) ;
        fprintf( fp , "\n" ) ;
    }
    return ;
}

void Show_ModRec( Program pg ) // debug
{
    for( int i = 0 ; i < pg.modCount ; ++i )
    {
        fprintf( stdout , "addr : %06X, length : %d\n", pg.modRecord[i].addr , pg.modRecord[i].length ) ;
    }
    return ;
}

#endif