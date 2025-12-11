#include "delimiters.h"

#include <stdio.h>

void delimiters_push(Parser* p, Token* token, const char* src_ctx, const usize length) {
    DelimiterStack* stack = &p -> delimiters;

    if (stack -> top >= DELIMITER_STACK_MAX) {
        // todo: error
        fprintf(stderr, "wth is going on");
        exit(1);
    }

    stack -> items[stack -> top++] = (Delimiter) {
        .token = token,
        .context = src_ctx,
        .length = length
    };
}

Delimiter* delimiters_pop(Parser* p) {
    DelimiterStack* stack = &p -> delimiters;

    if (stack -> top < 0) {
        return nullptr;
    }

    return &stack -> items[stack -> top--];
}
