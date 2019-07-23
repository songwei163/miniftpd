CC=gcc
CFLAGS=-Wall -g
LIBS=-lcrypt
PROJ=miniftpd
OBJ=main.o session.o sysutil.o ftpproto.o privparent.o str.o tunable.o \
parseconf.o privsock.o hash.o
$(PROJ):$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(PROJ) $(OBJ)
