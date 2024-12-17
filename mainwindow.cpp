#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
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
    chessBoard = new ChessBoard;
    // 由于某种未知原因，需要在chessBoard的UI内加入一个Widget来显示内容，主窗口不显示内容, this继承父关系(由于通过信号与槽重构了显示方法，不需要继承关系了，也不需要setWindowFlag了)
    // 连接子窗口的信号
    connect(chessBoard, &ChessBoard::reshowRequested, this, &MainWindow::reshow);
    chessBoard->isKeyboardOn = isKeyboardOn();
    chessBoard->isAutoSaveOn = isAutoSaveOn();
    chessBoard->isMouseOn = isMouseOn();
    // chessBoard->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    // 将chessBoard的窗口设置在新的窗口，防止其直接在MainWindow内部显示（但保留与mainwindow的父子关系）
    // 由于QWidget难以增加任务栏图标，也不方便做了大半再转成QMainwindow，故设置为常在最前窗口
    chessBoard->setWindowTitle("标题信息待定");
    chessBoard->show();
    this->hide();
}
void MainWindow::on_exitButton_clicked() {
    close();
}
void MainWindow::on_manualButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "", "棋谱二进制文件 (*.dat)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "提示", "未选择任何文件！");
        return;
    }
    std::ifstream inFile(fileName.toStdString(), std::ios::binary);
    if (!inFile.is_open()) {
        QMessageBox::warning(this, "错误", "无法打开文件！");
        return;
    }
    // 由于光标的问题，需要创建窗口在文件选择之后，不然修正光标的Timer会计入选文件时候的时间
    chessManual = new ChessManual;
    connect(chessManual, &ChessManual::reshowRequested, this, &MainWindow::reshow);
    chessManual->setWindowTitle("棋谱查看");

    while (inFile) {
        pieceDrop drop;
        inFile.read(reinterpret_cast<char *>(&drop), sizeof(pieceDrop));
        chessManual->pieceDrops.push_back(drop);
    }
    inFile.close();
    qDebug() << chessManual->pieceDrops.size();
    chessManual->show();
    this->hide();
}
void MainWindow::reshow() {
    this->show();
}