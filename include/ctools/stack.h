#ifndef CTOOLS_STACK
#define CTOOLS_STACK

typedef struct stack stack;

stack* stack_create();
int stack_size(stack* stack);
int stack_push(stack* stack, void* value);
int stack_push_sort(stack* stack, void* value, int (*comp)(const void* lhs, const void* rhs));
void* stack_pop(stack* stack);
void stack_shutdown(stack* stack);
void stack_destroy(stack* stack);

#endif // CTOOLS_STACK