#include "robot.h"

#define EXPAND_RADIUS 1
int botPiece;
int board[16][16];
int picked[16][16];
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_7 = {
    {{-1, 0, 1, 1, 1, 0, -1}, 100}}; // 眠3
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_6 = {
    {{0, 1, 1, 1, 1, 0}, 9000},  // 活4
    {{0, 1, 1, 1, 1, -1}, 1000}, // 冲4
    {{0, 0, 1, 1, 1, -1}, 100},  // 眠3
    {{0, 1, 0, 1, 1, -1}, 100},
    {{0, 1, 1, 0, 1, -1}, 100},
    {{0, 0, 1, 1, 0, 0}, 10},   // 活2
    {{0, 1, 0, 1, 1, 0}, 100},  // 活3
    {{0, 0, 0, 1, 1, -1}, 100}, // 眠2
    {{0, 0, 1, 0, 1, -1}, 100},
    {{0, 1, 0, 0, 1, -1}, 100}};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_5 = {
    {{1, 1, 1, 1, 1}, 10000}, // 连5
    {{1, 0, 1, 1, 1}, 800},   // 冲4
    {{1, 1, 0, 1, 1}, 800},
    {{0, 1, 1, 1, 0}, 800}, // 活3
    {{1, 0, 0, 1, 1}, 100}, // 眠3
    {{1, 0, 1, 0, 1}, 100},
    {{0, 1, 0, 1, 0}, 10}, // 活2
    {{1, 0, 0, 0, 1}, 10}  // 眠2
};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_4 = {
    {{1, 0, 0, 1}, 10}, // 活2
};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_3 = {};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_2 = {};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_1 = {};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_0 = {};
const std::vector<std::pair<std::vector<int>, int>> PATTERNS_BLANK = {};
const std::vector<std::vector<std::pair<std::vector<int>, int>>> patterns = {
    PATTERNS_0, PATTERNS_1, PATTERNS_2, PATTERNS_3, PATTERNS_4, PATTERNS_5, PATTERNS_6, PATTERNS_7};

const int DIRECTION[4][2] = {
    {0, 1}, // 水平
    {1, 0}, // 垂直
    {1, 1}, // 主对角线
    {1, -1} // 副对角线
};

std::vector<coord> getValidDrops(borderPos border) {
    // qDebug() << "getValidDrops";
    std::vector<coord> drops;
    for (int i = border.pos1.first; i <= border.pos2.first; i++) {
        for (int j = border.pos1.second; j <= border.pos2.second; j++) {
            if (board[i][j] == 0) {
                drops.push_back({i, j});
                // qDebug() << i << "," << j;
            }
            //{i,j}语法会自动推断类型，可以是pair或者含2个int的结构体
        }
    }
    return drops;
}

std::vector<coord> getSubboard(borderPos border) {
    // qDebug() << "getSubboard";
    std::vector<coord> pieces;
    for (int i = border.pos1.first; i <= border.pos2.first; i++) {
        for (int j = border.pos1.second; j <= border.pos2.second; j++) {
            pieces.push_back({i, j});
        }
    }
    return pieces;
}
int evaluate(borderPos border) {
    // qDebug() << "evaluate";
    int score = 0;
    int scoreInv = 0;
    memset(picked, 0, sizeof(picked));
    for (auto pos : getSubboard(border)) {
        std::pair<int, int> tmp = patternMatch(pos);
        score += tmp.first;
        scoreInv += tmp.second;
    }
    // qDebug() << "score: " << score;
    // qDebug() << "scoreInv: " << scoreInv;
    return score - scoreInv;
}

