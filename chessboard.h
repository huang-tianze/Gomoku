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

#include "define.h"
#include "robot.h"
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
    bool isPlayerFirst = true;
    int difficulty = 0;
    bool botThinking = false;
    void botInit();

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
    // mouseTracking
    int cursorX = -1, cursorY = -1;
    int cursorCol, cursorRow; // 从1开始的
    char cursorColDisplay;
    int cursorRowDisplay;
    int cursorChessX, cursorChessY;
    bool cursorInBoard;
    // drop piece
    int dropCol, dropRow;
    int dropRowDisplay;
    char dropColDisplay;
    int dropChessX, dropChessY;
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
    void drop(const int &, const int &);
    void drop(const coord &);
    void calMousePos(QMouseEvent *);

    borderPos border;
    void updateBorder();
    void botDrop();

    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CHESSBOARD_H
