/* 
 * File:   TCLFast.hxx
 * Author: kukulski
 *
 * Created on November 9, 2012, 2:58 PM
 */

#ifndef TCLFAST_HXX
#define	TCLFAST_HXX

#include "tclled.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

class TCLFast {
    
public:
    
    TCLFast(int wd, int ht) {
        width = wd;
        height = ht;
        
        initTCL();
         pixels = tcl.pixels;
         set_gamma(3.0, 3.0, 3.0);
        
        rawBuffer = new unsigned int[wd*ht];
        
    }
    
    ~TCLFast() {
        delete rawBuffer;
    }
    
    unsigned int *getRawBuffer() {
        return rawBuffer;
    }
    
    size_t getBufferSize() { return width * height * 4;}
    
    void prepPixels() {
        for(int i = 0; i < height; i++)
            for(int j = 0; j < width; j++)
                prepPixel(i,j);
    }
    
    void testPattern() {
  	tcl_color *pixels = tcl.pixels;

    for(int i = 0; i < width*height; i++) {
               
        int row = i/width;
        int col = i%width;
        int quad = i /25;
        
        tcl_color *px = getTCLPixel(row, col);

        ::write_gamma_color(px, row*255/4,col*255/4,quad*255/3);
    }
}
    
    void send() {
        ::send_buffer(spi,&tcl);
    }
    
    
private:
    tcl_buffer tcl;
    tcl_color *pixels;
    unsigned int *rawBuffer;
    int width, height;
    int spi;

 void prepPixel(int row, int col) {
	unsigned int px = getPixel(row,col);
	::write_bgra_gamma(getTCLPixel(row,col),px);
}
tcl_color *getTCLPixel(int row, int col) {
	int phase = row & 1;
	int unwoundCol = phase?col : (width-col-1);
	return &pixels[width*row+unwoundCol];
}


unsigned int getPixel(int row, int col) {
	return rawBuffer[width*row+col];
}
    
void initTCL()  {
    
    int leds = width * height;
    
  /* Open SPI device */
  spi = open("/dev/spidev2.0",O_WRONLY);
  if(spi<0) {
      /* Open failed */
      fprintf(stderr, "Error: SPI device open failed.\n");
      exit(1);
  }

  /* Initialize SPI bus for TCL pixels */
  if(spi_init(spi)<0) {
      /* Initialization failed */
      fprintf(stderr, "Unable to initialize SPI bus.\n");
      exit(1);
  }

  /* Allocate memory for the pixel buffer and initialize it */
  if(tcl_init(&tcl,leds)<0) {
      /* Memory allocation failed */
      fprintf(stderr, "Insufficient memory for pixel buffer.\n");
      exit(1);
  }
 
}
}; // class TCLFast
#endif	/* TCLFAST_HXX */

