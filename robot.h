#ifndef ROBOT_H
#define ROBOT_H

#include "define.h"
#include <QDebug>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

// std::vector<coord> getValidDrops(borderPos);
// std::vector<coord> getChess(borderPos border);
// int evaluate(borderPos);
// std::string getLine(coord, int, int, int);
// int evaluateLine(coord piece, int dx, int dy, int chess);
// int minimax_alphabeta(int depth, bool isMaximizing, int alpha, int beta,
//                       borderPos);
// coord invokeBot(int[16][16], borderPos, int, bool);
// void dropChess(coord, int);
// void undoDrop(coord);
// void inverseVector(std::vector<int> &);
// borderPos updateBorder(int, int, borderPos);
// borderPos expandBorder(borderPos, int);
// borderPos updateBorder(coord, borderPos);
// bool win(coord);
// int chessCount(coord dropPos, int dx, int dy);
// bool inBoard(int);

// void debugOutput(int[16][16]);
coord invokeBot(int[16][16], coord, bool);

#endif // ROBOT_H