std::pair<int, int> patternMatch(coord pos) {
    // qDebug() << "match";
    int score = 0;
    int scoreInv = 0;
    int x = pos.first;
    int y = pos.second;
    for (auto direction : DIRECTION) {
        int dx = direction[0];
        int dy = direction[1];
        std::vector<int> pattern;
        // 计算score
        for (int i = 0; i < 7; i++) {
            int newX = x + i * dx;
            int newY = y + i * dy;
            if (newX < 1 || newY < 1 || newX > 15 || newY > 15)
                pattern.push_back(3); // 3作为一个空格占位，后面再对于具体分的时候将其变换成需要的数
            else
                pattern.push_back(board[newX][newY]);
            if (i >= 3) {
                for (auto pattern_score : patterns[i + 1]) {
                    // if(botPiece == 1) {
                    std::vector<int> patternProcessed = processBlank(pattern, -botPiece);
                    if (pattern_score.first == patternProcessed) score += pattern_score.second;
                    patternProcessed = processBlank(pattern, botPiece);
                    inverseVector(patternProcessed);
                    if (pattern_score.first == patternProcessed) scoreInv += pattern_score.second;
                    // }
                }
            }
        }
    }
    std::pair<int, int> scorePair = {score, scoreInv};
    return scorePair;
}
std::vector<int> processBlank(std::vector<int> vct, int tgtNum) {
    for (auto &num : vct) {
        if (num == 3) num = tgtNum;
    }
    return vct;
}

void inverseVector(std::vector<int> &vct) {
    for (auto &num : vct) num = -num;
}

// alpha和max节点的部分都是自方决策， beta部分和min节点相当于对方决策
// alpha是根据已搜索的节点，当前深度下我方能保证的我方至少得到的分数，beta是对方能保证我方最多能得到的分数
// minimax算法只考虑了一个根节点和一层子节点，对于一个根节点和其子节点，max根节点会选则子节点里最大的，min节点会选择子节点里最小的 (换言之，minimax算法起作用至少需要2步决策)
// 而alpha-beta剪枝则考虑了一个根节点，一层枝节点和一层枝节点的子节点(也就是至少3步决策才起作用)，每评估完一个节点就更新一次该节点的父节点的取值区间，类似的，评估完子节点后得到父节点的值，就可以更新父节点的父节点，这就是alpha-beta剪枝算法一直做的事情
// 假如说根节点是max节点（也就是己方决策），那么枝节点就是min节点，假定只有3层的话，枝节点的子节点就是叶节点
// 首先对于枝节点和叶节点，枝节点会从其子叶节点里选最小的，也就是说，每评估完一个叶节点，都会尝试更新枝节点的取值上界，表明即使再对剩下的叶节点评估，枝节点的取值不会再大于这个上界了，利用最终取值val满足val <= score_i 更新val的取值区间，遍历完叶节点就得到了val，这个值就是beta，表明这个枝节点的决策得到的我方优势的最小值
// 当遍历完一个枝节点的子节点，那么就能得到枝节点的值，同样的，这个时候可以更新该枝节点的父节点的取值区间，而父节点是max节点，那么更新取值的效果就是父节点的值不会小于这个枝节点的值，后面再遍历枝节点，也不会选择更小的，只会选更大的，这个下界就是alpha
// 而剪枝的情况就是，当评估完某个叶节点得到score，得到某个min枝节点的上界，发现这个上界比比先前更新的根节点的下界低，说明这个min枝节点的决策可以让上一层的max节点的分数达到更低，但是我的max节点在上面，比你的min节点更先决策，那么我肯定不会在枝节点里选择这个决策，因为我先前评估的枝节点能得到的score比这个枝节点更高，于是这个枝节点就被舍弃了，也即剪枝。
// 总得来说，minimax算法的核心在于"交替评估，评估一个节点就是评估子节点"，节点是我决策就往子节点里选评分最大(对我最有利的)的，节点是你决策，你就会在子节点里选评分最小的(对我最不利的)，使用的数学原理是 minScore = min(score_i)，maxScore = max(score_i)
// 而alpha-beta剪枝的核心在于"评估更新取值区间"和"先决策的一方优于后决策的一方"，评估完一个节点就可以更新这个节点的父节点的取值区间，父节点又可以更新子节点的取值范围，不在范围内的就不是最优决策，因为这个更新方式基于对父节点对应的决策方最有利的决策；而如果出现范围矛盾，也就是以根节点的决策更新得到的下界和以枝节点的决策更新得到的上界矛盾，说明两者的决策不能同时满足，那么此时就会剪枝，以根节点的决策为准，枝节点的决策不考虑，因为根节点更先决策。更新的数学原理是 minScore <= score_i, maxScore >= score_i
int minimax_alphabeta(int depth, bool isMaxnode, int alpha, int beta, borderPos border) {
    // qDebug() << "depth: " << depth;
    if (depth == 0) return evaluate(border);
    if (isMaxnode) {
        // 相当于当前博弈树节点是max节点，由自己决策，for循环遍历该节点情况下的所有情况
        int maxScore = -std::numeric_limits<int>::max();
        for (auto drop : getValidDrops(border)) {
            dropChess(drop, botPiece);
            int score = minimax_alphabeta(depth - 1, false, alpha, beta, updateBorder(drop, border)); // 递归相当于假定已经得到了max节点的所有子节点的评估分
            undoDrop(drop);
            // 寻找max节点的子节点里的最大值，更新maxScore和alpha (两者基本上等价)
            maxScore = std::max(maxScore, score);
            alpha = std::max(alpha, score);
            if (beta <= alpha) {
                break;
                qDebug() << "剪枝";
            } // 不满足alpha < beta就break
        }
        return maxScore;
    } else {
        int minScore = std::numeric_limits<int>::max();
        for (auto drop : getValidDrops(border)) {
            dropChess(drop, -botPiece);
            int score = minimax_alphabeta(depth - 1, true, alpha, beta, border);
            undoDrop(drop);
            minScore = std::min(minScore, score);
            beta = std::min(beta, score);
            if (beta <= alpha) {
                break;
                qDebug() << "剪枝";
            }
        }
        return minScore;
    }
}
coord invokeBot(int pieces[16][16], borderPos border, int depth, bool isPlayerFirst) {
    // debugOutput(pieces);
    // border = expandBorder(border, 1);
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            board[i][j] = pieces[i][j];
        }
    }
    if (isPlayerFirst)
        botPiece = -1;
    else
        botPiece = 1;

    int bestScore = -std::numeric_limits<int>::max();
    coord bestDrop;
    for (auto drop : getValidDrops(border)) {
        dropChess(drop, botPiece);
        int dropScore = minimax_alphabeta(depth - 1, false, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), border);
        undoDrop(drop);
        if (dropScore > bestScore) {
            bestScore = dropScore;
            bestDrop = drop;
        }
    }
    return bestDrop;
}

