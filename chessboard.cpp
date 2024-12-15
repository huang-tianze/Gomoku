#include "chessboard.h"
#include "mainwindow.h"
#include "ui_chessboard.h"

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChessBoard) {
    ui->setupUi(this);

    // 状态栏
    chessStatusBar = new QStatusBar(this);
    ui->statusLayout->addWidget(chessStatusBar);
    chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    // qDebug() << "window size:" << this->size();

    // 背景 QWidget默认禁用了styledBackground，使用QSS绘制需要启用
    setAttribute(Qt::WA_StyledBackground, true);
    piecesImg.load(":/pic/transparent.png");

    // 光标
    this->setMouseTracking(true);
    // ui->centralWidget->setMouseTracking(true); 由于解决了ChessBoard背景的问题，替代品的centralWidget就删除了，这样也不需要chessBoard和centralWidget同时设置track了
    QTimer::singleShot(10, this, [this]() {
        this->setCursor(Qt::ArrowCursor);
    });
    // 不知道什么原因，打开窗口的时候光标会变成拉伸光标，这里做个10ms的延时改光标纠正一下
}
// 棋盘状态初始化(静态成员需要在这里额外定义)
int ChessBoard::pieces[16][16] = {{0}};

ChessBoard::~ChessBoard() {
    delete ui;
}

void ChessBoard::closeEvent(QCloseEvent *event) {

    this->parentWidget()->show();
    QWidget::closeEvent(event);
}
void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    cursorX = pos.x();
    cursorY = pos.y();
    if (cursorX > 670 || cursorY > 670 || cursorX < 41 || cursorY < 37) {
        chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    } else {
        row = (cursorY - 37) / 43;
        rowDisplay = 15 - row;
        col = (cursorX - 41) / 43;
        colDisplay = col + 'A';
        chessStatusBar->showMessage(QString("%1%2").arg(colDisplay).arg(rowDisplay));
        if (row + 1 == 15) chessY = 637;
        if (col + 1 == 15) chessX = 641;
        if (row != 15 && col != 15) {
            chessY = row * 43 + 37;
            chessX = col * 43 + 41;
        }
    }
    update(); // 让Qt更新部件，会在事件队列里加入一个QPaintEvent
}
// 棋子合格位置 :600 596, 移动距离43
void ChessBoard::paintEvent(QPaintEvent *event) {
    // predrop的棋子显示
    QPainter painter(this);
    painter.drawPixmap(0, 0, 1034, 738, piecesImg);
    if (cursorX > 670 || cursorY > 670 || cursorX < 41 || cursorY < 37) return;

    painter.setRenderHint(QPainter::Antialiasing);
    // int row = (cursorY - 37) / 43;
    // int col = (cursorX - 41) / 43;
    // int prePieceY, prePieceX;
    // if (row + 1 == 15) prePieceY = 637;
    // if (col + 1 == 15) prePieceX = 641;
    // if (row != 15 && col != 15) {
    //     prePieceY = row * 43 + 37;
    //     prePieceX = col * 43 + 41;
    // }
    if (pieces[row + 1][col + 1] != 0) return;
    QPixmap prePiecePixmap;
    if (isBlackOnChess)
        prePiecePixmap.load(":/pic/blackPiecePredrop.png");
    else
        prePiecePixmap.load(":/pic/whitePiecePredrop.png");
    painter.drawPixmap(chessX, chessY, 27, 27, prePiecePixmap);
}
void ChessBoard::mousePressEvent(QMouseEvent *event) {
    if (cursorX > 670 || cursorY > 670 || cursorX < 41 || cursorY < 37) return;
    // int row = (cursorY - 37) / 43;
    // int col = (cursorX - 41) / 43;
    // int pieceY, pieceX;
    // if (row + 1 == 15) pieceY = 637;
    // if (col + 1 == 15) pieceX = 641;
    // if (row != 15 && col != 15) {
    //     pieceY = row * 43 + 37;
    //     pieceX = col * 43 + 41;
    // }
    if (pieces[row + 1][col + 1] != 0) return;
    QPixmap piecePixmap;
    pieceDrop thisPieceDrop;
    thisPieceDrop.row = row;
    thisPieceDrop.col = col;
    if (isBlackOnChess) {
        piecePixmap.load(":/pic/blackPiece.png");
        pieces[row + 1][col + 1] = 1;
        thisPieceDrop.isBlack = true;
        ui->chessRecord->append(QString("黑方落子：%1%2").arg(colDisplay).arg(rowDisplay));
    } else {
        piecePixmap.load(":/pic/whitePiece.png");
        pieces[row + 1][col + 1] = -1;
        thisPieceDrop.isBlack = false;
        ui->chessRecord->append(QString("白方落子：%1%2").arg(colDisplay).arg(rowDisplay));
    }
    pieces[row + 1][0] += 1;
    pieces[0][col + 1] += 1;
    pieces[0][0] += 1;
    pieceDrops.push_back(thisPieceDrop);

    piecesImg = imgMerge(piecesImg, piecePixmap, chessX, chessY);
    update();
    isBlackOnChess = !isBlackOnChess;
}
QPixmap ChessBoard::imgMerge(QPixmap basePiecesImg, QPixmap newPieceImg, int x, int y) {
    QPainter painter(&basePiecesImg);
    painter.drawPixmap(x, y, 27, 27, newPieceImg);
    return basePiecesImg;
}
// 光标符合的位置是114 454和157 497，移动距离为43 43(废弃，不用光标)
// 棋子的符合位置有213 510，移动距离 43 43
void ChessBoard::save() {}
void ChessBoard::load() {}
void ChessBoard::dropPiece() {}

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
