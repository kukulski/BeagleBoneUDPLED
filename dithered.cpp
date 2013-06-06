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
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include "Options.hxx"

void prepPixels(unsigned int *raw, const tcl_buffer &tcl); 
void initTclBuf(const tcl_buffer &tcl);
void interruptHandler(int param);
using namespace std;

int main(int argc, const char * argv[])
{
    
    Options opts(argc, argv);
    
    TCLZoned tcl(opts.buffersize.x, opts.buffersize.y, opts.leds);
   tcl.addZones(opts.zones);
    UDPListener udp(opts.port);

    udp.setNonblocking();
    
    opts.dump();
    
    uint32_t *udpBuf = tcl.getBuffer()->getBuffer();
    size_t bufSize = tcl.getBuffer()->getBufferSize();
   
    
    
    tcl.testPattern();
    tcl.send();
    
    
    while(1) {
        size_t amount = udp.listen(udpBuf,bufSize);
    	if(amount == bufSize) {
           tcl.send();
    	} else {
            cout << "bad buffer size:" << amount << " expected: " << bufSize << endl;
        }
    }
    
    return 0;
}