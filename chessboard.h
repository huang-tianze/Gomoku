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

struct pieceDrop { // 由于互相include和重复定义结构体的问题，把pieceDrop的定义搬到这.
    int col;
    int row; // 存的是从0开始的col和row，最大是14(0b1110) // 不使用位域减少大小了，会在文件存储上变麻烦，没法正常读
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
    int round = 1;

    bool isKeyboardOn;
    bool isMouseOn;
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
    bool isPVE = false;
    bool isPlayerFist = true;
    bool isSaved = false;
    int difficulty = 0;
    bool isChessManual = false;
    QPixmap piecesImg; // 存储棋子图片，构造函数里初始化
    QPixmap lastPiecesImg;

    bool winnerJudge();
    void save();

    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};
QPixmap imgMerge(QPixmap, QPixmap, int, int);
#endif // CHESSBOARD_H
