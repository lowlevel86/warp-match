CC = gcc
CFLAGS = -Wall
INCS = -Iglad/include

OBJ = main.o glDisplay.o svgLoader.o glad/src/glad.o
LIBS = -lm -lpthread -lglfw -ldl

BIN = prog
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) $(INCS) -c main.c -o main.o

glDisplay.o: glDisplay.c
	$(CC) $(CFLAGS) $(INCS) -c glDisplay.c -o glDisplay.o

svgLoader.o: svgLoader.c
	$(CC) $(CFLAGS) $(INCS) -c svgLoader.c -o svgLoader.o

glad/src/glad.o: glad/src/glad.c
	$(CC) $(CFLAGS) $(INCS) -c glad/src/glad.c -o glad/src/glad.o

