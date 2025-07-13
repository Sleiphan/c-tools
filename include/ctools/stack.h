#ifndef CTOOLS_STACK
#define CTOOLS_STACK

typedef struct stack_node stack_node;

typedef struct stack {
    stack_node* top;
} stack;

void stack_create(stack* s);
void stack_push(stack* stack, void* value);
void stack_push_sort(stack* stack, void* value, int (*comp)(const void* lhs, const void* rhs));
// void stack_push_sort();
void* stack_pop(stack* stack);
void stack_destroy(stack* stack);

#endif // CTOOLS_STACK