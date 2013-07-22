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
#include <math.h>
#include <algorithm>
#include <list>

#include <iostream>

using namespace std;


class Buffer {
public:
    Buffer(int wd, int ht):
        wd(wd),
        ht(ht),
        zero(0){
        size = wd*ht;
        buf = new uint32_t [size];
        
         
         for(int i = 0 ; i < size; i++)      
             buf[i] = 0;
               
    }
    
    ~Buffer() {
        delete buf;
    }
    
    uint32_t *pixelAt(int x, int y) {
        return buf + offset(x,y);
    }
    
    int offset(int x, int y) {
        return x + (y * wd);
        
    }
    
    uint32_t *getZero() { return &zero;}
    
    uint32_t *getBuffer() { return pixelAt(0,0);}
    size_t getBufferSize() { return wd*ht*sizeof(uint32_t);}
    
    
    void writePixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
       
        uint32_t *out = pixelAt(x,y);
        
         buf[offset(x,y)] = (r << 8) | (g << 16) | (b << 24);
    }
    

private:
    int size;
    int wd, ht;
    uint32_t *buf;
    uint32_t zero;
};

typedef struct {
  int x, y;  
} XY;

class Zone {
 
public: 
//    Zone(const int x, const int y, const int wd, const int ht, const int offset, const int phase):xOrigin(x),yOrigin(y),wd(wd),ht(ht),offset(offset),phase(phase) {
//    }
    
       Zone(XY &origin, XY &size, const int offset, const int flipX, const int flipY):
        origin(origin),
          size(size),
          offset(offset),
          flipX(flipX),
           flipY(flipY) {
    }

    int endIndex() {
        return size.x*size.y + offset;
    }
    
    virtual void dump() {
        fprintf(stderr,"zone: @%d,%d, %dx%d offset:%d, phase%d,%d\n",origin.x, origin.y, size.x, size.y, offset, flipX, flipY);
    }
    
    virtual unsigned int *pixelForIndex(int ii) {
        
        int i = ii- offset;
        int suby = i/size.x;
        
        int linePhase = (suby + flipX) & 1;
        int subx = i%size.x;
        subx = linePhase ? (size.x - 1 - subx) : subx;
        suby = flipY ? (size.y -1 - suby) : suby;
        
        
        return rawBuffer->pixelAt(origin.x + subx, origin.y+suby);
    }
protected:
    XY origin, size;
    int flipX, flipY;
public:
    int offset;
    Buffer *rawBuffer;
};

class VZone:  public Zone {
public:
    
    VZone(XY &origin, XY &size, const int offset, const int flipX, const int flipY):
    Zone(origin,size,offset,flipX, flipY) {
    }
    
    virtual unsigned int *pixelForIndex(int ii) {
        
        int i = ii- offset;
        int subx = i/size.y;
        
        int linePhase = (subx + flipX) & 1;
        int suby = i%size.y;
        suby = linePhase ? (size.y - 1 - suby) : suby;
        subx = flipY ? (size.x -1 - subx) : subx;
     
        
        return rawBuffer->pixelAt(origin.x + subx, origin.y+suby);
    }
        virtual void dump() {
         fprintf(stderr,"vertical zone: @%d,%d, %dx%d offset:%d, phase%d,%d\n",origin.x, origin.y, size.x, size.y, offset, flipX, flipY);
    }

};



class BufferMap {
    
public:
    BufferMap(int count, Buffer *rawBuffer, tcl_color *pixels):
    count(count),
    rawBuffer(rawBuffer),
    pixels(pixels) {
        pixelMap = new uint32_t*[count];
        
        for(int i=0; i < count; i++) {
            pixelMap[i] = rawBuffer->getBuffer();
        }
    }
    
    
    void add(Zone *zone) {
        
      //  zone->dump();
        
        zone->rawBuffer = rawBuffer;
        int endIdx = std::min(count,zone->endIndex());
        
        for(int i = zone->offset; i < endIdx; i++) 
            pixelMap[i] = zone->pixelForIndex(i);
 
    }
    
