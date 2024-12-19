#include "chessboard.h"
#include "mainwindow.h"
#include "ui_chessboard.h"

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChessBoard) {
    ui->setupUi(this);
    // 棋盘状态置0
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            pieces[i][j] = 0;
        }
    }
    // 图标
    QIcon icon(":/pic/icon.svg");
    setWindowIcon(icon);
    // 状态栏
    chessStatusBar = new QStatusBar(this);
    ui->statusLayout->addWidget(chessStatusBar);
    chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    // qDebug() << "window size:" << this->size();
    // 背景 QWidget默认禁用了styledBackground，使用QSS绘制需要启用
    setAttribute(Qt::WA_StyledBackground, true);
    piecesImg.load(":/pic/transparent.png");
    lastPiecesImg.load(":/pic/transparent.png");
    // 光标
    this->setMouseTracking(true);
    // ui->centralWidget->setMouseTracking(true); 由于解决了ChessBoard背景的问题，替代品的centralWidget就删除了，这样也不需要chessBoard和centralWidget同时设置track了
    QTimer::singleShot(10, this, [this]() {
        this->setCursor(Qt::ArrowCursor);
    });
    // 不知道什么原因，打开窗口的时候光标会变成拉伸光标，这里做个10ms的延时改光标纠正一下
}

ChessBoard::~ChessBoard() {
    delete ui;
}

void ChessBoard::closeEvent(QCloseEvent *event) {
    if (isAutoSaveOn) save();
    emit reshowRequested();
    QWidget::closeEvent(event);
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    if (isEnded) return;
    calMousePos(event);
    if (!inBoard) {
        chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    } else {
        chessStatusBar->showMessage(QString("%1%2").arg(colDisplay).arg(rowDisplay));
    }
    update(); // 让Qt更新部件，会在事件队列里加入一个QPaintEvent
}

// 棋子合格位置 :600 596, 移动距离43
// 光标符合的位置是114 454和157 497，移动距离为43 43(废弃，不用光标)
// 棋子的符合位置有213 510，移动距离 43 43
void ChessBoard::paintEvent(QPaintEvent *event) {
    if (isEnded) return;
    // predrop的棋子显示
    QPainter painter(this);
    painter.drawPixmap(0, 0, 1034, 738, piecesImg); // 已有棋子绘制
    if (!inBoard) return;
    if (pieces[row + 1][col + 1] != 0) return;
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap prePiecePixmap;
    if (isBlackOnChess)
        prePiecePixmap.load(":/pic/blackPiecePredrop.png");
    else
        prePiecePixmap.load(":/pic/whitePiecePredrop.png");
    painter.drawPixmap(chessX, chessY, 27, 27, prePiecePixmap);
}

void ChessBoard::mousePressEvent(QMouseEvent *event) {
    if (isEnded) return;
    if (!inBoard) return;
    if (pieces[row + 1][col + 1] != 0) return;
    calMousePos(event);
    // 悔棋相关数据存储
    lastDropCol = col;
    lastDropRow = row;
    lastDropRowDisplay = rowDisplay;
    lastDropColDisplay = colDisplay;
    isCurBack = false;
    // 对局过程数据存储
    QPixmap piecePixmap;
    pieceDrop thisPieceDrop;
    thisPieceDrop.row = row;
    thisPieceDrop.col = col;
    // 落子..
    if (isBlackOnChess) {
        piecePixmap.load(":/pic/blackPiece.png");
        pieces[row + 1][col + 1] = 1;
        thisPieceDrop.isBlack = true;
        ui->chessRecord->append(QString("第%1手，黑方落子：%2%3").arg(round).arg(colDisplay).arg(rowDisplay));
    } else {
        piecePixmap.load(":/pic/whitePiece.png");
        pieces[row + 1][col + 1] = -1;
        thisPieceDrop.isBlack = false;
        ui->chessRecord->append(QString("第%1手，白方落子：%2%3").arg(round).arg(colDisplay).arg(rowDisplay));
    }
    pieces[row + 1][0] += 1;
    pieces[0][col + 1] += 1;
    pieces[0][0] += 1;
    lastPiecesImg = piecesImg;
    piecesImg = imgMerge(piecesImg, piecePixmap, chessX, chessY);
    update();
    thisPieceDrop.isEnd = winnerJudge();
    pieceDrops.push_back(thisPieceDrop);
    if (isEnded) save();
    isBlackOnChess = !isBlackOnChess;
    round++;
}

void ChessBoard::calMousePos(QMouseEvent *event) {
    QPoint pos = event->pos();
    cursorX = pos.x();
    cursorY = pos.y();
    if (cursorX > 670 || cursorY > 670 || cursorX < 41 || cursorY < 37)
        inBoard = false;
    else
        inBoard = true;
    row = (cursorY - 37) / 43;
    rowDisplay = 15 - row;
    col = (cursorX - 41) / 43;
    colDisplay = col + 'A';
    if (row + 1 == 15) chessY = 637;
    if (col + 1 == 15) chessX = 641;
    if (row != 15 && col != 15) {
        chessY = row * 43 + 37;
        chessX = col * 43 + 41;
    }
}

