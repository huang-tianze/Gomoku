#ifndef DEFINE_H
#define DEFINE_H
#include <QPainter>
#include <vector>
struct pieceDrop {
    int col;
    int row;
    bool isBlack;
    bool isEnd;
};

typedef std::pair<int, int> coord;

struct borderPos {
    coord pos1; // 左上角
    coord pos2; // 右下角
};

QPixmap imgMerge(QPixmap, QPixmap, int, int);

coord chessPosCalculate(const int &, const int &);

#endif // DEFINE_H