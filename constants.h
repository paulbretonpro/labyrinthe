#ifndef CONSTANTS_H
#define CONSTANTS_H

/** Directions constants */
const int NORTH = 1;
const int SOUTH = 2;
const int EAST = 4;
const int WEST = 8;

const int SOURCE_NORTH = 0;
const int SOURCE_SOUTH = 1;
const int SOURCE_EAST = 2;
const int SOURCE_WEST = 3;

const float ORIENTATION_NORTH[6] = {0, 0, -1, 0, 1, 0};
const float ORIENTATION_SOUTH[6] = {0, 0, 1, 0, 1, 0};
const float ORIENTATION_EAST[6] = {1, 0, 0, 0, 1, 0};
const float ORIENTATION_WEST[6] = {-1, 0, 0, 0, 1, 0};

#endif
