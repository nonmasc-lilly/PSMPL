#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"
/*
(*ret)[*ret_length-1].type = smpl_token_type_from_buf(buf, line);
(*ret)[*ret_length-1].line = line;
switch((*ret)[*ret_length-1].type) {
case SMPL_TOKEN_TYPE_INTEGER:
case SMPL_TOKEN_TYPE_IDENTIFIER:
    (*ret)[*ret_length-1].value = malloc(strlen(buf)+1);
    strcpy((*ret)[*ret_length-1].value, buf);
    break;
default:
    (*ret)[*ret_length-1].value = NULL;
    break;
}
*/

void smpl_token_create(struct smpl_token *ret, SMPL_TOKEN_TYPE type, uint32_t line, const char *value) {
    memset(ret, 0, sizeof(struct smpl_token));
    ret->type = type;
    ret->line = line;
    if(!value) return;
    ret->value = malloc(strlen(value)+1);
    strcpy(ret->value, value);
}

void smpl_token_destroy(struct smpl_token *ret) {
    if(ret->value) free(ret->value);
    memset(ret, 0, sizeof(struct smpl_token));
}

void smpl_token_create_from_buf(struct smpl_token *ret, const char *buf, uint32_t line) {
    memset(ret, 0, sizeof(struct smpl_token));
    ret->type = smpl_token_type_from_buf(buf, line);
    ret->line = line;
    switch(ret->type) {
    case SMPL_TOKEN_TYPE_INTEGER:
    case SMPL_TOKEN_TYPE_IDENTIFIER:
        ret->value = malloc(strlen(buf)+1);
        strcpy(ret->value, buf);
        break;
    }
}

SMPL_TOKEN_TYPE smpl_token_type_from_buf(const char *buf, uint32_t line) {
    char *eptr;
    const char *s;
    if(!*buf)                       return SMPL_TOKEN_TYPE_NULL;
    else if(!strcmp(buf, "header")) return SMPL_TOKEN_TYPE_HEADER;
    else if(!strcmp(buf, "subrt"))  return SMPL_TOKEN_TYPE_SUBRT;
    else if(!strcmp(buf, "end"))    return SMPL_TOKEN_TYPE_END;
    else if(!strcmp(buf, "halt"))   return SMPL_TOKEN_TYPE_HALT;
    strtol(buf, &eptr, 0);
    if(!*eptr) return SMPL_TOKEN_TYPE_INTEGER;
    for(s=buf;isalnum(*s) || *s == '_';++s);
    if(!*s) return SMPL_TOKEN_TYPE_IDENTIFIER;
    printf("Unexpected token %s on line: %d\n", buf, line);
    exit(1);
}

void smpl_lex_string(struct smpl_token **ret, uint32_t *ret_length, const char *string) {
    uint32_t i, line;
    char *buf;
    buf  = calloc(1,1);
    *ret = malloc(1);
    *ret_length = 0;
    for(i=0; string[i]; i++) {
        if(string[i] == '\n') line++;
        if(string[i] != '%' && !isspace(string[i])) {
            buf = realloc(buf, strlen(buf)+2);
            buf[strlen(buf)+1] = 0;
            buf[strlen(buf)+0] = string[i];
            continue;
        }
        if(string[i] == '%') {
            for(++i; string[i] != '%'; i++)
                if(string[i] == '\n') line++;
        }
        if(smpl_token_type_from_buf(buf, line) == SMPL_TOKEN_TYPE_NULL) continue;
        *ret = realloc(*ret, ++*ret_length * sizeof(struct smpl_token));
        smpl_token_create_from_buf(*ret+*ret_length-1, buf, line);
        *(buf = realloc(buf, 1)) = 0;
    }
}

