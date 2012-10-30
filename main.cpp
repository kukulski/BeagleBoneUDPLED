//
//  main.c
//  udpListen
//
//  Created by Timothy Kukulski on 10/30/12.
//  Copyright (c) 2012 Adobe. All rights reserved.
//


#include "tclled.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "UDPSender.h"
#include <stdlib.h>


void prepPixels(unsigned int *raw, const tcl_buffer &tcl); 
int main(int argc, const char * argv[])
{

 int fd;              /* SPI device file descriptor */
  const int leds = 100; /* 50 LEDs in the strand */
  tcl_buffer tclBuf;
 
  /* Open SPI device */
  fd = open("/dev/spidev2.0",O_WRONLY);
  if(fd<0) {
      /* Open failed */
      fprintf(stderr, "Error: SPI device open failed.\n");
      exit(1);
  }

  /* Initialize SPI bus for TCL pixels */
  if(spi_init(fd)<0) {
      /* Initialization failed */
      fprintf(stderr, "Unable to initialize SPI bus.\n");
      exit(1);
  }

  /* Allocate memory for the pixel buffer and initialize it */
  if(tcl_init(&tclBuf,leds)<0) {
      /* Memory allocation failed */
      fprintf(stderr, "Insufficient memory for pixel buffer.\n");
      exit(1);
  }


    UDPListener udp(43211);
    unsigned int udpBuf[100];
    
    
    while(1) {
        size_t amount = udp.listen(udpBuf,sizeof(udpBuf));
    
    	prepPixels(udpBuf, tclBuf);
		send_buffer(fd,&tclBuf);
    
    }
    
    return 0;
}


void writePixel(unsigned int *raw, tcl_color *pixels, int row, int col);
unsigned int getPixel(unsigned int *raw, int row, int col);
tcl_color *getTCLPixel(tcl_color *pixels, int row, int col);

void prepPixels( unsigned int *raw, const tcl_buffer &tcl) {

	tcl_color *pixels = tcl.pixels;
	
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
			writePixel(raw, pixels,i,j);
}

void writePixel(unsigned int *raw, tcl_color *pixels, int row, int col) {
	unsigned int px = getPixel(raw,row,col);
	write_bgra(getTCLPixel(pixels,row,col),px);
}
tcl_color *getTCLPixel(tcl_color *pixels, int row, int col) {
	int phase = row & 1;
	int unwoundCol = phase?col : 9-col;
	return &pixels[10*row+unwoundCol];
}


unsigned int getPixel( unsigned int *raw, int row, int col) {
	return raw[10*row+col];
}
