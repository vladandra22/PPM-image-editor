void compression (Photo *myImg, int x, int y, int size, QTree **t, int threshold)
{
    Pixel mean;
    mean.red = 0;
    mean.green = 0;
    mean.blue = 0;
    unsigned long long i, j, m = 0;
    uint64_t mr = 0, mg = 0, mb = 0;
    uint64_t area  = size * size;

    /* Because "size" modifies each recursive function call, 
    We need to use the actual size of the image in order to use 
    array dimensions. Accesing our array's pixels
    is similar to a matrix. 
    "k" represents the number of columns in our initial image. */

    int k = myImg->width; 
    for(i = x; i < x + size ; i++)
        for(j = y; j < y + size; j++)
    {
        mr += myImg->data[i * k + j].red;
        mg += myImg->data[i * k + j].green;
        mb += myImg->data[i * k + j].blue;
    }
    mr = mr / area;
    mg = mg / area;
    mb = mb / area;
    mean.red = mr;
    mean.green = mg;
    mean.blue = mb;

    for(i = x; i < x + size; i++)
        for(j = y; j < y + size; j++)
            {
                m += (mean.red - myImg->data[i * k + j].red) * 
                    (mean.red - myImg->data[i * k + j].red);

                m += (mean.green - myImg->data[i * k + j].green) * 
                    (mean.green - myImg->data[i * k + j].green);

                m += (mean.blue - myImg->data[i * k + j].blue) * 
                    (mean.blue - myImg->data[i * k + j].blue);
            }

    m = m / (3 * area);

    initQTree(t);
    // Add the new pixel to QTree
    insertPixeltoQTree(t, mean, size, size); 

    /* If the condition is not respected, the algorithm stops
    to the current descendents and the function stops its recursion. */
    if(m > threshold)
        {
            compression(myImg, x, y, size/2, &(*t)->topLeft, threshold);
            compression(myImg, x, y + size/2, size/2, &(*t)->topRight, threshold);
            compression(myImg, x + size/2, y + size/2, size/2, &(*t)->bottomRight, threshold);
            compression(myImg, x + size/2, y, size/2, &(*t)->bottomLeft, threshold);
        }

}

void addNodeToArray(QuadtreeNode **v, QTree *t, unsigned int *n)
{
    if(t == NULL)
        return;

    /* In main function, I allocated memory for only one QuadtreeNode. 
    Memory is reallocated each time we add a new node in this function. */

    if((*n) > 0)
        *v = realloc(*v, sizeof(QuadtreeNode) * ((*n) + 1) );
    (*v)[*n].red = t->color.red;
    (*v)[*n].green = t->color.green;
    (*v)[*n].blue = t->color.blue;
    (*v)[*n].area = t->width * t->height;
    /* Add in tree current index value in order to build compression tree
    and respect the given array structure */
    t->index = (*n); 
    (*n)++;
    addNodeToArray(v, t->topLeft, n);
    addNodeToArray(v, t->topRight, n);
    addNodeToArray(v, t->bottomRight, n);
    addNodeToArray(v, t->bottomLeft, n);
}

// Function is called with k = 0 initially
void arrayIndex (QTree *t, QuadtreeNode *v, int *k)
{
    // Verify if node is leaf
    if(t->topLeft == NULL) 
        {
            v[*k].top_left = -1;
            v[*k].top_right = -1;
            v[*k].bottom_left = -1;
            v[*k].bottom_right = -1;
            (*k)++;
            return;
        }
    else
    {
        v[*k].top_left = t->topLeft->index;
        v[*k].top_right = t->topRight->index;
        v[*k].bottom_left = t->bottomLeft->index;
        v[*k].bottom_right = t->bottomRight->index;
    }
    (*k)++;
    arrayIndex(t->topLeft, v, k);
    arrayIndex(t->topRight, v, k);
    arrayIndex(t->bottomRight, v, k);
    arrayIndex(t->bottomLeft, v, k);
}

