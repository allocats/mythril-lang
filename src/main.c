#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "arena/arena.h"
#include "ast/ast.h"
#include "ast/types.h"
#include "lexer/lexer.h"
#include "semantics/semantics.h"
#include "token/token.h"
#include "utils/ansi_codes.h"
#include "utils/macros.h"
#include "utils/types.h"

static ArenaAllocator arena = {0};

char* map_file(const char* path, usize* len) {
    i32 fd = open(path, O_RDONLY);
    if (fd == -1) {
        MEOW_ERROR("Failed to open file");
        return nullptr;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        MEOW_ERROR("Failed to stat file");
        close(fd);
        return nullptr;
    }

    char* buffer = mmap(NULL, st.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        MEOW_ERROR("Failed to allocate file");
        close(fd);
        return nullptr;
    }

    buffer[st.st_size] = 0;

    (*len) = st.st_size;

    return buffer;
}

i32 main(i32 argc, char* argv[]) {
    if (argc != 2) return 1;

    init_arena(&arena, 65536);

    usize len;
    b32 error = false;

    char* buffer = map_file(argv[1], &len);
    if (buffer == nullptr) {
        MEOW_PANIC("Failed to map file");
    }

    Tokens* tokens = lex_file(&arena, buffer, buffer + len);
    if (tokens == nullptr) {
        MEOW_PANIC("Failed to lex file");
    }

    tokens_print(tokens);

    // if (tokens -> had_error) {
    //     error = true;
    // }

    Program* program = ast_build(&arena, tokens, buffer);

    if (program -> had_error) {
        error = true;
    }

    print_program(program);
    
    SemanticCtx* ctx = analyze_program(program);

    if (error) {
        fprintf(
            stderr, 
            "compilation" 
            ANSI_BOLD ANSI_RED " failed" ANSI_RESET 
            "!\n"
        );

        return 1;
    }

    printf("\ncompiled" ANSI_BOLD ANSI_GREEN " successfully" ANSI_RESET "!\n");

    munmap(buffer, len);
}
