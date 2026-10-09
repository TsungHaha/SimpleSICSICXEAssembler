#ifndef _LITTAB_H
#define _LITTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern long int Stohex( const char *str ) ;

typedef struct {
    int addr ;         // litteral stored in where
    char name[64] ;    // the name of the litteral, ex. =C'EOF'
    int value ;        // the transformed value, ex. 454F46
    int length ;
    int visited ;
    int belong ;
} LITTERAL ;

typedef struct {
    LITTERAL *list ;
    int length ;
} LITTAB ;

void Add_litteral( LITTAB *littab , LITTERAL lit )
{
    lit.value = Stohex( lit.name ) ;
    lit.visited = 0 ;
    char *init = strchr( lit.name , '\'' ) ,
         *tail = strrchr( lit.name , '\'' ) ;
    if ( !init || !tail )
    {
        fprintf( stderr , "Your error, type : wrong code format for litteral.\n" ) ;
        exit(1) ;
    }
    lit.length = tail-init-1 ;
    if ( strstr( lit.name , "X\'" ) )
        lit.length /= 2 ;

    if ( !littab->length )
    {
        littab->list = ( LITTERAL * )calloc( 1 , sizeof(LITTERAL) ) ;
        if ( !littab->list )
        {
            fprintf( stderr , "System error, type : litteral overflow system.\n" ) ;
            exit(1) ;
        }
        ++littab->length ;
        littab->list[0] = lit ;

        // printf( "\t\taddr : %06X, len : %d, name : %s, val : %06X, vis : %d\n", pg->littab->list[0].addr , pg->littab->list[0].length , pg->littab->list[0].name , pg->littab->list[0].value , pg->littab->list[0].visited ) ; // test
        return ;
    }

    int len = littab->length ;
    for( int i = 0 ; i < len ; ++i )
    {
        if ( littab->list[i].value == lit.value )
            return ;
    }
    littab->list = ( LITTERAL * )realloc( littab->list , (len+1)*sizeof(LITTERAL) ) ;
    if ( !littab->list )
    {
        fprintf( stderr , "System error, type : litteral overflow system.\n" ) ;
        exit(1) ;
    }
    littab->list[len] = lit ;
    ++littab->length ;

    // printf( "\t\taddr : %06X, len : %d, name : %s, val : %06X, vis : %d\n", pg->littab->list[len].addr , pg->littab->list[len].length , pg->littab->list[len].name , pg->littab->list[len].value , pg->littab->list[len].visited ) ; // test
    return ;
}

void Put_litteral( FILE *fp , LITTAB *littab , int *Locctr , int belong )
{
    if ( !fp || !littab->list )
        return ;
    
    int len = littab->length ;
    for( int i = 0 ; i < len ; ++i )
    {
        if ( littab->list[i].visited == 0 )
        {
            littab->list[i].addr = *Locctr ;
            littab->list[i].belong = belong ;
            // printf( "\t\taddr : %06X, len : %d, name : %s, val : %06X, vis : %d\n", pg->littab->list[i].addr , pg->littab->list[i].length , pg->littab->list[i].name , pg->littab->list[i].value , pg->littab->list[i].visited ) ; // test
            fprintf( fp , "%05X %20s %20s %20s\t\t%04d\t%1d\t%d\n", *Locctr , "*" , littab->list[i].name , "(null)" , littab->list[i].length , 99 , littab->list[i].belong ) ;
            littab->list[i].visited = 1 ;
            *Locctr += littab->list[i].length ;
        }
    }
    return ;
}

LITTERAL Find_lit( const char *lit , LITTAB littab )
{
    if ( !littab.length )
    {
        fprintf( stderr , "Your error, type : you haven't define this litteral( %s ).\n", lit ) ;
        exit(1) ;
    }

    LITTERAL null = { .addr = 0 , .name = "" , .value = 0 , .length = 0 , .visited = 0 } ;
    int len = littab.length ;
    int value = Stohex( lit ) ;
    for( int i = 0 ; i < len ; ++i )
    {
        if ( littab.list[i].value == value )
            return littab.list[i] ;
    }
    return null ;
}

#endif