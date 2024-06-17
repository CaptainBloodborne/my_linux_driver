#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEVICE_NAME "/dev/chardev_stack"
#define MY_MAGIC 'G'
#define MY_IOCTL_RESIZE_STACK _IOW(MY_MAGIC, 1, int)

int main() {
    int fd, ret;
    int32_t value;
    size_t new_size;

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return errno;
    }

    // Resize the stack
    new_size = 20;
    ret = ioctl(fd, MY_IOCTL_RESIZE_STACK, &new_size);
    if (ret < 0) {
        perror("Failed to resize the stack");
        return errno;
    }

    // Push values onto the stack
    for (int i = 0; i < 10; i++) {
        value = i + 1;
        ret = write(fd, &value, sizeof(value));
        if (ret < 0) {
            perror("Failed to push value onto the stack");
            return errno;
        }
        // sleep(1);
    }

    // Pop values from the stack
    for (int i = 0; i < 10; i++) {
        ret = read(fd, &value, sizeof(value));
        if (ret < 0) {
            perror("Failed to pop value from the stack");
            return errno;
        } else if (ret == 0) {
            printf("Stack is empty\n");
            break;
        }
        // sleep(1);
        printf("Popped value: %d\n", value);
    }

    close(fd);
    return 0;
}
