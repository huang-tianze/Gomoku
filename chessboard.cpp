#include "chessboard.h"
#include "api.h"
#include "config.h"
#include "mainwindow.h"
#include "robot.h"
#include "ui_chessboard.h"
#include <qdir.h>

ChessBoard::ChessBoard(QWidget *parent, bool _isPVE)
    : QWidget(parent), ui(new Ui::ChessBoard), isPVE(_isPVE) {
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
    // ui->centralWidget->setMouseTracking(true);
    // 由于解决了ChessBoard背景的问题，替代品的centralWidget就删除了，这样也不需要chessBoard和centralWidget同时设置track了
    QTimer::singleShot(10, this,
                       [this]() { this->setCursor(Qt::ArrowCursor); });
    // 不知道什么原因，打开窗口的时候光标会变成拉伸光标，这里做个10ms的延时改光标纠正一下
    if (_isPVE) {
        this->ui->backButton->setText("LLM解析");
    }
}

ChessBoard::~ChessBoard() { delete ui; }

void ChessBoard::closeEvent(QCloseEvent *event) {
    if (isAutoSaveOn)
        save();
    emit reshowRequested();
    QWidget::closeEvent(event);
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    if (isEnded || botThinking)
        return;
    calMousePos(event);
    if (!cursorInBoard)
        chessStatusBar->showMessage("Ciallo～(∠・ω< )⌒☆");
    else
        chessStatusBar->showMessage(
            QString("%1%2").arg(cursorColDisplay).arg(cursorRowDisplay));
    update(); // 让Qt更新部件，会在事件队列里加入一个QPaintEvent
}

// 棋子合格位置 :600 596, 移动距离43
// 光标符合的位置是114 454和157 497，移动距离为43 43(废弃，不用光标)
// 棋子的符合位置有213 510，移动距离 43 43
void ChessBoard::paintEvent(QPaintEvent *event) {
    // predrop的棋子显示
    QPainter painter(this);
    painter.drawPixmap(0, 0, 1034, 738, piecesImg); // 已有棋子绘制
    // std::cout << cursorCol << ' ' << cursorRow << std::endl;
    if (!cursorInBoard || pieces[cursorRow][cursorCol] != 0)
        return;
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap prePiecePixmap;
    if (isBlackOnChess)
        prePiecePixmap.load(":/pic/blackPiecePredrop.png");
    else
        prePiecePixmap.load(":/pic/whitePiecePredrop.png");
    painter.drawPixmap(cursorChessX, cursorChessY, 27, 27, prePiecePixmap);
}

QPixmap imgMerge(QPixmap basePiecesImg, QPixmap newPieceImg, int x, int y) {
    QPainter painter(&basePiecesImg);
    painter.drawPixmap(x, y, 27, 27, newPieceImg);
    return basePiecesImg;
}

void ChessBoard::mousePressEvent(QMouseEvent *event) {
    if (botThinking || isEnded || !cursorInBoard ||
        pieces[cursorRow][cursorCol] != 0)
        return;
    calMousePos(event);
    drop(cursorRow, cursorCol);
}

void ChessBoard::calMousePos(QMouseEvent *event) {
    QPoint pos = event->pos();
    cursorX = pos.x();
    cursorY = pos.y();
    if (cursorX > 670 || cursorY > 670 || cursorX < 41 || cursorY < 37)
        cursorInBoard = false;
    else
        cursorInBoard = true;

    cursorRow = (cursorY - 37) / 43 + 1;
    cursorCol = (cursorX - 41) / 43 + 1;
    cursorRowDisplay = 15 - (cursorRow - 1);
    cursorColDisplay = cursorCol - 1 + 'A';
    std::tie(cursorChessX, cursorChessY) =
        chessPosCalculate(cursorRow, cursorCol);
}

coord chessPosCalculate(const int &row, const int &col) {
    coord chessPos;
    if (row == 15)
        chessPos.second = 637;
    else
        chessPos.second = (row - 1) * 43 + 37;
    if (col == 15)
        chessPos.first = 641;
    else
        chessPos.first = (col - 1) * 43 + 41;
    return chessPos;
}

