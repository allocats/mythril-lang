#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "arena/arena.h"
#include "diagnostics/diagnostics.h"
#include "files/types.h"
#include "lexer/lexer.h"
#include "mythril/types.h"
#include "tokens/types.h"
#include "utils/types.h"

static ArenaAllocator arena = {0};

void map_file(FileBuffer* file_buffer, char* path) {
    i32 fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", path);
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        fprintf(stderr, "Error: Failed to stat file '%s'\n", path);
        exit(1);
    }

    usize len = st.st_size + 1;

    char* buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED || !buffer) {
        close(fd);
        fprintf(stderr, "Error: Failed to mmap() file '%s'\n", path);
        exit(1);
    }

    buffer[len - 1] = 0;

    file_buffer -> ptr = buffer;
    file_buffer -> len = len;
}

i32 main(i32 argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <files>\n", argv[0]);
        return 1;
    }

    init_arena(&arena, 65536);

    char** file_paths = argv + 1;
    u32 file_count = argc - 1;

    FileBuffer buffers[file_count];

    DiagContext diag_ctx = {
        .arena = &arena,
        .index = 0,
        .warning_count = 0,
        .error_count = 0
    };

    Tokens tokens = {
        .tokens = arena_alloc(&arena, sizeof(Token) * 64),
        .count = 0,
        .capacity = 64
    };

    MythrilContext mythril_ctx = {
        .arena = &arena,
        .diag_ctx = &diag_ctx,
        .tokens = &tokens,
    };

    for (u32 i = 0; i < file_count; i++) {
        map_file(&buffers[i], file_paths[i]);

        diag_ctx.path = argv[i];
        diag_ctx.source_buffer = buffers[i].ptr;

        mythril_ctx.buffer_start = buffers[i].ptr;
        mythril_ctx.buffer_end = buffers[i].ptr + buffers[i].len;

        tokenize(&mythril_ctx);
    }

    i32 exit_code = 0; 

    if (diag_ctx.error_count == 0) {
        // codegen
        exit_code = 0;
    } else {
        // compilation failed
        exit_code = 1;
    }

    for (u32 i = 0; i < file_count; i++) {
        munmap(buffers[i].ptr, buffers[i].len);
    }

    return exit_code;
}