void dropChess(coord chessPos, int chessInt) {
    board[chessPos.first][chessPos.second] = chessInt;
}
void undoDrop(coord chessPos) {
    board[chessPos.first][chessPos.second] = 0;
}

borderPos updateBorder(int dropRow, int dropCol, borderPos border) {
    if (border.pos1.second >= dropCol) border.pos1.second = std::max(dropCol - EXPAND_RADIUS, 1);
    if (border.pos1.first >= dropRow) border.pos1.first = std::max(dropRow - EXPAND_RADIUS, 1);
    if (border.pos2.second <= dropCol) border.pos2.second = std::min(dropCol + EXPAND_RADIUS, 15);
    if (border.pos2.first <= dropRow) border.pos2.first = std::min(dropRow + EXPAND_RADIUS, 15);
    return border;
}
borderPos updateBorder(coord dropPos, borderPos border) {
    return updateBorder(dropPos.first, dropPos.second, border);
}
borderPos expandBorder(borderPos border, int expandRadius) {
    border.pos1.first = std::max(border.pos1.first - expandRadius, 1);
    border.pos1.second = std::max(border.pos1.second - expandRadius, 1);
    border.pos2.first = std::min(border.pos2.first + expandRadius, 15);
    border.pos2.second = std::min(border.pos2.second + expandRadius, 15);
    return border;
}
void debugOutput(int arr[16][16]) {
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            qDebug() << arr[i][j] << ' ';
        }
        qDebug() << '\n';
    }
}
int main() {
    return 0;
}