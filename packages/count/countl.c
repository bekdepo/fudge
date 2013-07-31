#include <arch.h>
#include <fudge.h>

void main()
{

    unsigned char buffer[FUDGE_BSIZE];
    unsigned int count;
    unsigned int offset;
    unsigned char num[32];
    unsigned int start;
    unsigned int lines = 0;

    for (offset = 0; (count = call_read(CALL_DI, offset, FUDGE_BSIZE, buffer)); offset += start)
    {

        unsigned int i;

        start = 0;

        for (i = 0; i < count; i++)
        {

            if (buffer[i] != '\n')
                continue;

            lines++;
            start = i + 1;

        }

        if (!start)
            return;

    }

    call_write(CALL_DO, 0, memory_write_number(num, 32, lines, 10, 0), num);

}
