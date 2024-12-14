#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui {
    class ChessBoard;
}

class ChessBoard : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    static int pieces[16][16]; // 存储棋盘状态
    // static pair 存储过程.

private slots:
    void on_homeButton_clicked();

private:
    Ui::ChessBoard *ui;
    QStatusBar *chessStatusBar;
    int cursorX = -1, cursorY = -1;
    bool isBlackOnChess = true;

    void dropPiece();
    void save();
    void load();
    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // CHESSBOARD_H
