#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lex.h"
#include "parse.h"
#include "codegen/codegen.h"

#define FALSE 0
#define TRUE  1
#define BOOLEAN uint8_t

int main(int argc, char **argv) {
    FILE *fp;
    uint32_t i, tokens_length, file_length;
    const char *input_path, *output_path = "./a.out";
    char *input_content;
    struct smpl_token *tokens;
    struct smpl_node nodes;
    struct smpl_byte_buffer compiled;
    BOOLEAN debug = FALSE;
    for(i=1; i<argc; i++) {
        if(argv[i][0] != '-') { input_path = argv[i]; continue; }
        switch(argv[i][1]) {
        case 'd': debug = TRUE; break;
        case 'o': output_path = argv[++i]; break;
        }
    }
    if(!input_path) {
        printf("No input file\n");
        exit(1);
    }
 
    fp = fopen(input_path, "r");
    fseek(fp, 0L, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    input_content = calloc(1,file_length+1);
    fread(input_content, 1, file_length, fp);
    fclose(fp);

    if(debug) printf("%s\n", input_content);
    smpl_lex_string(&tokens, &tokens_length, input_content);
    if(debug) { 
        printf("|%-15s | %-4s | %-15s|\n", "line", "type", "value");
        for(i=0; i<tokens_length; i++) {
            printf("|%15d | 0x%02x , %15s|\n", tokens[i].line, tokens[i].type, tokens[i].value ? tokens[i].value : "n/a");
        }
    }
    smpl_parse_tokens(&nodes, tokens, tokens_length);
    smpl_node_represent(&nodes, 0);

    smpl_compile_ast(&compiled, &nodes);
    if(debug) {
        printf("|0000| ");
        for(i=0; i < compiled.length; i++) {
            printf("%02x ", compiled.bytes[i]);
            if(i %  4 ==  3) printf(" ");
            if(i % 16 == 15) printf("\n|%04x| ", i+1);
        }
        printf("\n");
    }

    fp = fopen(output_path, "w");
    fwrite(compiled.bytes, 1, compiled.length, fp);
    fclose(fp);

    /*Free our tokens & AST*/

    smpl_node_destroy(&nodes);
    for(i=0; i<tokens_length; i++) smpl_token_destroy(tokens+i);
    free(tokens);
    free(input_content);
    exit(0);
}
