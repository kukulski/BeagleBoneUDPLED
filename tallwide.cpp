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

void prepPixels(unsigned int *raw, const tcl_buffer &tcl); 
void initTclBuf(const tcl_buffer &tcl);
void interruptHandler(int param);
using namespace std;



class Options {
public:
  
    Options(int argc, const char * argv[]):
    leds(200),
    port(54321)
    {
      buffersize ={20,10};
      slurp(argc,argv);

      
      
        findStart = args.begin();
        findEnd = args.end();
        buffersize = eatXY("size",buffersize);
        leds = eatInt("leds",leds);
        port = eatInt("port",port);
        
        eatZones();
    }
    
    ~Options() {
      std::list<Zone*>::iterator first = zones.begin(), last = zones.end();
      for ( ; first!=last; ++first ) 
          delete (*first);
    }

    void dump() {
        
        cout << "port:" << port << endl 
                <<"leds:" << leds << endl
                <<"wd:" << buffersize.x << ", ht:" << buffersize.y << endl;
    }
public:
    XY buffersize;
    int leds;
    int port;
    
    vector<string> args;
    list<Zone *> zones;
    
    vector<string>::iterator findStart, findEnd;

private:
    
    int getInt(string &strVal) { return atoi(strVal.c_str());}
    
    XY eatXY(const char *key, XY defaultVal) {
        vector<string>::iterator where, keyloc,xloc,yloc;
        
        where = find(findStart, findEnd,key);
        if(where == findEnd) return defaultVal;
        
 
        
        keyloc = where;
        xloc = where + 1;
        yloc = where + 2;
        

        XY rval{getInt(*xloc),getInt(*yloc)};

        args.erase(keyloc,yloc);
        return rval;
    }
    
    int eatInt(const char *key, int defaultVal) {
        vector<string>::iterator where;
        where = find(findStart,findEnd,key);
        if(where == findEnd) return defaultVal;

        int rval = getInt(where[1]);
        args.erase(where, where+1);
        return rval;
    }
    
    int eatFlag(const char *key) {
        vector<string>::iterator where;
        where = find(findStart,findEnd,key);

        if(where == findEnd) return 0;
     //   cerr << where[0] << endl;
       //  args.erase(where);
        return 1;   
    }
    
    void slurp(int argc, const char **argv) {
     for(int i=0 ; i < argc; i++) 
         args.push_back(argv[i]);
    }
  

    void eatZones() {
        while(findZone())
            zones.push_back(eatZone());
    }

    bool findZone() {
        findStart = find(args.begin(), args.end(), "zone");
        if(findStart == args.end()) return false;
        findStart++;
        
        findEnd = find(findStart,args.end(),"zone");
    }

    Zone *eatZone() {
        
        XY orig = eatXY("origin",XY{0,0});
        XY size = eatXY("size",XY{10,10});
    
        int offset = eatInt("offset",0);
        bool vertical = eatFlag("vertical");
        int flipX = eatFlag("flipX");
        int flipY = eatFlag("flipY");
    
      
        args.erase(findStart,findEnd);
        
        Zone *rval = vertical ? new VZone(orig, size,offset, flipX, flipY) :
            new Zone(orig,size,offset, flipX, flipY);
        
        return rval;
    }
};

int main(int argc, const char * argv[])
{
    
    Options opts(argc, argv);
    
    TCLZoned tcl(opts.buffersize.x, opts.buffersize.y, opts.leds);
   tcl.addZones(opts.zones);
    UDPListener udp(opts.port);

    
    opts.dump();
    
    uint32_t *udpBuf = tcl.getBuffer()->getBuffer();
    size_t bufSize = tcl.getBuffer()->getBufferSize();
   
    
    
    tcl.testPattern();
    
    tcl.send();
    
    int sent = 0;
    
    while(1) {
        size_t amount = udp.listen(udpBuf,bufSize);
    	if(amount == bufSize) {
           tcl.send();
           if(sent == 0) {
               cout << "received and sent a packet!";
               sent++;
           }
    	} else {
            cout << "bad buffer size:" << amount << " expected: " << bufSize << endl;
        }
    }
    
    return 0;
}