#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStatusBar>
#include <QTimer>
#include <QWidget>

#include <algorithm>
#include <ctime>
#include <fstream>
#include <vector>

struct pieceDrop {
    int col;
    int row;
    bool isBlack;
    bool isEnd;
};

namespace Ui {
    class ChessBoard;
}

class ChessBoard : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();

    int pieces[16][16]; // 存储棋盘状态
    std::vector<pieceDrop> pieceDrops;

    bool isPVE = false;
    bool isPlayerFist = true;
    int difficulty = 0;

    bool isAutoSaveOn;

signals:
    void reshowRequested();

private slots:
    void on_homeButton_clicked();
    void on_saveButton_clicked();
    void on_backButton_clicked();

private:
    Ui::ChessBoard *ui;
    QStatusBar *chessStatusBar;
    int cursorX = -1, cursorY = -1;
    int col, row; // 从1开始的
    char colDisplay;
    int rowDisplay;
    int chessX, chessY;
    bool inBoard;
    int round = 1;

    int lastDropRow, lastDropCol;
    int lastDropRowDisplay;
    char lastDropColDisplay;

    bool isBlackOnChess = true;
    bool isBotOnChess = false;
    bool isEnded = false;
    bool isCurBack = false;
    bool isSaved = false;

    QPixmap piecesImg; // 存储棋子图片，构造函数里初始化
    QPixmap lastPiecesImg;

    bool winnerJudge();
    void save();
    void updateChessPos(int, int);
    void drop(int, int);
    void calMousePos(QMouseEvent *);

    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};
QPixmap imgMerge(QPixmap, QPixmap, int, int);
#endif // CHESSBOARD_H
