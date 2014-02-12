#define TASK_DESCRIPTORS                32

struct task
{

    struct list_item item;
    struct {unsigned long ip; unsigned long sp;} registers;
    struct vfs_descriptor descriptors[TASK_DESCRIPTORS];

};

void task_init(struct task *task, unsigned long ip, unsigned long sp);
