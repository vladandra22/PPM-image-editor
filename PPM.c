#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "image_struct.h"
#include "quadtree.h"
#include "commands.h"


int main(int argc, char *argv[])
{
    int ok = 0;
    if(strcmp(argv[1], "-c") == 0)
    {
        FILE *f = fopen(argv[argc - 2], "rb");
        FILE *g = fopen(argv[argc - 1], "wb");
        int threshold = atoi(argv[2]);
        commandCompress(f, g, threshold);
        ok = 1;
        fclose(f);
        fclose(g);
    }
    
    if(strcmp(argv[1], "-d") == 0)
    {
        FILE *f = fopen(argv[argc - 2], "rb");
        FILE *g = fopen(argv[argc - 1], "wb");
        Photo *img_d;
        commandDecompress(img_d, f, g);
        ok = 1;
        fclose(f);
        fclose(g);
    }

    if(strcmp(argv[1], "-m") == 0)
    {
        FILE *f = fopen(argv[argc - 2], "rb");
        FILE *g = fopen(argv[argc - 1], "wb");
        char *task = argv[2];
        int threshold = atoi(argv[3]);
        commandMirror(f, task, threshold, g);
        ok = 1;
        fclose(f);
        fclose(g);
    }
    if(strcmp(argv[1], "-o") == 0)
    {
        unsigned int i;
        int threshold = atoi(argv[2]);
        FILE *f1 = fopen(argv[3], "rb");
        FILE *f2 = fopen(argv[4], "rb");
        FILE *g = fopen(argv[5], "wb");
        commandOverlay(f1, f2, threshold, g);
        ok = 1;
        fclose(f1);
        fclose(f2);
        fclose(g);
    }
    if(!ok)
        printf("Invalid command ! \n");
    else
        printf("All done !\n");
    return 0;

}
