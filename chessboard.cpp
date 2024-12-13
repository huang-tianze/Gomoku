#include "chessboard.h"
#include "mainwindow.h"
#include "ui_chessboard.h"

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChessBoard) {
    ui->setupUi(this);
    QStatusBar *statusBar = new QStatusBar(this);
    ui->statusBarVerticalLayout->addWidget(statusBar); // 样式已经通过ui的选择器清空
    QLabel *statusLabel = new QLabel("状态: 准备就绪", this);
    statusBar->addWidget(statusLabel);
}

ChessBoard::~ChessBoard() {
    delete ui;
}
void ChessBoard::closeEvent(QCloseEvent *event) {

    this->parentWidget()->show();
    QWidget::closeEvent(event);
}
void ChessBoard::save() {}

void ChessBoard::on_homeButton_clicked() {
    MainWindow *mainWindow = qobject_cast<MainWindow *>(parentWidget());
    if (mainWindow->isAutoSaveOn()) {
        save();
    } else {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "警告",
            "你未开启自动保存对局，如果不点击保存对局，本局的进度将会消失，你是否决定退出？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) return;
    }

    this->close();
    this->parentWidget()->show();
}
