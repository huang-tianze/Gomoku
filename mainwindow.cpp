#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) // 供main调用的窗口构造函数
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->statusbar->showMessage("Ciallo～(∠・ω< )⌒☆", 5000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::chessBoardDraw() {
    this->centralWidget()->setStyleSheet("background-image: url(:/pic/chessBoard.png);");
}
void MainWindow::dropPiece() {}
void MainWindow::on_pvpButton_clicked() {
    chessBoardDraw();
}
