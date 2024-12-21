#ifndef ROBOT_H
#define ROBOT_H

#include "define.h"
#include <iostream>
#include <limits>
#include <stack>
#include <vector>

std::vector<coord> getValidDrops(borderPos);
int evaluate();
int minimax_alphabeta(int depth, bool isMaximizing, int alpha, int beta, borderPos);
coord invokeBot(int[16][16], borderPos, int, bool);
void dropChess(coord, int);
void undoDrop(coord);
borderPos updateBorder(int, int, borderPos);
borderPos updateBorder(coord, borderPos);

#endif // ROBOT_H