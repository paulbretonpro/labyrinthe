CC = g++
CFLAGS = -std=c++11 -Wall

SRCS = Maze.cpp
OBJS = $(SRCS:.cpp=.o)

EXEC = Maze

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

run: $(EXEC)
	./$(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
