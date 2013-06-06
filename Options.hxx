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
        eatOmissions();
        
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
    list<XY> omissions;
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
    
    
    void eatOmissions() {
        
        while(true) {
            XY omit = eatXY("omit",XY{-1,-1});
            if(omit.x == -1) return;
            omissions.push_back(omit);
        }
    }
};
