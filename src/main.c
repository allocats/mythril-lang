#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "arena/arena.h"
#include "ast/ast.h"
#include "ast/types.h"
#include "lexer/lexer.h"
#include "token/token.h"
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

    char* buffer = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        MEOW_ERROR("Failed to allocate file");
        close(fd);
        return nullptr;
    }

    (*len) = st.st_size;

    return buffer;
}

i32 main(i32 argc, char* argv[]) {
    init_arena(&arena, 65536);

    usize len;

    char* buffer = map_file(argv[1], &len);
    if (buffer == nullptr) {
        MEOW_PANIC("Failed to map file");
    }

    Tokens* tokens = lex_file(&arena, buffer, buffer + len);
    if (tokens == nullptr) {
        MEOW_PANIC("Failed to lex file");
    }

    tokens_print(tokens);

    ProgramFunctions* funcs = ast_build(tokens, &arena);

    print_program(funcs);

    munmap(buffer, len);
}
