# CFLAGS = -O3
CFLAGS = -std=c++0x
CC = g++
BUNDLE = Makefile main.cpp UDPSender.cpp UDPSender.h tclled.h tclled.c
VERSION = 1.1
ARCHIVE = udplights
LIBS = -lm

all: udplights tallwide

archive: $(BUNDLE)
	mkdir $(ARCHIVE)-$(VERSION)
	cp $(BUNDLE) $(ARCHIVE)-$(VERSION)/
	tar cvfz $(ARCHIVE)-$(VERSION).tar.gz $(ARCHIVE)-$(VERSION)
	rm -rf $(ARCHIVE)-$(VERSION)

clean:
	$(RM) *.o
	$(RM) $(ARCHIVE)-$(VERSION).tar.gz

udplights: main.o UDPSender.o tclled.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

udpfast: udpfast.o UDPSender.o tclled.o
	$(CC) $(CFLAGS) $(LIBS) -lpthread -o $@ $^

tallwide: tallwide.o UDPSender.o tclled.o 
	$(CC) $(CFLAGS) $(LIBS)  -o $@ $^

tallwide.o: tallwide.cpp UDPSender.h TCLZoned.hxx 

tclled.o: tclled.c tclled.h

udpfast.o: udpfast.cpp UDPSender.h

UDPSender.o: UDPSender.cpp UDPSender.h

main.o: main.cpp UDPSender.h TCLFast.hxx 



	
	
