#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "arena/arena.h"
#include "ast/ast.h"
#include "ast/types.h"
#include "diagnostics/diagnostics.h"
#include "files/types.h"
#include "lexer/lexer.h"
#include "mythril/types.h"
#include "tokens/tokens.h"
#include "utils/types.h"

static ArenaAllocator arena = {0};

i32 map_file(FileBuffer* file_buffer, char* path) {
    i32 fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", path);
        fprintf(stderr, "compilation failed\n");
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        fprintf(stderr, "Error: Failed to stat file '%s'\n", path);
        fprintf(stderr, "compilation failed\n");
        return -1;
    }

    usize len = st.st_size + 1;

    char* buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    if (buffer == MAP_FAILED || !buffer) {
        close(fd);
        fprintf(stderr, "Error: Failed to mmap() file '%s'\n", path);
        fprintf(stderr, "compilation failed\n");
        return -1;
    }

    buffer[len - 1] = 0;

    file_buffer -> ptr = buffer;
    file_buffer -> len = len;
    file_buffer -> needs_free = true;

    return 0;
}

i32 main(i32 argc, char* argv[]) {
    #ifdef MYTHRIL_DEBUG

    #include <assert.h>

    static_assert(sizeof(MythrilContext) == 48, "MythrilContext is not 48 bytes");
    static_assert(sizeof(Token) == 16, "Token is not 16 bytes");

    #endif /* ifdef MYTHRIL_DEBUG */

    if (argc == 1) {
        fprintf(stderr, "Usage: %s <files>\n", argv[0]);
        return 1;
    }

    init_arena(&arena, 65536);

    i32 exit_code = 0;

    char** file_paths = argv + 1;
    u32 file_count = argc - 1;

    FileBuffer buffers[file_count];
    memset(buffers, 0, sizeof(buffers));

    DiagContext diag_ctx = {
        .arena = &arena,
        .index = 0,
        .warning_count = 0,
        .error_count = 0,
        .stderr_supports_colours = false
    };

    if (isatty(STDERR_FILENO)) {
        diag_ctx.stderr_supports_colours = true;
    }

    Tokens tokens = {
        .items = arena_alloc(&arena, sizeof(Token) * 64),
        .capacity = 64,
        .count = 0
    };

    Program program = {
        .declarations = nullptr, // Allocate in parse() initial capacity of tokens.count
        .capacity = 0,
        .count = 0
    };

    MythrilContext mythril_ctx = {
        .arena = &arena,
        .diag_ctx = &diag_ctx,
        .tokens = &tokens,
        .program = &program,
    };

    for (u32 i = 0; i < file_count; i++) {
        i32 ok = map_file(&buffers[i], file_paths[i]);

        if (ok == -1) {
            exit_code = 1;
            goto cleanup;
        }

        diag_ctx.path = file_paths[i];
        diag_ctx.source_buffer = buffers[i].ptr;

        mythril_ctx.buffer_start = buffers[i].ptr;
        mythril_ctx.buffer_end = buffers[i].ptr + buffers[i].len;

        tokenize(&mythril_ctx);
    }

    Token end_of_program_token = {
        .kind = TOK_EOP,
        .lexeme = nullptr,
        .length = 0
    };

    mythril_ctx.tokens -> items[mythril_ctx.tokens -> count++] = end_of_program_token;

    parse(&mythril_ctx, file_paths, buffers, file_count);

    #ifdef MYTHRIL_DEBUG
        print_tokens(tokens);
        print_program(&program);
    #endif /* ifdef MYTHRIL_DEBUG */

    printf("\n");

    if (diag_ctx.error_count == 0) {
        // codegen()
        exit_code = 0;
        printf("compiled successfully\n");
    } else {
        diagnostics_print_all(&diag_ctx);
        exit_code = 1;
    }

cleanup:
    for (u32 i = 0; i < file_count; i++) {
        if (buffers[i].needs_free) {
            munmap(buffers[i].ptr, buffers[i].len);
        }
    }

    return exit_code;
}
