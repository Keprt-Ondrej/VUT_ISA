PROGS = mytftpclient 
CC = g++
CFLAGS = -pedantic -Wall -Wextra -g -Werror #-O2 #TODO verze C??
LOGIN = xkeprt03
#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

OFILES = main.o server.o client.o
HFILES = main.h server.h client.h 

all: $(PROGS)

$(PROGS): $(OFILES) $(HFILES)
	$(CC) $(CFLAGS) -o  $(PROGS) $(OFILES)

doxygen:
	doxygen

pdf:
	$(MAKE) -C dokumentace/

pack: pdf	#zkuste to psat zvrchu dolu, jak to mate napr ve VS code at v tom neni bordel :D
	zip $(LOGIN).zip doxyfile Makefile README.txt *.c  *.cpp *.h dokumentace.pdf

clean:
	$(MAKE) -C dokumentace/ clean
	rm -fr *.o $(PROGS) dokumentace.pdf
