SRC = $(wildcard ./*.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
LFLAG = -L ../lib -pthread -lzlog -lconfig -lcrypto
CFLAG = -g -I./
BIN = ./client

%.o:%.c
		gcc -c $< -o $@ $(CFLAG)
$(BIN):$(OBJ)
		gcc $^ -o $@ $(LFLAG)

clean:
		rm -rf $(BIN) *.o