// Function that builds QuadTree from compressed array
void compressedToQtree(QuadtreeNode *v, unsigned int i, QTree **t) 
{
    if(i == -1)
    {
        fprintf(stderr, "Something went wrong");
        return;
    }
    initQTree(&(*t));
    Pixel p;
    p.red = v[i].red;
    p.green = v[i].green;
    p.blue = v[i].blue;
    unsigned int size = sqrt(v[i].area);
    insertPixeltoQTree (&(*t), p, size, size);
    (*t)->index = i;
    if(v[i].top_left != -1) 
    {
        compressedToQtree(v, v[i].top_left, &(*t)->topLeft); 
        compressedToQtree(v, v[i].top_right, &(*t)->topRight);
        compressedToQtree(v, v[i].bottom_right, &(*t)->bottomRight); 
        compressedToQtree(v, v[i].bottom_left, &(*t)->bottomLeft);
    }
    else
            (*t)->topLeft = (*t)->topRight = (*t)->bottomLeft = (*t)->bottomRight = NULL;
}

// Adds tree to PPM image
void decompress(Photo **myImg, int x, int y, int size, QTree *t)
{
    int i, j;
    int k = (*myImg)->width;
    if(t->topLeft == NULL)
    // Opposite starting direction compared to the compression
        for(i = y; i < y + size; i++) 
            for(j = x; j < x + size; j++)
                {
                    (*myImg)->data[i * k + j].red = t->color.red;
                    (*myImg)->data[i * k + j].green = t->color.green;
                    (*myImg)->data[i * k + j].blue = t->color.blue;
                }
    else
    {
        decompress(&(*myImg), x, y, size/2, t->topLeft);
        decompress(&(*myImg), x + size/2, y, size/2, t->topRight);
        decompress(&(*myImg), x + size/2, y + size/2, size/2, t->bottomRight);
        decompress(&(*myImg), x, y + size/2, size/2, t->bottomLeft);
    }
    free(t);
}

void vertical (QTree *t)
{
    if(t->topLeft != NULL)
    {
        vertical(t->topLeft);
        vertical(t->topRight);
        vertical(t->bottomRight);
        vertical(t->bottomLeft);
        QTree *aux;
        aux = t->topLeft;
        t->topLeft = t->bottomLeft;
        t->bottomLeft = aux;
        aux = t->topRight;
        t->topRight = t->bottomRight;
        t->bottomRight = aux;
    }
}

void horizontal (QTree *t)
{
    if(t->topLeft != NULL)
    {
        horizontal(t->topLeft);
        horizontal(t->topRight);
        horizontal(t->bottomRight);
        horizontal(t->bottomLeft);
        QTree *aux;
        aux = t->topLeft;
        t->topLeft = t->topRight;
        t->topRight = aux;
        aux = t->bottomLeft;
        t->bottomLeft = t->bottomRight;
        t->bottomRight = aux;
    }
}
int min (int a, int b)
{
    return (a < b)? a : b;
}

void overlay(QTree* node1, QTree* node2, QTree **t)
{
	(*t) = malloc(sizeof(QTree));
	
	if(node1->topLeft != NULL && node2->topLeft != NULL)
	{	
		overlay(node1->topLeft, 	node2->topLeft,		&(*t)->topLeft);
		overlay(node1->topRight,	node2->topRight,	&(*t)->topRight);
		overlay(node1->bottomRight, node2->bottomRight, &(*t)->bottomRight);
		overlay(node1->bottomLeft,	node2->bottomLeft,	&(*t)->bottomLeft);

	}
	else
		if(node1->topLeft == NULL && node2->topLeft != NULL)
		{
		overlay(node1, node2->topLeft,     &(*t)->topLeft);
		overlay(node1, node2->topRight,    &(*t)->topRight);
		overlay(node1, node2->bottomRight, &(*t)->bottomRight);
		overlay(node1, node2->bottomLeft,  &(*t)->bottomLeft);
		}
		else
		
			if(node1->topLeft != NULL && node2->topLeft == NULL)
			{
				overlay(node1->topLeft,     node2, &(*t)->topLeft);
				overlay(node1->topRight,    node2, &(*t)->topRight);
				overlay(node1->bottomRight, node2, &(*t)->bottomRight);
				overlay(node1->bottomLeft,  node2, &(*t)->bottomLeft);
			}
			else
				{
					(*t)->topRight	 = NULL;
					(*t)->topLeft	 = NULL;
					(*t)->bottomLeft  = NULL;
					(*t)->bottomRight = NULL;
				}
	;
    int minSize = min(node1->width, node2->width);
    (*t)->width = minSize;
    (*t)->height = minSize;
	(*t)->color.blue  = (node1->color.blue  + node2->color.blue)  / 2;
	(*t)->color.red   = (node1->color.red   + node2->color.red)   / 2;
	(*t)->color.green = (node1->color.green + node2->color.green) / 2;
}

