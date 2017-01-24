#include <stdio.h>

int main(int argc int argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Please provide the name of the raw file to recover!\n");
        return 1;
    }
    
    unsigned char * block = malloc(512 * sizeof(unsigned char));
    
    FILE * infile = fopen(argv[1], "r");
    if(infile == NULL)
    {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return 1;
    }
    
    while(fread(block, 512 * sizeof(unsigned char), 1, infile) == 1)
    {
            // check for jpeg.
    }
    if(feof(infile))
    {
        printf("Reached end of file\n");
    }
    else
    {
        printf("Other error occurred\n");
    }
}