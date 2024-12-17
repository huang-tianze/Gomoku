#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chessboard.h"
#include "chessmanual.h"
#include <QFileDialog>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool isAutoSaveOn();
    bool isKeyboardOn();
    bool isMouseOn();

private slots:
    // void chessBoardDraw();
    void hideButton();

    void on_pvpButton_clicked();
    void on_exitButton_clicked();
    void on_manualButton_clicked();
    void reshow();

private:
    Ui::MainWindow *ui;
    ChessBoard *chessBoard;
    ChessManual *chessManual;
};
#endif // MAINWINDOW_H
