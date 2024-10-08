#ifndef X__SMPL_LEX_H__X
#define X__SMPL_LEX_H__X
#include <stdint.h>

typedef uint8_t SMPL_TOKEN_TYPE; enum {
    SMPL_TOKEN_TYPE_NULL        =   0x00,
    SMPL_TOKEN_TYPE_IDENTIFIER  =   0x01,
    SMPL_TOKEN_TYPE_INTEGER     =   0x02,
    SMPL_TOKEN_TYPE_SUBRT       =   0x03,
    SMPL_TOKEN_TYPE_END         =   0x04,
    SMPL_TOKEN_TYPE_HALT        =   0x05,
    SMPL_TOKEN_TYPE_HEADER      =   0x06
};

struct smpl_token {
    SMPL_TOKEN_TYPE type;
    uint32_t line;
    char *value;
};

void smpl_token_create(struct smpl_token *ret, SMPL_TOKEN_TYPE type, uint32_t line, const char *value);
void smpl_token_create_from_buf(struct smpl_token *ret, const char *buf, uint32_t line);
void smpl_token_destroy(struct smpl_token *ret);

SMPL_TOKEN_TYPE smpl_token_type_from_buf(const char *buf, uint32_t line);
void smpl_lex_string(struct smpl_token **ret, uint32_t *ret_length, const char *string);

#endif
