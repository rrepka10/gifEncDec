/*---------------------------------------------------------------------------
  This program reads and writes GIF files to/from true color space.  It includes
  code to automatically generate the GIF color lookup trable
  
  gcc -g example.c  gifenc.c gifdec.c   -o example -fsanitize=address -fsanitize=undefined

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include "gifEncDec.h"


#define SYNTAX_ERROR    (30)
#define FILE_NOT_FOUND  (40)
#define MALLOC_ERROR    (50)
#define SUCCESS         (0)
#define GIF_ERROR       (60)



/*---------------------------------------------------------------------------
  Simple test program to demonstrate GIF functions
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
   
   // Used to access images as 2 D arrays
   pixel **RGBarray;
   pixel *RGBframe;
   uint8_t **IndxArray;
   uint8_t *IndxFrame;
   
   gd_GIF *inGif;
   ge_GIF *outGif;

   int i, j, k, w,h;

   // Table to hold the palette.
   pixel palette [MAX_PALETTE];
   int palSize = MAX_PALETTE;
    
   // Check command line arguments
    if ((argc != 3) && (argc != 4)){
      fprintf(stderr, "This programs tests GIF reading and writing\n");
      fprintf(stderr, "A duplicate PPM output file will also be produced\n");
      fprintf(stderr, "%s r|w|c gifFile [gifFile2] \n", argv[0]);
      fprintf(stderr, "Where: r|w       - read or write flags, required\n");
      fprintf(stderr, "       gifFile   - name of the file to read or write\n");
      fprintf(stderr, "       gifFile2  - optional 2nd file name for 'c' copy mode\n");
      fprintf(stderr, "e.g: %s r test.gif   - produces a PPM file from test.gif file\n", argv[0]);
      fprintf(stderr, "     %s w test.gif   - produces a sample gif file\n", argv[0]);
      fprintf(stderr, "     %s c in.gif out.gif  - copies in.gif to out.gif with full conversion\n", argv[0]);
      return(SYNTAX_ERROR);
    }
    
   // Clear the palette, not really necessary
   for (i = 0; i < MAX_PALETTE; i++) { 
      palette[i].r = 0; palette[i].g = 0; palette[i].b = 0;
      }
    
   /*------------------------------------------------------------------------
   Do stuff based on the flag.  Read the input GIF and write a new one out
   ------------------------------------------------------------------------*/
   if (strcmp(argv [1], "c") == 0) {
      // Open the GIF file
      inGif = gd_open_gif(argv[2]);
      if (!inGif) {
         fprintf(stderr, "Could not open %s\n", argv[2]);
         return(SYNTAX_ERROR);
      }

      printf("Reading %s %dx%d %d colors %d depth %d loop count\n", argv [2],
             inGif->width, inGif->height, inGif->palette->size, inGif->depth, inGif->loop_count);
    
      // Get memory for a 3 color frame
      RGBframe = malloc(inGif->width * inGif->height * 3);
      if (!RGBframe ) {
         fprintf(stderr, "Could not allocate frame\n");
         return(MALLOC_ERROR);
      }

      // Get the first frame from the GIF
      i = gd_get_frame(inGif);
      if (i == -1) { 
         fprintf(stderr, "GIF error\n");
         exit(GIF_ERROR);
         }
      
      // Render it to RBG
      gd_render_frame(inGif, (uint8_t *)RGBframe);
      
      if (i == 0) { 
         gd_rewind(inGif);
         }

      // output the same as input
      w = inGif->width;
      h = inGif->height;
 
      // Write out the wedge as a ppm
      writePPM("outc.ppm", w, h, RGBframe);      
      
      //Get space for the index color array
      IndxFrame = malloc(h*w*sizeof(uint8_t));   
    
      // Build a complete index color file from the true color file
      palSize = createGIF(RGBframe, IndxFrame, w, h, palette, palSize);
      printf("Writing %s with %d colors\n", argv[3], palSize);
    
      //                   file name    canvas size    palette  palette depth  infinite loop 
      outGif = ge_new_gif2(argv [3], w, h, (uint8_t *)palette, palSize, 0);
      if (outGif == NULL) {
         fprintf(stderr, "Could not create %s gif file\n", argv [2]);
         exit(GIF_ERROR);
      }
      outGif->frame = IndxFrame;

      // add frame, 10 second delay
      ge_add_frame(outGif, 0);
       
      /* remember to close the GIF */
      ge_close_gif(outGif);
      gd_close_gif(inGif);
       
      // Clean up memory
      free(RGBframe);
      free(IndxFrame);
   } // copy 
    
    
   /*------------------------------------------------------------------------
     Build and write a GIF file and copy a PNG
   ------------------------------------------------------------------------*/
   else if (strcmp(argv [1], "w") == 0) {
      // create an fixed GIF 
      w = 128;
      h = 64;
    
      // Get memory for a 3 color frame
      RGBarray = malloc(h*sizeof(pixel *));
      RGBframe = malloc(w * h * sizeof(pixel));
      if (!RGBframe || !RGBarray) {
          fprintf(stderr, "Could not allocate frame\n");
          return(MALLOC_ERROR);
      }    
         
      // Setup array linkage
       for (i = 0; i < h; i++) {
          RGBarray[i] = &RGBframe[i*w];
       }
    
      //Get space for the index color array
      IndxArray = malloc(h*sizeof(uint8_t *));
      IndxFrame = malloc(h*w*sizeof(uint8_t));   
      if (!IndxFrame || !IndxArray) {
          fprintf(stderr, "Could not allocate frame\n");
          return(MALLOC_ERROR);
      } 
      
      // Setup array linkage
      for (i = 0; i < h; i++) {
          IndxArray[i] = &IndxFrame[i*w];
       }
    
      // Build a simple color wedge
      for (i = 0; i < h; i++) {
         for (j = 0; j < w; j++) {
            RGBarray [i][j].r = i+j;
            RGBarray [i][j].g = i-j;
            RGBarray [i][j].b = j-i;
         }  
      } // end i
      
      // Write out the wedge as a ppm
      writePPM("outw.ppm", w, h, RGBframe);     
 
      // Build a complete index color file from the true color file
      palSize = createGIF(RGBframe, IndxFrame, w, h, palette, palSize);
      printf("Palette size %d\n", palSize);
     
      //                   file name    canvas size    palette  palette depth  infinite loop 
      outGif = ge_new_gif2(argv [2], w, h, (uint8_t *)palette, palSize, 0);
      if (outGif == NULL) {
         fprintf(stderr, "Could not create %s gif file\n", argv [2]);
         exit(GIF_ERROR);
      }
         
      outGif->frame = IndxFrame;

      // add frame, 10 second delay
      ge_add_frame(outGif, 0);
       
      /* remember to close the GIF */
      ge_close_gif(outGif);
       
      // Clean up memory
      free(RGBarray);
      free(RGBframe);
      free(IndxArray);
      free(IndxFrame);
   } // write
   
   
   /*------------------------------------------------------------------------
     Read a GIF file and produce a PNG
   ------------------------------------------------------------------------*/
   else  {
      // Open the GIF file
      inGif = gd_open_gif(argv[2]);
      if (!inGif) {
         fprintf(stderr, "Could not open %s\n", argv[2]);
         return(SYNTAX_ERROR);
      }

      printf("Reading %s %dx%d %d colors %d depth %d loop count\n", argv [2],
             inGif->width, inGif->height, inGif->palette->size, inGif->depth, inGif->loop_count);
    
      // Get memory for a 3 color frame
      RGBframe = malloc(inGif->width * inGif->height * 3);
      if (!RGBframe ) {
         fprintf(stderr, "Could not allocate frame\n");
         return(MALLOC_ERROR);
      }

      // Get the first frame from the GIF
      i = gd_get_frame(inGif);
      if (i == -1) { 
         fprintf(stderr, "GIF error\n");
         exit(GIF_ERROR);
         }
      
      // Render it to RBG
      gd_render_frame(inGif, (uint8_t *)RGBframe);
     
      // Write the ppm version
      printf("Writing out.ppm version of %s file\n", argv [2]);
      writePPM("outr.ppm", inGif->width, inGif->height, RGBframe);  
     
      if (i == 0) { 
         gd_rewind(inGif);
         }
      gd_close_gif(inGif);
    
      // Clean up memory
      free(RGBframe);
   } // End if read

  
   return(SUCCESS);
} // end main()
   


/*---------------------------------------------------------------------------
   Writes a PPM format image file
      
      char *filename - The PPM file image name to write 
      PPMImage *img  - A pointer to an (PPM) image object
      
      Returns: nothing
      
      Error handling: exit with a return code
----------------------------------------------------------------------------*/
void writePPM(const char *filename, int x, int y, pixel *img) {
   FILE *fp;
   //open file for output
   fp = fopen(filename, "wb");
   if (!fp) {
       fprintf(stderr, "Unable to open file '%s'\n", filename);
       exit(1);
   }

   //write the header file as ascii data on each line
   //image format
   fprintf(fp, "P6\n");

   //comments
   fprintf(fp, "# Created by FELIXKLEMM\n");

   //image size x y
   fprintf(fp, "%d %d\n", x, y);

   // rgb component depth
   fprintf(fp, "%d\n",255);

   // pixel data - binary 
   fwrite(img, 3 * x, y, fp);
   fclose(fp);
}




