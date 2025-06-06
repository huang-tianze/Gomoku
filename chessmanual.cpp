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
    int row = pieceDrops[round - 1].row;
    int col = pieceDrops[round - 1].col;
    bool isBlackNow = pieceDrops[round - 1].isBlack;

    int rowDisplay = 15 - (row - 1);
    char colDisplay = col - 1 + 'A';
    chessStatusBar->showMessage(QString("%1%2").arg(colDisplay).arg(rowDisplay));
    if (row == 15)
        chessY = 637;
    else
        chessY = (row - 1) * 43 + 37;
    if (col == 15)
        chessX = 641;
    else
        chessX = (col - 1) * 43 + 41;

    QString player;
    if (isBlackNow) {
        player = "黑";
        pieceImg.load(":/pic/blackPiece.png");
    } else {
        player = "白";
        pieceImg.load(":/pic/whitePiece.png");
    }
    ui->chessRecord->append(QString("第%1手，%2方落在了%3%4").arg(round).arg(player).arg(colDisplay).arg(rowDisplay));
    piecesImg = imgMerge(piecesImg, pieceImg, chessX, chessY);
    update();

    if (pieceDrops[round - 1].isEnd) {
        isEnded = true;
        ui->nextButton->setEnabled(false);
        if (pieceDrops[round - 1].isBlack) {
            ui->chessRecord->append("黑方胜利！");
            QMessageBox::information(this, "对局结束！", "黑方胜利！");
        } else {
            ui->chessRecord->append("白方胜利！");
            QMessageBox::information(this, "对局结束！", "白方胜利！");
        }
    }
    if (round >= pieceDrops.size()) {
        isEnded = true;
        ui->nextButton->setEnabled(false);
    }
    round++;
    // qDebug() << chessX << ',' << chessY << '\n';
}

void ChessManual::paintEvent(QPaintEvent *event) {
    // if (isEnded) return; 这段不需要，会导致正常需要绘画的piecesImg跳过绘制
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(0, 0, 1034, 738, piecesImg);
}
