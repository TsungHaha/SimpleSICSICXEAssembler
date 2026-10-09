#include "opandpse_table.h"
#include "symbol_table.h"
#include "other.h"
#include "litteral.h"

int isXE = 0 ;
Program *program = NULL ;
int program_count = 0 ;


int main( int argc , char **argv )
{
    #pragma region INIT
    if ( argc < 3 )
    {
        fprintf( stderr , "tip : sictomac [SIC SIC/XE soruce code name] [machine code name]\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    FILE *source_file = fopen( argv[1] , "rb" ) ;
    if ( !source_file )
    {
        fprintf( stderr , "Can't find program %s, do you misspell it?\n", argv[1] ) ;
        exit( EXIT_FAILURE ) ;
    }
    FILE *byproduct_file = fopen( "ans/byproduct_file.txt" , "wb" ) ;
    if ( !byproduct_file )
    {
        fprintf( stderr , "System error, type : some file can't open.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    char machine_code_fpname[1024] = "" ;
    sprintf( machine_code_fpname , "%s%s", "ans/" , argv[2] ) ;
    FILE *machine_code = fopen( machine_code_fpname , "wb" ) ;
    if ( !machine_code )
    {
        fprintf( stderr , "System error, type : file %s can't open.\n", argv[2] ) ;
        exit( EXIT_FAILURE ) ;
    }
    FILE *readable_machine_code = fopen( "ans/result.txt" , "wb" ) ;
    if ( !readable_machine_code )
    {
        fprintf( stderr , "System error, type : some file can't open.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    if ( strstr( argv[1] , ".xe" ) )
        isXE = 1 ;
    else if ( strstr( argv[1] , ".sic" ) )
        isXE = 0 ;
    else
    {
        fprintf( stderr , "System error, type : undefined file type.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    
    SYMTAB symtab = { .symboltab = NULL , .SYM_size = 0 } ;
    if ( !Init_symtab( &symtab ) )
    {
        fprintf( stderr , "System error, type : can't creat symtab.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }

    program_count = 1 ;
    program = ( Program * )calloc( 1 , sizeof( Program ) ) ;
    program[0] = Init_program() ;

    LITTAB *littab = ( LITTAB * )calloc( 1 , sizeof(LITTAB) ) ;
    
    #pragma endregion

/******************************************************************************************************************************/
//                                                                                                                            //
//                                                      Pass 1                                                                //
//                                                                                                                            //
/******************************************************************************************************************************/
    #pragma region PASS1

    char instruction[1024] = "" ;
    int line = 0 ; // for debuging
    int sectionBelong = 0 ;
    while ( fgets( instruction , 1024 , source_file ) )
    {
        ++line ;
        int type = 0 ;
        int address = program[sectionBelong].Locctr ;
        if ( ( strchr( instruction , '.' ) ) ) // read a annotation
        {
            int annotation = strchr( instruction , '.' ) - &instruction[0] ;
            instruction[annotation] = '\0' ;
        }
        
        char col[3][128] ; // used for divide instruction string
        memset( col , 0 , sizeof( col ) ) ;
        int count = sscanf( instruction , "%s %s %s", col[0] , col[1] , col[2] ) ;
        if( count < 0 ) // if no input read, then skip
            continue ;

        SYMBOL new_star = { .symname = "*" , .value = address , .length = 0 , .number = 0XA } ; // the symbol *
        Modifysymbol( &symtab , "*" , new_star ) ;

        char *label = NULL , *op = NULL , *value = NULL ;
        int length = 0 ; // count the instruction length
        for( int i = 0 ; i < count ; ++i )
        {
            OPCODE tp_op = FindOPcode( col[i] ) ;
            if ( tp_op.length < 0 ) // not fount
            {
                int tp_ps = FindPseudo( col[i] ) ;
                if ( tp_ps > 0 ) // found pseudo code
                {
                    op = col[i] ;
                    switch ( tp_ps )
                    {
                        case 1 :
                        {
                            sectionBelong = 0 ;
                            if ( i < count-1 )
                            {
                                program[sectionBelong].Locctr = strtol( col[i+1] , NULL , 16 ) ;
                                program[sectionBelong].specify = 1 ;
                            }
                            else
                            {
                                program[sectionBelong].Locctr = 0 ;
                                program[sectionBelong].specify = 0 ;
                            }
                            program[sectionBelong].start = program[sectionBelong].Locctr ;
                            address = program[sectionBelong].Locctr ;
                            strcpy( program[sectionBelong].program_name , label ) ;
                            break ;
                        }
                        case 2 :
                        {
                            if ( i < count-1 )
                                value = col[i+1] ;
                            int pg_len = 0 ;
                            program[sectionBelong].end = address ;
                            for( int pg_i = 0 , next_start_addr = program[0].end ; pg_i < program_count ; ++pg_i )
                            {
                                program[pg_i].program_length = program[pg_i].end - program[pg_i].start ;
                                program[pg_i].start = pg_i == 0 ? program[pg_i].start : next_start_addr ;
                                program[pg_i].end = program[pg_i].start + program[pg_i].program_length ;
                                next_start_addr = program[pg_i].end ;
                                pg_len += program[pg_i].program_length ;
                            }
                            fprintf( machine_code , "H%-6s%06X%06X\n", program[0].program_name , program[0].start , pg_len ) ;
                            break ;
                        }
                        case 3 :
                        {
                            char *head = strchr( col[i+1] , '\'' ),
                                 *end  = strrchr( col[i+1] , '\'' ) ;
                            if ( !head || !end )
                            {
                                fprintf( stderr , "Your error, type : not a permit BYTE statement.\n" ) ;
                                exit( EXIT_FAILURE ) ;
                            }
                            else
                            {
                                length = end-head-1 ;
                                if ( strstr( col[i+1] , "X\'" ) )
                                    length /= 2 ;
                            }
                            break ;
                        }
                        case 4 : 
                        {
                            length = WORD ;
                            break ;
                        }
                        case 5 : 
                        {
                            length = strtol( col[i+1] , NULL , 10 )*BYTE ;
                            break ;
                        }
                        case 6 : 
                        {
                            length = strtol( col[i+1] , NULL , 10 )*WORD ;
                            break ;
                        }
                        case 8 : 
                        {
                            address = 0 ;
                            char buffer[128] ;
                            memset( buffer , 0 , sizeof(char) ) ;
                            strcpy( buffer , col[i+1] ) ; // buffer up the value in col[i+1]
                            value = buffer ; // use value to do operation
                            SYMBOL tp_sy = FindSymbol( value , symtab ) ;
                            if ( tp_sy.length < 0 ) // can't find a symbol -> compund symbol
                            {
                                for( char temp[64] ; sscanf( value , "%[^+-*/]", temp ) != 0 ; ++value )
                                {
                                    value += strlen( temp ) ;
                                    tp_sy = FindSymbol( temp , symtab ) ;
                                    int position ;
                                    char sign = *( value-strlen(temp)-1 ) ;
                                    switch ( sign )
                                    {
                                        case '+' :
                                        {
                                            address += tp_sy.value ;
                                            break ;
                                        }
                                        case '-' :
                                        {
                                            address -= tp_sy.value ;
                                            break ;
                                        }
                                        case '*' :
                                        {
                                            address *= tp_sy.value ;
                                            break ;
                                        }
                                        case '/' :
                                        {
                                            address /= tp_sy.value ;
                                            break ;
                                        }
                                        case '(' : case ')' :
                                            break ;
                                        default :
                                        {
                                            address += tp_sy.value ;
                                            break ;
                                        }
                                    }
                                    if ( *value == 0 )
                                        break ;
                                }
                            }
                            else
                                address = tp_sy.value ;
                            break ;
                        }
                        case 12 : 
                        {
                            program[sectionBelong].end = address ;
                            if ( count == 1 ) // (null) USE (null)
                            {
                                sectionBelong = 0 ;
                                address = program[sectionBelong].Locctr ;
                                break ;
                            }
                            else
                            {
                                int find_pg = 0 , find = 0 ;
                                for (  ; find_pg < program_count ; ++find_pg  ) // if the program has been created ?
                                {
                                    if ( strcmp( program[find_pg].program_name , col[i+1] ) == 0 )
                                    {
                                        find = 1 ;
                                        sectionBelong = find_pg ;
                                        address = program[sectionBelong].Locctr ;
                                        break ;
                                    }
                                }
                                if ( find == 0 ) // never found
                                {
                                    
                                    program = ( Program * )realloc( program , (program_count+1)*sizeof(Program) ) ;
                                    if ( !program )
                                        exit(1) ;
                                    program[program_count] = Init_program() ;
                                    strcpy( program[program_count].program_name , col[i+1] ) ;
                                    sectionBelong = program_count ;
                                    ++program_count ;
                                    address = program[sectionBelong].Locctr ;
                                }
                            }
                            break ;
                        }
                        default :
                            break ;
                    }

                    if ( i < count )
                        value = col[i+1] ;
                    
                    if ( label )
                    {
                        SYMBOL new_sy = { .symname = "" , .length = length , .value = address , .number = 0 , .type = 1 , .belong = sectionBelong } ;
                        strcpy( new_sy.symname , label ) ;
                        if ( tp_ps == 8 )
                            new_sy.type = 0 ;
                        Add_symbol( &symtab , new_sy ) ;
                    }

                    type = tp_ps ;
                    break ;
                }
                else // is a label
                    label = col[i] ;
            }
            else // is a op code
            {
                type = 0 ;
                op = col[i] ;
                if ( i < count )
                {
                    value = col[i+1] ;
                    if ( label )
                    {
                        SYMBOL new_sy = { .symname = "" , .length = 0 , .value = address , .number = 0 , .type = 1 , .belong = sectionBelong } ;
                        strcpy( new_sy.symname , label ) ;
                        Add_symbol( &symtab , new_sy ) ;
                    }
                }
                length = tp_op.length ;
                break ;
            }
        }
        if ( strcmp( value , "" ) == 0 )
            value = NULL ;

        if ( !label )
            label = "(null)" ;
        if ( !value )
            value = "(null)" ;
        if ( value[0] == '=' )
        {
            LITTERAL new_lit = { .addr = 0 , .length = 0 , .name = "" , .value = 0 , .visited = 0 , .belong = sectionBelong } ;
            strcpy( new_lit.name , value ) ;
            Add_litteral( littab , new_lit ) ;
        }
        fprintf( byproduct_file , "%05X %20s %20s %20s\t\t%04d\t%1d\t%d\n", address , label , op , value , length , type , sectionBelong ) ;
        if ( type == 2 || type == 7 )
            Put_litteral( byproduct_file , littab , &program[sectionBelong].Locctr , sectionBelong ) ;
        program[sectionBelong].Locctr += length ;
        memset( instruction , 0 , sizeof( instruction ) ) ;
    }

    fclose( source_file ) ;
    fclose( byproduct_file ) ;
    line = 0 ;

    // PrintSymbol( symtab ) ; // test
    //for ( int i = 0 ; i < program_count ; ++i ) // test
      //  printf( "pro( %s ) : len = %d\n", program[i].program_name , program[i].end-program[i].start ) ;

    #pragma endregion

/******************************************************************************************************************************/
//                                                                                                                            //
//                                                      Pass 2                                                                //
//                                                                                                                            //
/******************************************************************************************************************************/
    #pragma region PASS2

    byproduct_file = fopen( "ans/byproduct_file.txt" , "rb" ) ;
    if ( !byproduct_file )
    {
        fprintf( stderr , "System error, type : some file can't open.\n" ) ;
        exit( EXIT_FAILURE ) ;
    }
    
    memset( instruction , 0 , sizeof( instruction ) ) ;
    
    if( isXE )
    {
        //
        //
        // #pragma region XEPRO
        //
        //
        char record[TRECLEN+1] ; // limit the text record length lower than 60, +1=>'\0'
        memset( record , 0 , sizeof( record ) ) ;
        int starting_addr = program[0].start ;
        
        while ( fgets( instruction , 1024 , byproduct_file ) )
        {
            int end = 0 ; // indicate the END pseudo code reached
            ++line ;
            int type = 0 ;
            int need_record = 0 ;
            int now_position = 0 , length = 0 ;
            int belong = -1 ;
            char label[64] = "" , op[64] = "" , value[64] = "" ;
            int count = sscanf( instruction , "%x %s %s %s %d %d %d", &now_position , label , op , value , &length , &type , &belong ) ;
            int mac_code = 0x00000000 ;
            
            if ( strcmp( label , "(null)" ) == 0 )
                strcpy( label , "" ) ;
            if ( strcmp( value , "(null)" ) == 0 )
                strcpy( value , "" ) ;

            if ( type == 0 ) // is op code
            {
                SYMBOL new_PC = { .symname = "PC" , .length = 2 , .value = now_position + length , .number = 0x8 , .belong = -1 } ;
                Modifysymbol( &symtab , "PC" , new_PC ) ; // update

                OPCODE tp_op = FindOPcode( op ) ;
                switch ( length )
                {
                    case 1 : // format 1
                    {
                        mac_code |= tp_op.num & 0xFF ;
                        break ;
                    }
                    case 2 : // format 2
                    {
                        mac_code |= tp_op.num & 0xFF ;
                        mac_code <<= 4 ;
                        char *A = NULL , *B = NULL ;
                        A = value ;
                        char *end = strchr( A , ',' ) ;
                        if( end )
                            *end++ = 0 ;
                        else
                            end = "" ;
                        B = end ;
                        SYMBOL As = FindSymbol( A , symtab ) ;
                        SYMBOL Bs = FindSymbol( B , symtab ) ;
                        mac_code |= As.number ;
                        mac_code <<= 4 ;
                        mac_code |= Bs.number ;
                        break ;
                    }
                    case 3 :
                    {
                        int n = 1 , i = 1 ;
                        int x = 0 ;
                        int b = 0 , p = 0 ;
                        int e = 0 ;
                        int disp = 0X000 ;
                        char *endptr = value ;
                        SYMBOL tp_PC = FindSymbol( "PC" , symtab ) ;
                        SYMBOL tp_B  = FindSymbol( "B"  , symtab ) ;
                        
                        char buffer[128] ;
                        strcpy( buffer , value ) ;
                        
                        mac_code |= tp_op.num & 0XFC ;
                        mac_code >>= 1 ; // return one bit => 1111 1100 -> 111 1110
                        if ( strchr( value , '@' ) )
                        {
                            strcpy( value , buffer+1 ) ;
                            n = 1 ; i = 0 ;
                        }
                        else if ( strchr( value , '#' ) )
                        {
                            strcpy( value , buffer+1 ) ;
                            n = 0 ; i = 1 ;
                        }
                        mac_code |= n ; mac_code <<= 1 ;
                        mac_code |= i ; mac_code <<= 1 ;
                        
                        if ( strstr( value , ",X" ) )
                        {
                            char *cut = strchr( value , ',' ) ;
                            *cut = 0 ;
                            x = 1 ;
                            mac_code |= x ;
                        }
                        mac_code <<= 1 ;

                        if ( ~n & i ) // if immediate relative addressing mode
                        {
                            if ( strstr( value , "C\'" ) || strstr( value , "X\'" ) )
                            {
                                char *temp = value ;
                                strcpy( value , temp += 2 ) ;
                                char *cut = strrchr( value , '\'' ) ;
                                *cut = 0 ;
                                disp |= Stohex( value ) ;
                            }
                            else
                                disp |= strtol( value , &endptr , 10 ) ; 
                        }

                        SYMBOL tp_sy = FindSymbol( value , symtab ) ;
                        if ( !disp && ( endptr - value == 0 ) /* # value */ && strcmp( value , "" ) != 0 )
                        // 如果 disp 裡還沒有值，且有 value 欄位
                        {
                            if ( tp_sy.length < 0 )
                            {
                                LITTERAL tp_lit = Find_lit( value , *littab ) ;
                                if ( tp_lit.visited >= 0 )
                                {
                                    tp_sy.value = tp_lit.addr ;
                                    tp_sy.belong = tp_lit.belong ;
                                }
                                else
                                {
                                    fprintf( stderr , "Your error, type : can't find symbol( %d : %s ).\n", line , value ) ;
                                    exit( EXIT_FAILURE ) ;
                                }
                            }
                            tp_sy.value += belong != tp_sy.belong ? program[tp_sy.belong].start : 0 ;

                            if ( ( disp = tp_sy.value - tp_PC.value ) >= -2047 && disp <= 2048 )
                            {
                                b = 0 ;
                                p = 1 ;
                            }
                            else if ( ( disp = tp_sy.value - tp_B.value ) >= 0 && disp <= 4095 )
                            {
                                b = 1 ;
                                p = 0 ;
                            }
                            else
                            {
                                fprintf( stderr , "Your error, type : need format 4 or imposible( %d : %s ).\n", line , instruction ) ;
                                exit( EXIT_FAILURE ) ;
                            }   
                        }
                        mac_code |= b ; mac_code <<= 1 ;
                        mac_code |= p ; mac_code <<= 1 ;
                
                        mac_code |= e ;
                        mac_code <<= 12 ;
                        
                        mac_code |= disp & 0X0FFF ;
                        strcpy( value , buffer ) ;
                        break ;
                    }
                    case 4 :
                    {
                        int n = 1 , i = 1 ;
                        int x = 0 ;
                        int b = 0 , p = 0 ;
                        int e = 1 ;
                        int disp = 0X00000 ;
                        char *endptr = value ;
                        SYMBOL tp_PC = FindSymbol( "PC" , symtab ) ;
                        SYMBOL tp_B  = FindSymbol( "B"  , symtab ) ;
                        
                        char buffer[128] ;
                        strcpy( buffer , value ) ;
                        
                        mac_code |= tp_op.num & 0XFC ;
                        mac_code >>= 1 ;
                        if ( strchr( value , '@' ) )
                        {
                            strcpy( value , buffer+1 ) ;
                            n = 1 ; i = 0 ;
                        }
                        else if ( strchr( value , '#' ) )
                        {
                            strcpy( value , buffer+1 ) ;
                            n = 0 ; i = 1 ;
                        }
                        mac_code |= n ; mac_code <<= 1 ;
                        mac_code |= i ; mac_code <<= 1 ;
                        
                        if ( strstr( value , ",X" ) )
                        {
                            char *cut = strchr( value , ',' ) ;
                            *cut = 0 ;
                            x = 1 ;
                            mac_code |= x ;
                        }
                        mac_code <<= 1 ;
                        mac_code <<= 2 ;

                        if ( ~n & i ) // if immediate relative addressing mode
                        {
                            if ( strstr( value , "C\'" ) || strstr( value , "X\'" ) )
                            {
                                char *temp = value ;
                                strcpy( value , temp += 2 ) ;
                                char *cut = strrchr( value , '\'' ) ;
                                *cut = 0 ;
                                disp |= Stohex( value ) & 0XFFFFF ;
                            }
                            else
                                disp |= strtol( value , &endptr , 10 ) ;
                        }

                        SYMBOL tp_sy = FindSymbol( value , symtab ) ;
                        if ( !disp && ( endptr - value == 0 ) && strcmp( value , "" ) != 0 )
                        {
                            if ( tp_sy.length < 0  )
                            {
                                LITTERAL tp_lit = Find_lit( value , *littab ) ;
                                if ( tp_lit.visited >= 0 )
                                {
                                    tp_sy.value = tp_lit.value ;
                                    tp_sy.belong = tp_lit.belong ;
                                }
                                else
                                {
                                    fprintf( stderr , "Your error, type : can't find symbol( %d : %s ).\n", line , value ) ;
                                    exit( EXIT_FAILURE ) ;
                                }
                            }
                            else if ( n )
                            {
                                MODREC tp_modrec = { .addr = now_position , .sign = 0 , .length = 5 } ;
                                Add_modrec( &program[0] , tp_modrec ) ;
                            }

                            disp |= tp_sy.value ;
                        }
                
                        mac_code |= e ;
                        mac_code <<= 20 ;

                        mac_code |= disp & 0XFFFFF ;
                        strcpy( value , buffer ) ;
                        break ;
                    }
                    default :
                        break ;
                }
                if ( program_count == 1 )
                    fprintf( readable_machine_code , "%05X %20s %20s %20s\t\t%0*X\n", now_position , label , op , value , length*2 , mac_code ) ;
                else
                    fprintf( readable_machine_code , "%05X %d %20s %20s %20s\t\t%0*X\n", now_position , belong , label , op , value , length*2 , mac_code ) ;
                need_record = 1 ;
            }
            else
            {
                switch ( type )
                {
                    case -1 : 
                    {
                        fprintf( stderr , "System error, type : Wrong pseudo code( %d %s ).\n", line , op ) ;
                        exit( EXIT_FAILURE ) ;
                    }
                    case 2 : 
                    {
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%5s %20s %20s %20s\n", "" , label , op , value ) ;
                        else
                            fprintf( readable_machine_code , "%5s %1s %20s %20s %20s\n", "" , "" , label , op , value ) ;
                        if ( strlen( record ) != 0 )
                            need_record = 1 ;
                        end = 1 ;
                        break ;
                    }
                    case 3 :
                    {
                        mac_code = Stohex( value ) ;
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%05X %20s %20s %20s\t\t%0*X\n", now_position , label , op , value , length*2 , mac_code ) ;
                        else 
                            fprintf( readable_machine_code , "%05X %d %20s %20s %20s\t\t%0*X\n", now_position , belong , label , op , value , length*2 , mac_code ) ;
                        need_record = 1 ;
                        break ;
                    }
                    case 4 :
                    {
                        mac_code = strtol( value , NULL , 10 ) ;
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%05X %20s %20s %20s\t\t%06X\n", now_position , label , op , value , mac_code ) ;
                        else
                            fprintf( readable_machine_code , "%05X %d %20s %20s %20s\t\t%06X\n", now_position , belong , label , op , value , mac_code ) ;
                        need_record = 1 ;
                        break ;
                    }
                    case 5 : case 6 : case 12 :
                    {
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%05X %20s %20s %20s\n", now_position , label , op , value ) ;
                        else
                            fprintf( readable_machine_code , "%05X %d %20s %20s %20s\n", now_position , belong , label , op , value ) ;
                        need_record = 2 ;
                        break ;
                    }
                    case 7 :
                    {
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%5s %20s %20s %20s\n", "" , label , op , value ) ;
                        else
                            fprintf( readable_machine_code , "%5s %1s %20s %20s %20s\n", "" , "" , label , op , value ) ;
                        break ;
                    }
                    case 10 :
                    {
                        SYMBOL temp = FindSymbol( value , symtab ) ;
                        SYMBOL new_B = { .symname = "B" , .length = 2 , .value = temp.value , .number = 3 } ;
                        Modifysymbol( &symtab , "B", new_B ) ;
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%5s %20s %20s %20s\n", "" , label , op , value ) ;
                        else
                            fprintf( readable_machine_code , "%5s %d %20s %20s %20s\n", "" , belong , label , op , value ) ;
                        break ;
                    }
                    case 99 :
                    {
                        LITTERAL tp_lit = Find_lit( op , *littab ) ;
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%05X %20s %20s %20s\t\t%0*X\n", now_position , label , op , value , tp_lit.length*2 , tp_lit.value ) ;
                        else
                            fprintf( readable_machine_code , "%05X %d %20s %20s %20s\t\t%0*X\n", now_position , belong , label , op , value , tp_lit.length*2 , tp_lit.value ) ;
                        mac_code = tp_lit.value ;
                        need_record = 1 ;
                        break ;
                    }
                    default :
                    {
                        if ( program_count == 1 )
                            fprintf( readable_machine_code , "%05X %20s %20s %20s\n", now_position , label , op , value ) ;
                        else
                            fprintf( readable_machine_code , "%05X %d %20s %20s %20s\n", now_position , belong , label , op , value ) ;
                        break ;
                    }
                }
            }

            if ( need_record ) // Text record
            {
                now_position += belong != 0 ? program[belong].start : 0 ;
                int textlen = strlen( record ) ;
                if ( textlen == 0 )
                    starting_addr = now_position ;
                if ( need_record == 2 && textlen != 0 ) // uncontinuous memory
                {
                    fprintf( machine_code , "T%06X%02X%s\n", starting_addr , strlen( record )/2 , record ) ;
                    memset( record , 0 , sizeof( record ) ) ;
                }
                if ( need_record == 1 )
                {
                    if ( textlen + length*2 > TRECLEN || end )
                    {
                        fprintf( machine_code , "T%06X%02X%s\n", starting_addr , strlen( record )/2 , record ) ;
                        memset( record , 0 , sizeof( record ) ) ;
                        snprintf( record , TRECLEN , "%0*X", length*2 , mac_code ) ;
                        starting_addr = now_position ;
                    }
                    else if ( need_record == 1 )
                        snprintf( record + textlen , TRECLEN , "%0*X", length*2 , mac_code ) ;
                }
            }
            if ( end )
            {
                Put_modrec( machine_code , program[0] ) ;
                if ( !program[0].specify )
                    fprintf( machine_code , "E" ) ;
                else
                    fprintf( machine_code , "E%06X", program[0].start ) ;
            }
            memset( instruction , 0 , sizeof( instruction ) ) ;
        }
    }
    else // is SIC
    {
        //
        //
        // #pragma region SICPRO
        //
        //
        char record[TRECLEN+1] ;
        memset( record , 0 , sizeof( record ) ) ;
        int starting_addr = program[0].start ;
        int end = 0 ;
        while ( fgets( instruction , 1024 , byproduct_file ) )
        {            
            ++line ;
            int type = 0 ;
            int need_record = 0 ;
            int now_position = 0 , length = 0 ;
            int code_belong = -1 ;

            char label[64] = "" , op[64] = "" , value[64] = "" ;
            int count = sscanf( instruction , "%x %s %s %s %d %d %d", &now_position , label , op , value , &length , &type , &code_belong ) ;
            
            SYMBOL new_PC = { .symname = "PC" , .length = 2 , .value = now_position+length , .number = 0x8 , .type = 0 , .belong = -1 } ;
            Modifysymbol( &symtab , "PC" , new_PC ) ;
            int mac_code = 0x000000 ;
            
            if ( type == 0 )
            {
                // #pragma region OPCMD
                OPCODE tp_op = FindOPcode( op ) ;
                mac_code |= tp_op.num & 0xFF ;
                mac_code <<= 1 ;
                if ( strcmp( value , "(null)" ) != 0 )
                {
                    char buffer[strlen( value )+1] ;
                    strcpy( buffer , value ) ;
                    char *cut = strstr( value , ",X" ) ;
                    if ( cut )
                    {
                        *cut = 0 ;
                        mac_code |= 0b1 ;
                    }
                    mac_code <<= 15 ;
                    SYMBOL tp_sy = FindSymbol( value , symtab ) ;
                    if ( tp_sy.length < 0 )
                    {
                        LITTERAL tp_lit = Find_lit( value , *littab ) ;
                        if ( tp_lit.visited >= 0 )
                            tp_sy.value = tp_lit.value ;
                        else
                        {
                            fprintf( stderr , "Your error, type : can't find symbol( %d : %s ).\n", line , value ) ;
                            exit( EXIT_FAILURE ) ;
                        }
                    }
                    mac_code |= tp_sy.value & 0x7FFF ;
                    strcpy( value , buffer ) ;    
                }
                else
                    mac_code <<= 15 ;

                if ( strcmp( label , "(null)" ) == 0 )
                    label[0] = '\0' ;
                if ( strcmp( value , "(null)" ) == 0 )
                    value[0] = '\0' ;
                fprintf( readable_machine_code , "%04X %20s %20s %20s\t\t%0*X\n", now_position , label , op , value , length*2 , mac_code ) ;
                need_record = 1 ;
            }
            else
            {
                // #pragma region PSECMD
                if ( strcmp( label , "(null)" ) == 0 )
                    label[0] = '\0' ;
                if ( strcmp( value , "(null)" ) == 0 )
                    value[0] = '\0' ;
                switch ( type )
                {
                    case -1 : 
                    {
                        fprintf( stderr , "System error, type : Wrong pseudo code( %d %s ).\n", line , op ) ;
                        exit( EXIT_FAILURE ) ;
                    }
                    case 2 : 
                    {
                        fprintf( readable_machine_code , "%4s %20s %20s %20s\n", "" , label , op , value ) ;
                        if ( strlen( record ) != 0 )
                            need_record = 1 ;
                        end = 1 ;
                        break ;
                    }
                    case 3 :
                    {
                        char buffer[strlen( value )+1] ;
                        strcpy( buffer , value ) ;
                        if ( strstr( value , "C\'" ) || strstr( value , "X\'" ) )
                            mac_code = Stohex( value ) ;
                        strcpy( value , buffer ) ;
                        fprintf( readable_machine_code , "%04X %20s %20s %20s\t\t%0*X\n", now_position , label , op , value , length*2 , mac_code ) ;
                        need_record = 1 ;
                        break ;
                    }
                    case 4 :
                    {
                        mac_code = strtol( value , NULL , 10 ) ;
                        fprintf( readable_machine_code , "%04X %20s %20s %20s\t\t%06X\n", now_position , label , op , value , mac_code ) ;
                        need_record = 1 ;
                        break ;
                    }
                    case 5 : case 6 :
                    {
                        need_record = 2 ;
                        fprintf( readable_machine_code , "%04X %20s %20s %20s\n", now_position , label , op , value ) ;
                        break ;
                    }
                    case 10 :
                    {
                        SYMBOL temp = FindSymbol( value , symtab ) ;
                        SYMBOL new_B = { .symname = "B" , .length = 2 , .value = temp.value , .number = 3 } ;
                        Modifysymbol( &symtab , "B", new_B ) ;
                        fprintf( readable_machine_code , "%4s %20s %20s %20s\n", "" , label , op , value ) ;
                        break ;
                    }
                    default :
                    {
                        fprintf( readable_machine_code , "%04X %20s %20s %20s\n", now_position , label , op , value ) ;
                        break ;
                    }
                }
            }

            if ( need_record )
            {
                int textlen = strlen( record ) ;
                if ( textlen == 0 )
                    starting_addr = now_position ;
                if ( need_record == 2 && strcmp( record , "" ) != 0 )
                {
                    fprintf( machine_code , "T%06X%02X%s\n", starting_addr , now_position - starting_addr , record ) ;
                    memset( record , 0 , sizeof( record ) ) ;
                }
                else if ( need_record == 1 )
                {
                    if ( textlen + length*2 > TRECLEN || end )
                    {
                        fprintf( machine_code , "T%06X%02X%s\n", starting_addr , now_position - starting_addr , record ) ;
                        memset( record , 0 , sizeof( record ) ) ;
                        snprintf( record , TRECLEN , "%0*X", length*2 , mac_code ) ;
                        starting_addr = now_position ;
                    }
                    else if ( need_record == 1 )
                        snprintf( record + textlen , TRECLEN , "%0*X", length*2 , mac_code ) ;
                }
            }
            if ( end )
            {
                if ( !program[0].specify )
                    fprintf( machine_code , "E" ) ;
                else
                    fprintf( machine_code , "E%06X", program[0].start ) ;
                break ;
            }

            memset( instruction , 0 , sizeof( instruction ) ) ;
        }
        
    }

    #pragma endregion

/******************************************************************************************************************************/
//                                                                                                                            //
//                                                      Finish                                                                //
//                                                                                                                            //
/******************************************************************************************************************************/
    #pragma region END

    fclose( byproduct_file ) ;
    fclose( readable_machine_code ) ;
    fclose( machine_code ) ;

    fprintf( stdout , "Success to transfer assenbly language to machine code.\n" ) ;

    return 0 ;

    #pragma endregion
}