void ChessBoard::drop(const int &row, const int &col) {

    dropRow = row;
    dropCol = col;
    updateBorder();
    std::tie(dropChessX, dropChessY) = chessPosCalculate(dropRow, dropCol);
    dropRowDisplay = 15 - (dropRow - 1);
    dropColDisplay = dropCol - 1 + 'A';
    // 悔棋相关数据存储
    lastDropCol = dropCol;
    lastDropRow = dropRow;
    lastDropRowDisplay = dropRowDisplay;
    lastDropColDisplay = dropColDisplay;
    isCurBack = false;
    // 对局过程数据存储
    QPixmap piecePixmap;
    pieceDrop thisPieceDrop;
    thisPieceDrop.row = dropRow;
    thisPieceDrop.col = dropCol;
    // 落子..
    if (isBlackOnChess) {
        piecePixmap.load(":/pic/blackPiece.png");
        pieces[dropRow][dropCol] = 1;
        thisPieceDrop.isBlack = true;
        ui->chessRecord->append(QString("第%1手，黑方落子：%2%3")
                                    .arg(round)
                                    .arg(dropColDisplay)
                                    .arg(dropRowDisplay));
    } else {
        piecePixmap.load(":/pic/whitePiece.png");
        pieces[row][col] = 2; // 1黑棋，2白棋
        thisPieceDrop.isBlack = false;
        ui->chessRecord->append(QString("第%1手，白方落子：%2%3")
                                    .arg(round)
                                    .arg(dropColDisplay)
                                    .arg(dropRowDisplay));
    }
    lastPiecesImg = piecesImg;
    piecesImg = imgMerge(piecesImg, piecePixmap, dropChessX, dropChessY);
    update();
    thisPieceDrop.isEnd = winnerJudge();
    pieceDrops.push_back(thisPieceDrop);
    if (isEnded)
        save();
    isBlackOnChess = !isBlackOnChess;
    round++;
    if (!botThinking && isPVE)
        botDrop(row, col);
}

void ChessBoard::updateBorder() {
    if (round == 1) {
        border.pos1.second = std::max(dropCol - 1, 1);
        border.pos1.first = std::max(dropRow - 1, 1);
        border.pos2.second = std::min(dropCol + 1, 15);
        border.pos2.first = std::min(dropRow + 1, 15);
        return;
    }
    if (border.pos1.second >= dropCol)
        border.pos1.second = std::max(dropCol - 1, 1);
    if (border.pos1.first >= dropRow)
        border.pos1.first = std::max(dropRow - 1, 1);
    if (border.pos2.second <= dropCol)
        border.pos2.second = std::min(dropCol + 1, 15);
    if (border.pos2.first <= dropRow)
        border.pos2.first = std::min(dropRow + 1, 15);
}

void ChessBoard::drop(const coord &rowCol) {
    drop(rowCol.first, rowCol.second);
}

