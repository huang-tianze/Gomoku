#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QSize>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <vector>

struct pieceDrop {
    int col, row; // 存的是从0开始的col和row
    bool isBlack;
};

namespace Ui {
    class ChessBoard;
}

class ChessBoard : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    static int pieces[16][16]; // 存储棋盘状态
    std::vector<pieceDrop> pieceDrops;
    int round = 0;

private slots:
    void on_homeButton_clicked();

    void on_backButton_clicked();

private:
    Ui::ChessBoard *ui;
    QStatusBar *chessStatusBar;
    int cursorX = -1, cursorY = -1;
    int col, row; // 从0开始的
    char colDisplay;
    int rowDisplay;
    int chessX, chessY;

    int lastDropRow, lastDropCol; // 从0开始
    int lastDropRowDisplay;
    char lastDropColDisplay;
    bool isBlackOnChess = true;
    bool isEnded = false;
    bool isCurBack = false;
    QPixmap piecesImg; // 存储棋子图片，构造函数里初始化
    QPixmap lastPiecesImg;

    QPixmap imgMerge(QPixmap, QPixmap, int, int);
    void winnerJudge();
    void save();
    void load();

    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CHESSBOARD_H
