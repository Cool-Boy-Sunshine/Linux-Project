SRC = $(wildcard ./*.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
LFLAG = -L ../lib -pthread -lzlog -lconfig
DFLAG = -lhiredis
CFLAG = -g -I./
BIN = ./server

%.o:%.c
		gcc -c $< -o $@ $(CFLAG)
$(BIN):$(OBJ)
		gcc $^ -o $@ $(LFLAG) $(DFLAG)

clean:
		rm -rf $(BIN) *.o
