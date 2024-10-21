SOURCE = jobs.c server.c list.c main.c
OBJS = jobs.o server.o list.o main.o
HEADER = jobs.h list.h
OUT = jobCommander
FILES = jobExecutorServer.txt ./fifo1 ./fifo2
CC = gcc
FLAGS = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT)

main.o : main.c
	$(CC) $(FLAGS) main.c

jobs.o : jobs.c
	$(CC) $(FLAGS) jobs.c

server.o : server.c
	$(CC) $(FLAGS) server.c

list.o : list.c
	$(CC) $(FLAGS) list.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean : 
	rm -f $(OBJS) $(OUT) $(FILES)

rebuild : clean build