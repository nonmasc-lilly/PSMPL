#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../parse.h"
#include "codegen.h"

#define SMPL_8086_START_ADDRESS 0x7C00

struct smpl_8086_state {
    uint32_t current_address;
    uint32_t subroutines_length;
    struct {
        char *name;
        uint32_t address;
    } *subroutines;
};

void smpl_byte_buffer_create(struct smpl_byte_buffer *ret, uint32_t length) {
    ret->bytes  = calloc(1,length ? length : 1);
    ret->length = length;
}
void smpl_byte_buffer_resize(struct smpl_byte_buffer *ret, int32_t offset) {
    ret->length += offset;
    ret->bytes = realloc(ret->bytes, ret->length ? ret->length : 1);
    if(offset > 0) memset(ret->bytes+ret->length-offset, 0, offset-1);
}
void smpl_byte_buffer_destroy(struct smpl_byte_buffer *ret) {
    free(ret->bytes);
    memset(ret, 0, sizeof(struct smpl_byte_buffer));
}

static void smpl_8086_compile_primary(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast);
static void smpl_8086_compile_halt(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast);
static void smpl_8086_compile_statement(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast);
static void smpl_8086_compile_subroutine(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast);
static void smpl_8086_compile_halt(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast) {
    if(ast->type != SMPL_NODE_TYPE_HALT) return;
    smpl_byte_buffer_resize(ret, 2);
    ret->bytes[ret->length - 2] = 0xEB;
    ret->bytes[ret->length - 1] = 0xFE;
    state->current_address+=2;
}

#define SMPL_8086_STATEMENT_LENGTH 1
static void smpl_8086_compile_statement(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast) {
    uint32_t i;
    void ((*functions[SMPL_8086_STATEMENT_LENGTH])(struct smpl_byte_buffer*,struct smpl_8086_state*,const struct smpl_node *ast));
    functions[0] = smpl_8086_compile_halt;
    for(i=0; i<SMPL_8086_STATEMENT_LENGTH; i++)
        functions[i](ret, state, ast);
}

static void smpl_8086_compile_subroutine(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast) {
    uint32_t i;
    if(ast->type != SMPL_NODE_TYPE_SUBROUTINE) return;
    state->subroutines = realloc(state->subroutines, ++state->subroutines_length*sizeof(*state->subroutines));
    state->subroutines[state->subroutines_length-1].address = state->current_address; 
    state->subroutines[state->subroutines_length-1].name    = malloc(strlen(ast->value)+1);
    strcpy(state->subroutines[state->subroutines_length-1].name, ast->value);
    for(i=0; i<ast->children_length; i++)
        smpl_8086_compile_statement(ret, state, ast->children+i);
    smpl_byte_buffer_resize(ret, 1);
    ret->bytes[ret->length-1] = 0xC3;
    state->current_address++;
}

#define SMPL_8086_PRIMARY_LENGTH 1
static void smpl_8086_compile_primary(struct smpl_byte_buffer *ret, struct smpl_8086_state *state, const struct smpl_node *ast) {
    uint32_t i;
    void ((*functions[SMPL_8086_PRIMARY_LENGTH])(struct smpl_byte_buffer*,struct smpl_8086_state*,const struct smpl_node *ast));
    functions[0] = smpl_8086_compile_subroutine;
    for(i=0; i<SMPL_8086_PRIMARY_LENGTH; i++)
        functions[i](ret, state, ast);
}

static void smpl_8086_compile_header(
        struct smpl_byte_buffer *ret,
        uint8_t *success,
        struct smpl_8086_state *state,
        const struct smpl_node *ast) {
    uint32_t i;
    uint32_t start_address;
    if(ast->type != SMPL_NODE_TYPE_HEADER) {
        *success = 0;
        return;
    }
    *success = 1;
    start_address = state->current_address;
    for(i=0; i<ast->children_length; i++)
        smpl_8086_compile_primary(ret, state, ast->children+i);
    if(state->current_address - start_address > 0x1FE) {
        printf("CODE GENDERATION LEVEL ERROR: PLATFORM=8086:\n\tPROGRAM ERROR: header is larger than 512 bytes\n");
        exit(1);
    }
    smpl_byte_buffer_resize(ret, 510-(state->current_address-start_address)+2);
    ret->bytes[ret->length-1] = 0xAA;
    ret->bytes[ret->length-2] = 0x55;
}

void smpl_compile_ast(struct smpl_byte_buffer *ret, const struct smpl_node *ast) {
    struct smpl_8086_state state;
    uint32_t i;
    uint8_t header_exists;
    smpl_byte_buffer_create(ret, 0);
    state.current_address = SMPL_8086_START_ADDRESS;
    state.subroutines = malloc(1);
    state.subroutines_length = 0;
    smpl_8086_compile_header(ret, &header_exists, &state, ast->children);
    for(i=header_exists ? 1 : 0; i<ast->children_length; i++)
        smpl_8086_compile_primary(ret, &state, ast->children+i);
}





