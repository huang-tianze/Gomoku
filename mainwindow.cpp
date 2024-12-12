#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
int onChess = 0;
MainWindow::MainWindow(QWidget *parent) // 供main调用的窗口构造函数
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->statusbar->showMessage("Ciallo～(∠・ω< )⌒☆", 5000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::chessBoardDraw() { // deprecated..
    this->centralWidget()->setStyleSheet("background-image: url(:/pic/chessBoard.png);");
}
void MainWindow::hideButton() {
    this->ui->pvpButton->hide();
    this->ui->pveButton->hide();
    this->ui->manualButton->hide();
}
void MainWindow::dropPiece() {}
void MainWindow::on_pvpButton_clicked() {
    chessBoardDraw();
    hideButton();
    // ChessBoard *chessBoard = new ChessBoard; // 如果继承this，会在主页面显示，而QWdiget事实上应该是另一个窗口，与MainWindow同级
    // QPainter painter(chessBoard);
    // QPixmap chessBoardImg;
    // chessBoardImg.load(":/pic/chessBoard.png");
    // painter.drawPixmap(0, 0, 111, 111, chessBoardImg);
    // chessBoard->show();
    // this->hide();
}
void MainWindow::on_exitButton_clicked() {
    close();
}