    void writeOut(tcl_color *pixels) {
        for(int i = 0; i < count; i++) {
            uint32_t *from = pixelMap[i];
            int offset = from - rawBuffer->getBuffer();
            ::write_bgra_gamma(pixels+i,*from);
        }
    }
    
    
    void mapOutBadPixel(uint32_t *px) {
        int idx = findIndex(px);
        if(idx < 0) return;
        if(idx >= count) return;
        mapOut(idx);
    }
    
    
    
private:
    void mapOut(int idx) {
        for(int i = idx; i < count-1; i++) {
            pixelMap[i] = pixelMap[i+1];
        }
    }
    
    int findIndex(uint32_t *px) {
        
        for(int i = 0; i < count; i++) {
            uint32_t *from = pixelMap[i];
            if(from == px) return i;
        }
        return -1;
    }
    

    int count;
        uint32_t **pixelMap;
        Buffer *rawBuffer;
        tcl_color *pixels;
};


class TCLZoned {
    
public:
    
    TCLZoned(int wd, int ht, int pixelCount):
        width(wd),
        height(ht),
        spi(-1)
        {
       set_gamma(3.0, 3.0, 3.0);
       initTCL(pixelCount);

       rawBuffer = new Buffer(wd,ht);
       pixelMap = new BufferMap(pixelCount, rawBuffer, tcl.pixels);
    }
    
    ~TCLZoned() {
        delete rawBuffer;
    }
  
    void mapOutBadPixel(XY px) {
        pixelMap->mapOutBadPixel(rawBuffer->pixelAt(px.x,px.y));
    }
    
 Buffer *getBuffer() { return rawBuffer;}
    
  
 
 void addZones(std::list<Zone*> zones) {
     std::list<Zone*>::iterator first = zones.begin(), last = zones.end();
      for ( ; first!=last; ++first ) 
          add(*first);
 }
    
    void addOmissions(std::list<XY> omissions) {
        std::list<XY>::iterator first = omissions.begin(), last = omissions.end();
        for ( ; first!=last; ++first ) {
            cout << "omitting " << first->x << first->y << endl;
            mapOutBadPixel(*first);
        }
    }
 
    void add(Zone *zone) {
        
        pixelMap->add(zone);
    }
    
    
    void testPattern() {
    for(int i = 0; i < width*height; i++) {
        int row = i/width;
        int col = i%width;
        int quad = i /(width*height/4);
        rawBuffer->writePixel(col,row, row*255/height,col*255/width,0);
    }
}
    
    void send() {
        
      if(spi < 0)
          openSPI();
      
      if(spi < 0)
          return;
        
       pixelMap->writeOut(tcl.pixels);
       ::send_buffer(spi,&tcl);
    }
    
private:
    tcl_buffer tcl;
    BufferMap *pixelMap;
    Buffer *rawBuffer;
    int width, height;
    int spi;
    int leds;
    

  
    
    
    
void openSPI() {
     /* Open SPI device */
  spi = open("/dev/spidev2.0",O_WRONLY);
  if(spi<0) {
      /* Open failed */
      fprintf(stderr, "Error: SPI device open failed.\n");
      return;
  }

  /* Initialize SPI bus for TCL pixels */
  if(spi_init(spi)<0) {
      /* Initialization failed */
      fprintf(stderr, "Unable to initialize SPI bus.\n");
      close(spi);
      spi = -1;
      return;
  } 
    
}    
    
void initTCL(int leds)  {
    


  /* Allocate memory for the pixel buffer and initialize it */
  if(tcl_init(&tcl,leds)<0) {
      /* Memory allocation failed */
      fprintf(stderr, "Insufficient memory for pixel buffer.\n");
      exit(1);
  }
 
}
}; // class TCLFast


/// next: make a new class  that uses pixelDither


#endif	/* TCLFAST_HXX */

