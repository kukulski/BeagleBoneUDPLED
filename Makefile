CFLAGS = -O3
CC = gcc
BUNDLE = Makefile main.cpp UDPSender.cpp UDPSender.h tclled.h tclled.c
VERSION = 1.1
ARCHIVE = udplights
LIBS = -lm

all: udplights

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



main.o: main.cpp UDPSender.h

UDPSender.o: UDPSender.cpp UDPSender.h

tclled.o: tclled.c tclled.h