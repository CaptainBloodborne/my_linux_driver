#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/slab.h>
// #include <linux/mutex.h>

#include "stack.h"

#define DEVICE_NAME "chardev_stack"
#define IOCTL_RESIZE_STACK _IOW('G', 1, int)


static int major;
static int device_is_open = 0;
static Stack stack;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *file, unsigned cmd, unsigned long arg);

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl
};

static int __init chardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    // mutex_init(&mtx);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", major);
        return major;
    }
    printk(KERN_INFO "chardev_driver assigned major number %d. To talk to\n", major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major);
    return 0;
}

static void __exit chardev_exit(void) {

    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, world!\n");
}

static long device_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
    size_t new_size;

    switch (cmd)
    {
        case IOCTL_RESIZE_STACK:
            if (copy_from_user(&new_size, (size_t __user *) arg, sizeof(new_size)))
            {
                return -EFAULT;
            }

            if (new_size <= 0)
            {
                return -EINVAL;
            }

            resizeStack(&stack, new_size);
            break;
        default:
           return -ENOTTY;
    }

    return 0;
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_is_open)
        return -EBUSY;
    device_is_open++;

    // if(mutex_lock_interruptible(&mtx))
    // {
    //     return -ERESTARTSYS;
    // }
    
    size_t initial_stack_size = 100;
    initStack(&stack, initial_stack_size);

    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_is_open--;

    destroyStack(&stack);
    
    module_put(THIS_MODULE);

    // mutex_unlock(&mtx);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset) {
    int32_t value;
    Stack *stack_p = &stack;

    if (stack_p->top == 0)
    {
        printk(KERN_ALERT "Stack is empty!\n");
        return 0;
    }

    value = pop(stack_p);

    if (copy_to_user(buffer, &value, sizeof(value)))
    {
        return -EFAULT;
    }

    printk(KERN_INFO "Pop %d from stack.\n", value);
    
    return sizeof(value);
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t len, loff_t * off) {
    int32_t value;
    Stack* stack_p = &stack;

    if (stack_p->top == stack_p->current_stack_len)
    {
        printk(KERN_ALERT "Stack is full!\n");
        return -ENOMEM;
    }

    if (copy_from_user(&value, buffer, sizeof(value)))
    {
        return -EFAULT;
    }

    push(&stack, value);
    printk(KERN_INFO "Pushed %d to stack.\n", value);
    
    return sizeof(value);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Artem Pichugin");
MODULE_DESCRIPTION("A simple character device driver with stack");

module_init(chardev_init);
module_exit(chardev_exit);
