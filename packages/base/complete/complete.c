#include <fudge.h>

void main()
{

    unsigned char ibuffer[FUDGE_BSIZE];
    unsigned char cbuffer[FUDGE_BSIZE];
    unsigned int icount = call_read(FUDGE_IN, 0, FUDGE_BSIZE, ibuffer);
    unsigned int ocount = 0;
    unsigned int ccount;
    unsigned int coffset;
    unsigned int start;

    for (coffset = 0; (ccount = call_read(FUDGE_CWD, coffset, FUDGE_BSIZE, cbuffer)); coffset += start)
    {

        unsigned int i;

        start = 0;

        for (i = 0; i < ccount; i++)
        {

            if (cbuffer[i] != '\n')
                continue;

            if (memory_match(cbuffer + start, ibuffer, icount))
                ocount += call_write(FUDGE_OUT, ocount, i - start + 1, cbuffer + start);

            start = i + 1;

        }

        if (!start)
            return;

    }

}

