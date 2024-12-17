#include "chessmanual.h"
#include "ui_chessmanual.h"

ChessManual::ChessManual(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChessManual) {
    ui->setupUi(this);

    QIcon icon(":/pic/icon.svg");
    setWindowIcon(icon);
    chessStatusBar = new QStatusBar(this);
    ui->statusLayout->addWidget(chessStatusBar);
    chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    setAttribute(Qt::WA_StyledBackground, true);
    piecesImg.load(":/pic/transparent.png");
    lastPiecesImg.load(":/pic/transparent.png");
    this->setMouseTracking(true);
    QTimer::singleShot(10, this, [this]() {
        this->setCursor(Qt::ArrowCursor);
    });
}

ChessManual::~ChessManual() {
    delete ui;
}

void ChessManual::closeEvent(QCloseEvent *event) {

    emit reshowRequested();
    QWidget::closeEvent(event);
}
void ChessManual::on_homeButton_clicked() {
    this->close();
    emit reshowRequested();
}
void ChessManual::on_nextButton_clicked() {
    qDebug() << pieceDrops.size();
    if (pieceDrops[round].isEnd) {
        isEnded = true;
        ui->nextButton->setEnabled(false);
        if (pieceDrops[round].isBlack) {
            ui->chessRecord->append("黑方胜利！");
        } else {
            ui->chessRecord->append("白方胜利！");
        }
    }
    int row = pieceDrops[round].row;
    int col = pieceDrops[round].col;
    bool isBlackNow = pieceDrops[round].isBlack;

    int rowDisplay = 15 - row;
    char colDisplay = col + 'A';
    chessStatusBar->showMessage(QString("%1%2").arg(colDisplay).arg(rowDisplay));
    if (row + 1 == 15) chessY = 637;
    if (col + 1 == 15) chessX = 641;
    if (row != 15 && col != 15) {
        chessY = row * 43 + 37;
        chessX = col * 43 + 41;
    }
    QString player;
    if (isBlackNow) {
        player = "黑";
        pieceImg.load(":/pic/blackPiece.png");
    } else {
        player = "白";
        pieceImg.load(":/pic/whitePiece.png");
    }
    ui->chessRecord->append(QString("第%1手，%2方落在了%3%4").arg(round + 1).arg(player).arg(colDisplay).arg(rowDisplay));

    imgMerge(piecesImg, pieceImg, chessX, chessY);
    update();

    round++;
    if (round >= pieceDrops.size()) {
        isEnded = true;
        ui->nextButton->setEnabled(false);
    }
}
void ChessManual::paintEvent(QPaintEvent *event) {
    if (isEnded) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(0, 0, 1034, 738, piecesImg);
}