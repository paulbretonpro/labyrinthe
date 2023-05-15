CC = g++
CFLAGS = -std=c++11 -Wall

SRCS = mazer.cpp ../Maze.cpp
OBJS = $(SRCS:.cpp=.o)

EXEC = mazer

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

run: $(EXEC)
	./$(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
