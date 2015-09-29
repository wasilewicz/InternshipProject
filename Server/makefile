CFLAGS = 
LIBS = -lm

OBJ =\
	main.o

all: main

.c.o:
	$(CC) -c $(INCLUDES) $(CFLAGS) $<

main: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o server
