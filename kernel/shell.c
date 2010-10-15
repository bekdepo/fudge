#include <lib/call.h>
#include <lib/elf.h>
#include <lib/stack.h>
#include <lib/file.h>
#include <lib/string.h>
#include <lib/vfs.h>
#include <kernel/elf.h>
#include <kernel/kernel.h>
#include <kernel/shell.h>

char shellBuffer[SHELL_BUFFER_SIZE];
struct stack shellStack;

static void shell_clear()
{

    file_write("fudge:/$ ");
    stack_clear(&shellStack);

}

static void shell_execute_flat(unsigned int *address, int argc, char *argv[])
{

    void (*func)(int argc, char *argv[]) = (void (*)(int argc, char *argv[]))address;

    func(argc, argv);

}

static void shell_execute_elf(struct vfs_node *node, struct elf_header *header, int argc, char *argv[])
{

    struct elf_program_header *pHeader = (struct elf_program_header *)(0x280000 + header->programHeaderOffset);

    vfs_read(node, pHeader->offset, pHeader->memorySize, (void *)pHeader->virtualAddress);

    shell_execute_flat((unsigned int *)pHeader->virtualAddress, argc, argv);

}

static void shell_call(struct vfs_node *node, int argc, char *argv[])
{

    char *buffer = (char *)0x280000;

    vfs_read(node, 0, node->length, buffer);

    if (elf_check((struct elf_header *)buffer))
    {

        shell_execute_elf(node, (struct elf_header *)0x280000, argc, argv);

    }

    else
    {

        file_write("Unrecognized binary format. Continue? (y/n): ");

        char c = file_read_single();

        file_write("\n");

        if (c == 'y')
        {

            shell_execute_flat((unsigned int *)0x280000, argc, argv);

        }

    }

}

static void shell_interpret(char *command)
{

    char *argv[32];
    int argc = string_split(argv, command, ' ');

    if (argc)
    {

        struct vfs_node *initrd = call_open("initrd");
        struct vfs_node *node = vfs_find(initrd, argv[0]);

        if (node)
            shell_call(node, argc, argv);

        else
        {

            file_write(argv[0]);
            file_write(": Command not found\n");

        }

    }

    shell_clear();

}

static void shell_handle_input(char c)
{

    switch (c)
    {

        case '\t':

            break;

        case '\b':

            if (stack_pop(&shellStack))
            {

                file_write_single('\b');
                file_write_single(' ');
                file_write_single('\b');

             }

            break;

        case '\n':

            stack_push(&shellStack, '\0');
            file_write_single(c);
            shell_interpret(shellBuffer);

            break;

        default:

            stack_push(&shellStack, c);
            file_write_single(c);

            break;

    }

}

static void shell_poll()
{

    for (;;)
    {

        char c = file_read_single();

        shell_handle_input(c);

    }

}

void shell_init()
{

    stack_init(&shellStack, shellBuffer, SHELL_BUFFER_SIZE);

    file_write("Fudge\n");
    file_write("-----\n");
    file_write("Copyright (c) 2009 Jens Nyberg\n");
    file_write("Type 'cat help.txt' to read the help section.\n\n");

    shell_clear();
    shell_poll();

}