void ChessBoard::drop(int row, int col) { // 从1开始的row和col
}

QPixmap imgMerge(QPixmap basePiecesImg, QPixmap newPieceImg, int x, int y) {
    QPainter painter(&basePiecesImg);
    painter.drawPixmap(x, y, 27, 27, newPieceImg);
    return basePiecesImg;
}

bool ChessBoard::winnerJudge() {
    if (round == 226) {
        QMessageBox::information(this, "对局结束！", "棋谱已满，平局！");
        ui->chessRecord->append("平局");
    }
    int targetNum;
    int curRow = row + 1, curCol = col + 1; // 从1开始的行列
    if (isBlackOnChess)
        targetNum = 1;
    else
        targetNum = -1;
    int verticalCount = 0;
    int horizontalCount = 0;
    int mainDiagonalCount = 0;
    int counterDiagonalCount = 0;
    for (int i = curCol - 1; pieces[curRow][i] == targetNum && i >= 1; i--) horizontalCount++;
    for (int i = curCol + 1; pieces[curRow][i] == targetNum && i <= 15; i++) horizontalCount++;
    for (int i = curRow - 1; pieces[i][curCol] == targetNum && i >= 1; i--) verticalCount++;
    for (int i = curRow + 1; pieces[i][curCol] == targetNum && i <= 15; i++) verticalCount++;
    for (int i = 1; pieces[curRow - i][curCol - i] == targetNum && i <= 4 && curRow - i >= 1 && curCol - i >= 1; i++) mainDiagonalCount++;
    for (int i = 1; pieces[curRow + i][curCol + i] == targetNum && i <= 4 && curRow + i <= 15 && curCol + i <= 15; i++) mainDiagonalCount++;
    for (int i = 1; pieces[curRow + i][curCol - i] == targetNum && i <= 4 && curRow + i <= 15 && curCol - i >= 1; i++) counterDiagonalCount++;
    for (int i = 1; pieces[curRow - i][curCol + i] == targetNum && i <= 4 && curRow - i >= 1 && curCol + i <= 15; i++) counterDiagonalCount++;
    if (verticalCount >= 4 || horizontalCount >= 4 || mainDiagonalCount >= 4 || counterDiagonalCount >= 4) {
        if (isBlackOnChess) {
            QMessageBox::information(this, "对局结束！", "黑方胜利！");
            ui->chessRecord->append("黑方胜利！");
        } else {
            QMessageBox::information(this, "对局结束！", "白方胜利！");
            ui->chessRecord->append("白方胜利！");
        }
        isEnded = true; // 如果前面加上声明，bool isEnded = true; 那么这个变量就是新的变量，而不是类中的成员变量.
        return true;
    }
    return false;
}

void ChessBoard::save() {
    if (isSaved) return;
    if (pieceDrops.size() < 3) return;
    long long nowTime = time(NULL);
    QString fileName = "D:/Files/SDU/2024ProgramDesign/Project/2-Assignment/Gomoku/chessManual/" + QString::number(nowTime) + ".dat";
    std::ofstream outFile(fileName.toStdString(), std::ios::binary);
    if (!outFile) {
        qDebug() << "无法打开文件进行写入！"; // TODO: 改成QMessageBox
        return;
    }
    for (auto &drop : pieceDrops) {
        outFile.write(reinterpret_cast<char *>(&drop), sizeof(pieceDrop));
    }
    //
    outFile.close();
    qDebug() << "导出完成：" << fileName;
    isSaved = true;
}

void ChessBoard::on_homeButton_clicked() {
    if (isAutoSaveOn) {
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
    emit reshowRequested();
}

void ChessBoard::on_backButton_clicked() {
    if (round == 1) return;
    // 由于不想增加QPixmap的存储了，只做一步悔棋. 加个QPixmap的vector存一下可以实现多步悔棋，但多步悔棋不讲武德x
    if (isCurBack) {
        QMessageBox::information(this, "悔棋", "不能连续悔棋！");
        return;
    }
    if (isEnded) isEnded = !isEnded;
    isBlackOnChess = !isBlackOnChess;
    isCurBack = true;
    round--;
    pieces[lastDropRow + 1][lastDropCol + 1] = 0;
    pieceDrops.pop_back();
    if (isBlackOnChess) {
        ui->chessRecord->append(QString("悔棋：黑方撤回了在%1%2的一步").arg(lastDropColDisplay).arg(lastDropRowDisplay));
    } else {
        ui->chessRecord->append(QString("悔棋：白方撤回了在%1%2的一步").arg(lastDropColDisplay).arg(lastDropRowDisplay));
    }
    piecesImg = lastPiecesImg;
    update();
}
void ChessBoard::on_saveButton_clicked() {
    save();
}