bool ChessBoard::winnerJudge() {
    if (round == 226) {
        QMessageBox::information(this, "对局结束！", "棋谱已满，平局！");
        ui->chessRecord->append("平局");
    }
    int targetNum;
    int curRow = dropRow, curCol = dropCol;
    if (isBlackOnChess)
        targetNum = 1;
    else
        targetNum = 2;
    int verticalCount = 0;
    int horizontalCount = 0;
    int mainDiagonalCount = 0;
    int counterDiagonalCount = 0;
    for (int i = curCol - 1; pieces[curRow][i] == targetNum && i >= 1; i--)
        horizontalCount++;
    for (int i = curCol + 1; pieces[curRow][i] == targetNum && i <= 15; i++)
        horizontalCount++;
    for (int i = curRow - 1; pieces[i][curCol] == targetNum && i >= 1; i--)
        verticalCount++;
    for (int i = curRow + 1; pieces[i][curCol] == targetNum && i <= 15; i++)
        verticalCount++;
    for (int i = 1; pieces[curRow - i][curCol - i] == targetNum && i <= 4 &&
                    curRow - i >= 1 && curCol - i >= 1;
         i++)
        mainDiagonalCount++;
    for (int i = 1; pieces[curRow + i][curCol + i] == targetNum && i <= 4 &&
                    curRow + i <= 15 && curCol + i <= 15;
         i++)
        mainDiagonalCount++;
    for (int i = 1; pieces[curRow + i][curCol - i] == targetNum && i <= 4 &&
                    curRow + i <= 15 && curCol - i >= 1;
         i++)
        counterDiagonalCount++;
    for (int i = 1; pieces[curRow - i][curCol + i] == targetNum && i <= 4 &&
                    curRow - i >= 1 && curCol + i <= 15;
         i++)
        counterDiagonalCount++;
    if (verticalCount >= 4 || horizontalCount >= 4 || mainDiagonalCount >= 4 ||
        counterDiagonalCount >= 4) {
        if (isBlackOnChess) {
            QMessageBox::information(this, "对局结束！", "黑方胜利！");
            ui->chessRecord->append("黑方胜利！");
        } else {
            QMessageBox::information(this, "对局结束！", "白方胜利！");
            ui->chessRecord->append("白方胜利！");
        }
        isEnded = true; // 如果前面加上声明，bool isEnded = true;
                        // 那么这个变量就是新的变量，而不是类中的成员变量.
        return true;
    }
    return false;
}

void ChessBoard::save() {
    if (isSaved || pieceDrops.size() < 3)
        return;
    // if (pieceDrops.size() < 3) return;
    long long nowTime = time(NULL);
    QString fileName =
        QString::fromStdString(savePath) + QString::number(nowTime) + ".dat";
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
        QMessageBox::StandardButton reply =
            QMessageBox::question(this, "警告",
                                  "你未开启自动保存对局，如果不点击保存对局，本"
                                  "局的进度将会消失，你是否决定退出？",
                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    this->close();
    emit reshowRequested();
}

void ChessBoard::on_backButton_clicked() {
    if (round == 1)
        return;
    if (isPVE) {
        std::string message =
            call_deepseek(pieces, {dropRow, dropCol}, !isPlayerFirst,
                          API_MODE::analyze_opponent_move);
        QMessageBox::information(this, "LLM解析",
                                 QString::fromStdString(message));
        return;
    }
    // 由于不想增加QPixmap的存储了，只做一步悔棋.
    // 加个QPixmap的vector存一下可以实现多步悔棋，但多步悔棋不讲武德x
    if (isCurBack) {
        QMessageBox::information(this, "悔棋", "不能连续悔棋！");
        return;
    }
    if (isEnded)
        isEnded = !isEnded;
    isBlackOnChess = !isBlackOnChess;
    isCurBack = true;
    round--;
    pieces[lastDropRow][lastDropCol] = 0;
    pieceDrops.pop_back();
    if (isBlackOnChess) {
        ui->chessRecord->append(QString("悔棋：黑方撤回了在%1%2的一步")
                                    .arg(lastDropColDisplay)
                                    .arg(lastDropRowDisplay));
    } else {
        ui->chessRecord->append(QString("悔棋：白方撤回了在%1%2的一步")
                                    .arg(lastDropColDisplay)
                                    .arg(lastDropRowDisplay));
    }
    piecesImg = lastPiecesImg;
    update();
}

void ChessBoard::on_saveButton_clicked() { save(); }

void ChessBoard::botInit() {
    botThinking = true;
    if (isPVE && !isPlayerFirst)
        drop(8, 8);
    botThinking = false;
}

void ChessBoard::botDrop(const int &row, const int &col) {
    if (isEnded)
        return;
    botThinking = true;

    coord tmp = invokeBot(pieces, {row, col}, isPlayerFirst);
    // qDebug() << "border: " << border.pos1 << border.pos2;
    // qDebug() << tmp;
    drop(tmp);
    botThinking = false;
}
