#include <linux/types.h>

typedef struct Stack {
    size_t current_stack_len;
    size_t top;
    int32_t* data;

} Stack;

void resizeStack(Stack* stack, size_t n);
void initStack(Stack* stack, size_t n);
void destroyStack(Stack* stack);
int32_t pop(Stack* stack);
void push(Stack* stack, int32_t data);