void commandCompress(FILE *f, FILE *g, int threshold)
{
    QuadtreeNode *v;
    QTree *t;
    unsigned int nrNodes = 0;
    Photo *img = readPPM(f);
    compression(img, 0, 0,  img->width, &t, threshold); 

    // Initially allocating only one array element, then reallocating
    v = (QuadtreeNode*)malloc(sizeof(QuadtreeNode)); 
    addNodeToArray(&v, t, &nrNodes);
    int length = 0;
    arrayIndex(t, v, &length);
    int i, nrColors = 0;
    for(i = 0; i < nrNodes; i++)
        if(v[i].top_left == -1)
            nrColors++;
    fwrite(&nrColors, sizeof(int), 1, g);
    fwrite(&nrColors, sizeof(int), 1, g);
    fwrite(v, sizeof(QuadtreeNode), nrNodes, g);
    free(v);
    freeQTree(&t);
    free(img->data);
    free(img);
}
void commandDecompress (Photo *myImg_d, FILE *f, FILE *g)
{
    QuadtreeNode *v;
    QTree *t;
    unsigned int nrColors, nrNodes;


    fread(&nrColors, sizeof(int), 1, f); 
    fread(&nrNodes, sizeof(int), 1, f);
    v = (QuadtreeNode*)malloc(sizeof(QuadtreeNode) * nrNodes);

    // Read array from binary file
    fread(v, sizeof(QuadtreeNode), nrNodes, f);

    compressedToQtree(v, 0, &t);
    int size = t->width;
    allocPPM(&myImg_d, size);
    decompress(&myImg_d, 0, 0, size, t);
    writePPM(myImg_d, g);
    free(v);
    free(myImg_d->data);
    free(myImg_d);
}

void commandMirror(FILE *f, char *task, int threshold, FILE *g)
{
    Photo *img, *img_d;
    QTree *t;
    img = readPPM(f);
    compression(img, 0, 0, img->width, &t, threshold);
    if(strcmp(task, "v") == 0)
        vertical(t);
    else if(strcmp(task, "h") == 0)
            horizontal(t);
    int size = t->width;
    allocPPM(&img_d, size);
    decompress(&img_d, 0, 0, size, t);
    writePPM(img_d, g);
    free(img_d->data);
    free(img_d);
    free(img->data);
    free(img);
}

void commandOverlay(FILE *f1, FILE *f2, int threshold, FILE *g)
{
    Photo *img1 = readPPM(f1);
    Photo *img2 = readPPM(f2);
	QTree * node1 = NULL;
	QTree* node2 = NULL;
    compression(img1, 0, 0, img1->width, &node1, threshold);
    compression(img2, 0, 0, img2->width, &node2, threshold);
		
    QTree *nodeOverlay = NULL;
	overlay(node1, node2, &nodeOverlay);
    Photo *img_d;
    int size = nodeOverlay->width;
 	allocPPM(&img_d, size);
    decompress(&img_d, 0, 0, size, nodeOverlay);
    writePPM(img_d, g);
	free(img_d->data);
    free(img_d);
    free(img1->data);
    free(img1);
    free(img2->data);
    free(img2);
}