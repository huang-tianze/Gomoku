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
    chessBoard->isAutoSaveOn = isAutoSaveOn();
    // chessBoard->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    // 将chessBoard的窗口设置在新的窗口，防止其直接在MainWindow内部显示（但保留与mainwindow的父子关系）
    // 由于QWidget难以增加任务栏图标，也不方便做了大半再转成QMainwindow，故设置为常在最前窗口
    chessBoard->setWindowTitle("双人对局");
    chessBoard->show();
    this->hide();
}

void MainWindow::on_exitButton_clicked() {
    close();
}

void MainWindow::on_manualButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "D:/Files/SDU/2024ProgramDesign/Project/2-Assignment/Gomoku/chessManual", "棋谱二进制文件 (*.dat)");
    // qDebug() << fileName;
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
        if (inFile.eof()) break; // 不加eof判断的话，不知道为什么最后一条数据会重复一次
        chessManual->pieceDrops.push_back(drop);
    }
    inFile.close();
    // qDebug() << chessManual->pieceDrops.size();
    chessManual->show();
    this->hide();
}

void MainWindow::reshow() {
    this->show();
}

void MainWindow::on_actionOpen_triggered() {
    on_manualButton_clicked();
}

void MainWindow::on_pveButton_clicked() {
    chessBoard = new ChessBoard;
    connect(chessBoard, &ChessBoard::reshowRequested, this, &MainWindow::reshow);
    chessBoard->isAutoSaveOn = isAutoSaveOn();
    chessBoard->isPVE = true;

    QMessageBox msgBox;
    msgBox.setWindowTitle("选择棋子颜色");
    msgBox.setText("你希望选择黑棋还是白棋？（黑棋先手）");
    QPushButton *yes = msgBox.addButton("黑棋", QMessageBox::YesRole);
    QPushButton *no = msgBox.addButton("白棋", QMessageBox::NoRole);
    msgBox.exec();
    if (msgBox.clickedButton() == yes) {
        chessBoard->isPlayerFist = true;
    } else {
        chessBoard->isPlayerFist = false;
    }

    chessBoard->setWindowTitle("人机对战");
    chessBoard->show();
    this->hide();
}
