// RGB Pixel Structure
typedef struct Pixel 
{
     unsigned char red, green, blue; 
} Pixel;

// PPM image structure stored in a dynamic array
typedef struct Photo 
{
     unsigned int width, height;
     Pixel *data;
} Photo;


void allocPPM(Photo **img, int size)
{
    (*img) = (Photo*)malloc(sizeof(Photo));
    (*img)->data = (Pixel*)malloc(size * size * sizeof(Pixel));
    (*img)->width = size;
    (*img)->height = size;
}

// Read PPM image (binary format)
Photo *readPPM(FILE *f)
{
        char l[256], *type;
        Photo *img;
        unsigned int width, height, d;
        if (f == NULL) 
            fprintf(stderr, "Can't open file");
        // Reads "P6", stops at newline
        type = fgets(l, 256, f);
        type = fgets(l, 256, f); 
        sscanf(l, "%u %u", &width, &height);
        type = fgets(l, 256, f);
        sscanf(l, "%u", &d); // d = 255
        img = (Photo*)malloc(sizeof(Photo));
        allocPPM(&img, width); // width = height, square photo
        int i;
        for(i = 0; i < width*height; i++) 
        {
            fread(&img->data[i].red, 1, 1, f);
            fread(&img->data[i].green, 1, 1, f);
            fread(&img->data[i].blue, 1, 1, f);
        }
        return img;
}

// Writes PPM image in binary file format
void writePPM(Photo *img, FILE *g)
{
    char *type = "P6\n";
    fprintf(g, "%s", type);
    // Image is square so it is enough to use only one of the 2 dimensions
    int size = img->width; 
    fprintf(g, "%d %d\n255\n", size, size);
    int i;
    for(i = 0; i < size*size; i++) 
        {
            fwrite(&img->data[i].red, 1, 1, g);
            fwrite(&img->data[i].green, 1, 1, g);
            fwrite(&img->data[i].blue, 1, 1, g);
        }
} 
