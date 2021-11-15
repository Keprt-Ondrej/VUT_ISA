PROGS = mytftpclient 
CC = g++
CFLAGS = -pedantic -Wall -Wextra -g -Werror #-O2 #TODO verze C??
LOGIN = xkeprt03
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

OFILES = main.o read_from_server.o write_on_server.o comunication.o
HFILES = main.h read_from_server.h write_on_server.h comunication.h

all: $(PROGS)

$(PROGS): $(OFILES) $(HFILES)
	$(CC) $(CFLAGS) -o  $(PROGS) $(OFILES)

doxygen:
	doxygen

pdf:
	$(MAKE) -C dokumentace/

pack: pdf clean 	#zkuste to psat zvrchu dolu, jak to mate napr ve VS code at v tom neni bordel :D
	tar -cv --file=$(LOGIN).tar doxyfile Makefile README.txt *.c  *.cpp *.h manual.pdf

clean:
	$(MAKE) -C dokumentace/ clean
	rm -fr *.o $(PROGS)
