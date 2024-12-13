#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
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

private slots:
    void on_homeButton_clicked();

private:
    Ui::ChessBoard *ui;
    void save();
    void closeEvent(QCloseEvent *event) override; // 重写关闭事件，使得关闭子窗口的时候，主窗口能显示
};

#endif // CHESSBOARD_H
