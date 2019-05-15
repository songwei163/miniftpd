.PHONY:clean

CC=gcc
CFLAGS=-Wall -g
BIN=minftpd
OBJS=main.o sysutil.o common.o session.o ftpproto.o privparent.o str.o

$(BIN):$(OBJS)
		$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
		$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)