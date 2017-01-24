#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bmp.h"

void print_rgb(RGBTRIPLE * t);

int main(int argc, char ** argv)
{
    if(argc != 4)
    {
        fprintf(stderr, "Resize required exactly (3) command line arguments!\n");
        return 1;
    }
    // convert provided reduction factor to float, on failure returns 0
    // will deal with this condition later
    float factor = atof(argv[1]);
    
    //open infile and outfile
    char * infile_name = argv[2];
    FILE * infile = fopen(infile_name, "rb");
    
    char * outfile_name = argv[3];
    FILE * outfile = fopen(outfile_name, "wb");
    
    // check file pointers
    if(infile == NULL)
    {
        fprintf(stderr, "Could not open infile!\n");
        return 1;
    }
    if(outfile == NULL)
    {
        fprintf(stderr, "Could not open outfile!\n");
        return 1;
    }

    // read file info into memory
    BITMAPFILEHEADER * file_header = malloc(sizeof(BITMAPFILEHEADER));
    fread(file_header, sizeof(BITMAPFILEHEADER), 1, infile);
    
    BITMAPINFOHEADER * info_header = malloc(sizeof(BITMAPINFOHEADER));
    fread(info_header, sizeof(BITMAPINFOHEADER), 1, infile);
    
    // grab pertinant file info
    LONG in_width = info_header->biWidth; 
    LONG in_height = info_header->biHeight;
    LONG in_pixels = abs(in_width * in_height);

    // calculate outfile width and height
    LONG out_width = (LONG) (factor * in_width);
    LONG out_height = (LONG) (factor * in_height);
    LONG out_pixels = abs(out_width * out_height);
    
    // short circuit too small factor
    if(out_width == 0 || out_height == 0)
    {
        fprintf(stderr, "Outfile size too small, try a larger resize factor!\n");
        return 1;
    }
    
    // scanlines
    int in_padding = (4 - (in_width * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (out_width * sizeof(RGBTRIPLE)) % 4) % 4;;
    
    // pull infile info into memory
    RGBTRIPLE * in_data = malloc(in_pixels * sizeof(RGBTRIPLE));
    
    for(int i = 0; i < in_pixels; i++)
      {     
	if(fread(in_data + i, sizeof(RGBTRIPLE), 1, infile) != 1)
	  {
	    printf("error reading in file\n");
	    return 1;
	  }
        if(i != 0 && (i + 1) % in_width == 0)
	  {
	    if(in_padding != 0)
	      {
		fseek(infile, in_padding, SEEK_CUR);
	      }
	  }
      }  
    
    // resize data
    RGBTRIPLE * out_data = malloc(out_pixels * sizeof(RGBTRIPLE));
    for(int i = 0; i < out_pixels; i++)
      {
        // infer out_image coordinates
        LONG out_y = i / out_width;
        LONG out_x = i % out_width;
        
        // infer out_image coords
        LONG in_y = (LONG) (out_y / factor);
        LONG in_x = (LONG) (out_x / factor);
        
        // transfer data
	*(out_data + i) = *(in_data + in_y * in_width + in_x);
      }
    
    // change header data
    info_header->biWidth = out_width;
    info_header->biHeight = out_height;    
    info_header->biSizeImage = out_width * abs(out_height) * sizeof(RGBTRIPLE);
    file_header->bfSize = info_header->biSizeImage + sizeof(BITMAPFILEHEADER)
        + sizeof(BITMAPINFOHEADER);
    
    // write header to file
    fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, outfile);
    fwrite(info_header, sizeof(BITMAPINFOHEADER), 1, outfile);
    
    // write data to file
    for(int i = 0; i < out_pixels; i++)
      {
	fwrite(out_data + i, sizeof(RGBTRIPLE), 1, outfile);
	// check if you've reached the end of a row of pixels
	if(i != 0 && (i + 1) % out_width == 0)
	  {
	    // write out_padding zero bytes to the file
	    for(int j = 0; j < out_padding; j++)
	      {     
		fputc(0x00, outfile);
	      }
	  }
      }
    
    free(file_header);
    free(info_header);
    free(in_data);
    free(out_data);
    fclose(infile);
    fclose(outfile);
    
}

void print_rgb(RGBTRIPLE * t)
{
  printf("%d %d %d\n", t->rgbtBlue, t->rgbtGreen, t->rgbtRed);
}
