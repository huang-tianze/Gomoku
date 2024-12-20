#ifndef CHESSMANUAL_H
#define CHESSMANUAL_H

#include <QPainter>
#include <QStatusBar>
#include <QTimer>
#include <QWidget>

#include "chessboard.h"

namespace Ui {
    class ChessManual;
}

class ChessManual : public QWidget {
    Q_OBJECT

private slots:
    void on_homeButton_clicked();
    void on_nextButton_clicked();

public:
    explicit ChessManual(QWidget *parent = nullptr);
    ~ChessManual();
    int round = 1;
    std::vector<pieceDrop> pieceDrops;
    int totalRound; //  = pieceDrops.size(); 不能在类里赋值，因为数据是后塞入的，需要在赋值后再初始化
signals:
    void reshowRequested();

private:
    Ui::ChessManual *ui;
    QStatusBar *chessStatusBar;
    int chessX, chessY;
    bool isEnded;
    QPixmap pieceImg;
    QPixmap piecesImg;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // CHESSMANUAL_H
