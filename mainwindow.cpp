#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
int onChess = 0;
MainWindow::MainWindow(QWidget *parent) // 供main调用的窗口构造函数
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->statusbar->showMessage("Ciallo～(∠・ω< )⌒☆");
}

MainWindow::~MainWindow() {
    delete ui;
}

// void MainWindow::chessBoardDraw() { // deprecated..
//     this->centralWidget()->setStyleSheet("background-image: url(:/pic/chessBoard.png);");
// }
bool MainWindow::isAutoSaveOn() {
    if (this->ui->actionAutoSave->isChecked())
        return true;
    else
        return false;
}
bool MainWindow::isKeyboardOn() {
    if (this->ui->actionKeyboardOn->isChecked())
        return true;
    else
        return false;
}
bool MainWindow::isMouseOn() {
    if (this->ui->actionMouseOn->isChecked())
        return true;
    else
        return false;
}
void MainWindow::hideButton() {
    this->ui->pvpButton->hide();
    this->ui->pveButton->hide();
    this->ui->manualButton->hide();
}

void MainWindow::on_pvpButton_clicked() {
    chessBoard = new ChessBoard(this);     // 由于某种未知原因，需要在chessBoard的UI内加入一个Widget来显示内容，主窗口不显示内容, this继承父关系
    chessBoard->setWindowFlag(Qt::Window); // 将chessBoard的窗口设置在新的窗口，防止其直接在MainWindow内部显示（但保留与mainwindow的父子关系）
    chessBoard->setWindowTitle("标题信息待定");
    chessBoard->show();
    this->hide();
}
void MainWindow::on_exitButton_clicked() {
    close();
}

void MainWindow::on_continueButton_clicked() {
}
