#include <abi.h>
#include <fudge.h>

static struct args
{

    char *position;
    unsigned int count;

} args[32];

static unsigned int parse(unsigned int count, void *buffer)
{

    char *b = buffer;
    char *begin = buffer;
    unsigned int nargs = 0;
    unsigned int i;

    for (i = 0; i < count; i++)
    {

        if (b[i] != '\n' && b[i] != ' ')
            continue;

        b[i] = '\0';
        args[nargs].position = begin;
        args[nargs].count = b + i - begin;
        begin = b + i + 1;
        nargs++;

    }

    return nargs;

}

void main()
{

    unsigned char buffer[FUDGE_BSIZE];
    unsigned int count;
    unsigned int nargs;
    unsigned int i;

    call_open(CALL_P0);
    count = call_read(CALL_P0, 0, 1, FUDGE_BSIZE, buffer);
    nargs = parse(count, buffer);
    call_close(CALL_P0);

    for (i = 0; i < nargs; i += 4)
    {

        unsigned int channel = ascii_rvalue(args[i].position, args[i].count, 10);
        unsigned int mount = ascii_rvalue(args[i + 1].position, args[i + 1].count, 10);
        unsigned int backend = ascii_rvalue(args[i + 2].position, args[i + 2].count, 10);

        if (!call_walk(CALL_L0, CALL_PR, args[i + 3].count - 1, args[i + 3].position + 1))
            continue;

        call_open(CALL_L0);
        call_auth(channel, backend);
        call_mount(mount, channel, CALL_L0);
        call_close(CALL_L0);

    }

}
