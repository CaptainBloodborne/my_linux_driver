#include <linux/types.h>
#include <linux/slab.h>

#include "stack.h"

void initStack(Stack* stack, size_t n)
{
    stack->data = kzalloc(n, sizeof(int32_t));

    if (stack->data == NULL)
    {
        pr_err("Failed to allocate memory!");
    }

    stack->current_stack_len = n;
    stack->top = 0;
}

void destroyStack(Stack* stack)
{
    kfree(stack->data);
}

void resizeStack(Stack* stack, size_t n)
{
    if (n > stack->current_stack_len)
    {
        int32_t* data = kmalloc(n, sizeof(int32_t));

        if (data == NULL)
        {
            pr_err("Failed to allocate memory!");
        }

        for (size_t i = 0; i < stack->current_stack_len; i++)
        {
            data[i] = stack->data[i];
        }

        kfree(stack->data);

        stack->data = data;
        stack->current_stack_len = n;

    } else 
    {
        int32_t* data = kzalloc(n, sizeof(int32_t));

        if (data == NULL)
        {
            pr_err("Failed to allocate memory!");
        }

        for (size_t i = 0; i < n; i++)
        {
            data[i] = stack->data[i];
        }

        kfree(stack->data);

        stack->data = data;
        stack->current_stack_len = n;

        if (stack->top > stack->current_stack_len)
        {
            stack->top = stack->current_stack_len - 1;   
        }
    }
}

void push(Stack* stack, int32_t data)

{
    printk(KERN_INFO "Current top value is %d", stack->data[stack->top]);

    if (stack->top >= stack->current_stack_len) pr_err("Stack max limit exceeded!");

    stack->data[stack->top++] = data;

    printk(KERN_INFO "New top value is %d", stack->data[stack->top]);
}

int32_t pop(Stack* stack)
{
    printk(KERN_INFO "Current top value is %d", stack->data[stack->top]);

    if (stack->top == 0) pr_err("Stack is empty!");

    printk(KERN_INFO "New top value is %d", stack->data[stack->top]);
    
    return stack->data[--stack->top];
}
