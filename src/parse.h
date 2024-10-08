#ifndef X__SMPL_PARSE_H__X
#define X__SMPL_PARSE_H__X
#include <stdint.h>
#include "lex.h"

typedef uint8_t SMPL_NODE_TYPE; enum {
    SMPL_NODE_TYPE_NULL         =   0x00,
    SMPL_NODE_TYPE_ROOT         =   0x01,
    SMPL_NODE_TYPE_IDENTIFIER   =   0x02,
    SMPL_NODE_TYPE_INTEGER      =   0x03,
    SMPL_NODE_TYPE_SUBROUTINE   =   0x04,
    SMPL_NODE_TYPE_HALT         =   0x05,
    SMPL_NODE_TYPE_HEADER       =   0x06,
};

struct smpl_node {
    SMPL_NODE_TYPE type;
    uint32_t children_length;
    char *value;
    struct smpl_node *children;
};

struct smpl_parse_state {
    uint32_t subroutines_length;
    char **subroutines;
};

void smpl_assert(uint8_t condition, const char *msg, ...);
void smpl_node_create(struct smpl_node *ret, SMPL_NODE_TYPE type, const char *value);
void smpl_node_destroy(struct smpl_node *ret);
void smpl_node_represent(struct smpl_node *ret, uint32_t level);
void smpl_node_add_child(struct smpl_node *ret, SMPL_NODE_TYPE type, const char *value);

typedef void (*SMPL_PARSER_FUNCTION_POINTER)(
        struct smpl_node*,
        uint32_t *,
        struct smpl_parse_state *,
        struct smpl_token *,
        uint32_t);

void smpl_parse_halt(
    struct smpl_node *parent,
    uint32_t *index,
    struct smpl_parse_state *state,
    struct smpl_token *tokens,
    uint32_t tokens_length
);
void smpl_parse_statement(
    struct smpl_node *parent,
    uint32_t *index,
    struct smpl_parse_state *state,
    struct smpl_token *tokens,
    uint32_t tokens_length
);
void smpl_parse_header(
    struct smpl_node *parent,
    uint32_t *index,
    struct smpl_parse_state *state,
    struct smpl_token *tokens,
    uint32_t tokens_length
);
void smpl_parse_subroutine(
    struct smpl_node *parent,
    uint32_t *index,
    struct smpl_parse_state *state,
    struct smpl_token *tokens,
    uint32_t tokens_length
);
void smpl_parse_primary(
    struct smpl_node *parent,
    uint32_t *index,
    struct smpl_parse_state *state,
    struct smpl_token *tokens,
    uint32_t tokens_length
);
void smpl_parse_tokens(
    struct smpl_node *ret,
    struct smpl_token *tokens,
    uint32_t tokens_length
);

#endif
