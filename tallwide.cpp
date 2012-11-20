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
#include <signal.h>
#include "TCLZoned.hxx"

void prepPixels(unsigned int *raw, const tcl_buffer &tcl); 
void initTclBuf(const tcl_buffer &tcl);
void interruptHandler(int param);

int main(int argc, const char * argv[])
{
//    signal(SIGINT, interruptHandler); 
    
   fprintf(stderr,"%s running, listening on port 54321\n",argv[0]);
 
    TCLZoned tcl(22,12,200);
    tcl.add(Zone(7,0,10,7,0,1));
    tcl.add(Zone(0,7,22,5,100,1));
 
   
    UDPListener udp(54321);
    uint32_t *udpBuf = tcl.getBuffer()->getBuffer();
    size_t bufSize = tcl.getBuffer()->getBufferSize();
   
    tcl.testPattern();
    
    tcl.send();
    
    while(1) {
        size_t amount = udp.listen(udpBuf,bufSize);
    	if(amount == bufSize) {
           tcl.send();
    	}
    }
    
    return 0;
}



void interruptHandler(int param) {
    
      fprintf(stderr,"quitting\n");
      exit(1);
    
}
