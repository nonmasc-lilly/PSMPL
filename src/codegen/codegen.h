#ifndef X__SMPL_CODEGEN_H__X
#define X__SMPL_CODEGEN_H__X
#include <stdint.h>
#include "../parse.h"


typedef uint8_t SMPL_BYTE;
struct smpl_byte_buffer {
    SMPL_BYTE *bytes;
    uint32_t length;
};

void smpl_byte_buffer_create(struct smpl_byte_buffer *ret, uint32_t length);
void smpl_byte_buffer_resize(struct smpl_byte_buffer *ret, int32_t offset);
void smpl_byte_buffer_destroy(struct smpl_byte_buffer *ret);

void smpl_compile_ast(struct smpl_byte_buffer *ret, const struct smpl_node *ast);
#endif
