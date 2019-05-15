.PHONY:clean

CC=gcc
CFLAGS=-Wall -g
BIN=minfitpd
OBJS=main.o sysutil.o common.o session.o ftpproto.o privparent.o str.o tunable.o parseconf.o

$(BIN):$(OBJS)
		$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
		$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)