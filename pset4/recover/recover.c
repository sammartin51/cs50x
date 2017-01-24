#include <stdio.h>
#include <stdlib.h>

int is_jpeg(unsigned char * block);

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
	fprintf(stderr, "Please provide the name of the raw file to recover!\n");
	return 1;
    }
    
    unsigned char * block_buffer = malloc(512 * sizeof(unsigned char));
    
    FILE * infile = fopen(argv[1], "r");
    if(infile == NULL)
    {
	fprintf(stderr, "Could not open %s\n", argv[1]);
	return 1;
    }

    int image_num = 0;
    char fname_buffer[10];
    FILE * outfile = NULL;
    // read through all the blocks in the file
    while(fread(block_buffer, 512 * sizeof(unsigned char), 1, infile) == 1)
    {
	// check to see that current file is a jpeg opening block
	if(is_jpeg(block_buffer) == 1)
	{
	    // close the previous outfile if there's an open one
	    if(outfile != NULL)
	    {
		fclose(outfile);
	    }

	    // prepare the filename buffer and open the file
	    sprintf(fname_buffer, "03%d.jpg", ++image_num);
	    outfile = fopen(fname_buffer, "w");
	    if(outfile == NULL)
	    {
		fprintf(stderr, "Could not open %s\n", fname_buffer);
		return 1;
		}
	    }
	
	// if there's a file open for editing, write the block to it
	// garbage at the front of the file won't be written this way
	if(outfile != NULL)
	{
	    fwrite(block_buffer, 512 * sizeof(unsigned char), 1, outfile);
	}
    }
    // check for how the file ended.
    if(feof(infile))
    {
	printf("Reached end of file\n");
    }
    else
    {
	printf("Other error occurred\n");
    }

    free(block_buffer);
    fclose(infile);
    fclose(outfile);
}

// function to check whether a block is a jpeg opening block
int is_jpeg(unsigned char * buffer)
{
    // check the first three bytes against the check buffer
    unsigned char check[3] = {0xff, 0xd8, 0xff};
    for(int i = 0; i < 3; i++)
    {;
	if(*(buffer + i) != check[i])
	{
	    // failed
	    return 0;
	}
    }
  
    if(*(buffer + 3) > 0xdf)
    {
	return 1;
    }   

    // failed
    return 0;
}
  
  
