#include <inttypes.h>
/* Structure used for the final array.
The attribute assures data alignment. */

typedef struct QuadtreeNode {
    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;

// My QuadTree structure
typedef struct QTree
{
    Pixel color;
    int index;
    unsigned int width, height;
    struct QTree *topLeft, *topRight, *bottomLeft, *bottomRight;
} QTree;

void initQTree (QTree **t)
{
    *t = (QTree*)malloc(sizeof(QTree));
    (*t)->topLeft = NULL;
    (*t)->topRight = NULL;
    (*t)->bottomLeft = NULL;
    (*t)->bottomRight = NULL;
}

// Insert given pixel in QuadTree
void insertPixeltoQTree (QTree **t, Pixel p, unsigned int width, unsigned int height)
{
    (*t)->color.red = p.red;
    (*t)->color.green = p.green;
    (*t)->color.blue = p.blue;
    (*t)->width = width;
    (*t)->height = height;
}

void freeQTree (QTree **t)
{
    if((*t) != NULL)
    {
        freeQTree(&(*t)->topLeft);
        freeQTree(&(*t)->topRight);
        freeQTree(&(*t)->bottomRight);
        freeQTree(&(*t)->bottomLeft);
    }
    free(*t);
}
