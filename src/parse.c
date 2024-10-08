#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lex.h"
#include "parse.h"

void smpl_assert(uint8_t condition, const char *msg, ...) {
    if(!condition) {
        va_list a;
        va_start(a, msg);
        vprintf(msg, a);
        exit(1);
    }
}
void smpl_node_create(struct smpl_node *ret, SMPL_NODE_TYPE type, const char *value) {
    memset(ret, 0, sizeof(struct smpl_node));
    ret->type = type;
    ret->children = malloc(1);
    if(!value) return;
    ret->value = malloc(strlen(value)+1);
    strcpy(ret->value, value);
}
void smpl_node_destroy(struct smpl_node *ret) {
    uint32_t i;
    if(ret->value) free(ret->value);
    for(i=0; i < ret->children_length; i++)
        smpl_node_destroy(ret->children+i);
    free(ret->children);
    memset(ret, 0, sizeof(struct smpl_node));
}
void smpl_node_represent(struct smpl_node *ret, uint32_t level) {
    uint32_t i;
    printf("|");
    for(i=0; i<level; i++) printf("#");
    printf("> %02x : %s\n", ret->type, ret->value ? ret->value : "n/a");
    for(i=0; i<ret->children_length; i++) {
        smpl_node_represent(ret->children+i, level+1);
    }
}
void smpl_node_add_child(struct smpl_node *ret, SMPL_NODE_TYPE type, const char *value) {
    ret->children = realloc(
        ret->children,
        ++ret->children_length * sizeof(struct smpl_node)
    );
    smpl_node_create(ret->children+ret->children_length-1, type, value);
}
void smpl_parse_halt(
        struct smpl_node *parent,
        uint32_t *index,
        struct smpl_parse_state *state,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    if(tokens[*index].type != SMPL_TOKEN_TYPE_HALT) return;
    smpl_node_add_child(parent, SMPL_NODE_TYPE_HALT, NULL);
    ++*index;
}

#define SMPL_PARSER_FUNCTION_STATEMENT_MAX 1
void smpl_parse_statement(
        struct smpl_node *parent,
        uint32_t *index,
        struct smpl_parse_state *state,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    uint32_t i;
    SMPL_PARSER_FUNCTION_POINTER functions[SMPL_PARSER_FUNCTION_STATEMENT_MAX] = {
        smpl_parse_halt
    };
    for(i=0; i<SMPL_PARSER_FUNCTION_STATEMENT_MAX; i++)
        functions[i](parent,index,state,tokens,tokens_length);
}
void smpl_parse_header(
        struct smpl_node *parent,
        uint32_t *index,
        struct smpl_parse_state *state,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    struct smpl_node *subject;
    if(tokens[*index].type != SMPL_TOKEN_TYPE_HEADER) return;
    ++*index;
    smpl_node_add_child(parent, SMPL_NODE_TYPE_HEADER, NULL);
    subject = parent->children+parent->children_length-1;
    while(tokens[*index].type != SMPL_TOKEN_TYPE_END) {
        smpl_parse_primary(subject, index, state, tokens, tokens_length);
    }
    ++*index;
}
void smpl_parse_subroutine(
        struct smpl_node *parent,
        uint32_t *index,
        struct smpl_parse_state *state,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    struct smpl_node *subject;
    if(tokens[*index].type != SMPL_TOKEN_TYPE_SUBRT) return;
    ++*index;
    smpl_assert(
        tokens[*index].type == SMPL_TOKEN_TYPE_IDENTIFIER,
        "Error: Expected identifier on line %d for subroutine statement\n",
        tokens[*index - 1].line
    );
    state->subroutines = realloc(
        state->subroutines,
        ++state->subroutines_length * sizeof(char *)
    );
    state->subroutines[state->subroutines_length-1] = malloc(strlen(tokens[*index].value)+1);
    strcpy(state->subroutines[state->subroutines_length-1], tokens[*index].value);
    smpl_node_add_child(parent, SMPL_NODE_TYPE_SUBROUTINE, tokens[*index].value);
    ++*index;
    subject = parent->children+parent->children_length-1;
    while(tokens[*index].type != SMPL_TOKEN_TYPE_END) {
        smpl_parse_statement(subject, index, state, tokens, tokens_length);
    }
    ++*index;
}
#define SMPL_PARSER_FUNCTION_PRIMARY_MAX 1
void smpl_parse_primary(
        struct smpl_node *parent,
        uint32_t *index,
        struct smpl_parse_state *state,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    uint32_t i;
    SMPL_PARSER_FUNCTION_POINTER functions[SMPL_PARSER_FUNCTION_PRIMARY_MAX] = {
        smpl_parse_subroutine
    };
    for(i=0; i<SMPL_PARSER_FUNCTION_PRIMARY_MAX; i++)
        (functions[i])(parent,index,state,tokens,tokens_length);
}

void smpl_parse_tokens(
        struct smpl_node *ret,
        struct smpl_token *tokens,
        uint32_t tokens_length) {
    struct smpl_parse_state state;
    uint32_t i;
    state.subroutines_length = 0;
    state.subroutines = malloc(1);
    smpl_node_create(ret, SMPL_NODE_TYPE_ROOT, NULL);
    i=0;
    smpl_parse_header(ret, &i, &state, tokens, tokens_length);
    for(; i<tokens_length;) {
        smpl_parse_primary(ret, &i, &state, tokens, tokens_length);
    }
}
