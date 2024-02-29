CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp) -gdwarf-4
LFLAGS = $(shell pkg-config --libs gmp)

all:keygen encrypt decrypt

numtheory:randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

randstate: randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

ss: keygen.o ss.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

keygen: keygen.o randstate.o ss.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o randstate.o ss.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

decrypt: decrypt.o randstate.o ss.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f keygen encrypt decrypt numtheory randsate *.o

cleankeys:
	rm -f *.{pub,priv}

format:
	clang-format -i -style=file *.[ch]

