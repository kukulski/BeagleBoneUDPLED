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
#include "TCLFast.hxx"

void prepPixels(unsigned int *raw, const tcl_buffer &tcl); 
void initTclBuf(const tcl_buffer &tcl);
void interruptHandler(int param);

int main(int argc, const char * argv[])
{
    signal(SIGINT, interruptHandler); 
    
   fprintf(stderr,"%s running, listening on port 54321\n",argv[0]);
 
   TCLFast tcl(10,10);
   
    UDPListener udp(54321);
    unsigned int *udpBuf = tcl.getRawBuffer();
    size_t bufSize = tcl.getBufferSize();
   
    
    tcl.testPattern();
    tcl.send();
    
    while(1) {
        size_t amount = udp.listen(udpBuf,bufSize);
    	if(amount == bufSize) {
 	    tcl.prepPixels();
            tcl.send();
    	}
    }
    
    return 0;
}



void interruptHandler(int param) {
    
      fprintf(stderr,"quitting\n");
      exit(1);
    
}
