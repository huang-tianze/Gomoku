#ifndef ROBOT_H
#define ROBOT_H

#include "define.h"
#include <QDebug>
#include <iostream>
#include <limits>
#include <stack>
#include <vector>

std::vector<coord> getValidDrops(borderPos);
std::vector<coord> getSubboard(borderPos border);
int evaluate(borderPos);
std::pair<int, int> patternMatch(coord);
int minimax_alphabeta(int depth, bool isMaximizing, int alpha, int beta, borderPos);
coord invokeBot(int[16][16], borderPos, int, bool);
void dropChess(coord, int);
void undoDrop(coord);
std::vector<int> processBlank(std::vector<int>, int);
void inverseVector(std::vector<int> &);
borderPos updateBorder(int, int, borderPos);
borderPos expandBorder(borderPos, int);
borderPos updateBorder(coord, borderPos);

void debugOutput(int[16][16]);

#endif // ROBOT_H