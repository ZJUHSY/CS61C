
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

#define MAX_LABEL_SIZE 100

const int SYMTBL_NON_UNIQUE = 0;
const int SYMTBL_UNIQUE_NAME = 1;

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
    write_to_log("Error: allocation failed\n");
    exit(1);
}

void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char* name) {
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_symbol(FILE* output, uint32_t addr, const char* name) {
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMTBL_NON_UNIQUE or SYMTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
    /* YOUR CODE HERE */
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol)*INITIAL_SIZE);
    if(!symbol){
        allocation_failed();
        return NULL;
    }
    // for(int i = 0; i < INITIAL_SIZE; i++){
    //     symbol->name = malloc(sizeof(MAX_LABEL_SIZE));
    //     if(!symbol->name){
    //         allocation_failed();
    //         return NULL;
    //     }
    //     symbol ++ ;
    // }
    SymbolTable* res = (SymbolTable*)malloc(sizeof(SymbolTable));
    if(!res){
        allocation_failed();
        return NULL;
    }
    res->mode = mode;
    res->len = 0;
    res->tbl = symbol;
    res->cap = INITIAL_SIZE;
    return res;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
    /* YOUR CODE HERE */
    Symbol* symbol = table->tbl;
    for(int i = 0;i<table->len;i++){
        free(symbol->name);
        symbol++;
    }
    free(table->tbl);
    free(table);
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   ADDR is given as the byte offset from the first instruction. The SymbolTable
   must be able to resize itself as more elements are added. 

   Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
   return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. If memory
   allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
    /* YOUR CODE HERE */
    if(_check_name_exists(table, name) && table->mode==1){
        name_already_exists(name);
        return -1;
    }
    if(addr%4!=0){
        addr_alignment_incorrect();
        return -1;
    }
    if(table->len>=table->cap){
        table->cap *= SCALING_FACTOR;
        table->tbl = (Symbol*)realloc(table->tbl, (sizeof(Symbol))*(table->cap));
        if(!(table->tbl)){
            allocation_failed();
            return -1;
        }
    }
    char* copy_name = malloc(sizeof(strlen(name) + 1));
    if(!copy_name){
        allocation_failed();
        return -1;
    }
    Symbol* symbol = table->tbl;
    for(int i = 0; i < table->len; i++){
        symbol++;
    }
    symbol->addr = addr;
    strcpy(copy_name, name);
    symbol -> name = copy_name;
    table->len += 1;
    return 0;
}

int _check_name_exists(SymbolTable* table, const char* name){
    Symbol* symbol = table->tbl;
    for(int i = 0;i < table->len; i++){
        // printf("%s\t%s\n", symbol -> name, name);
        if(strcmp(symbol->name,name) == 0){
            return 1;
        }
        symbol++;
    }
    return 0;
}
/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {
    /* YOUR CODE HERE */
    Symbol* current_tbl = table->tbl;
    for(int i = 0; i < table->len; i++){
        if(strcmp(current_tbl->name, name)==0){
            return current_tbl->addr;
        }
        current_tbl++;
    }
    return -1;   
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_symbol() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
    /* YOUR CODE HERE */
    int i;
    Symbol* tbl = table->tbl;
    for(i = 0; i < table->len; i++){
        write_symbol(output, tbl->addr, tbl->name);
        tbl++;
    }
}
