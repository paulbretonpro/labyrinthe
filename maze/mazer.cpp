#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "../Maze.h"

int main(int argc, char** argv) {
    size_t width = argc > 1 ? std::stoi(argv[1]) : 10;
    size_t height = argc > 2 ? std::stoi(argv[2]) : width;
    size_t weight = argc > 3 ? std::stoi(argv[3]) : 2;
    int seed = argc > 4 ? std::stoi(argv[4]) : time(nullptr);

    Maze* maze = new Maze(width, height, weight, seed);
    maze->export_digital("maze.num");
    maze->export_graphical("maze.txt");

    std::cout << argv[0] << " " << width << " " << height << " " << weight << " " << seed << std::endl;

    return 0;
}
