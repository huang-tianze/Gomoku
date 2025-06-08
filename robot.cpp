#include "config.h"
#include "define.h"
#include <iostream>
#include <vector>

#define DEBUG_LOG 1
#if DEBUG_LOG
#define LOG(x) qDebug() << (x)
#else
#define LOG(x)
#endif

#include "robot.h"

#define EXPAND_RADIUS 1
#define LINE_LENGTH 4
#undef slots
#include <torch/csrc/jit/serialization/import.h>
#include <torch/torch.h>

struct Coor {
    int ret;
    int x;
    int y;
    float p;
};

// const std::string modelPath = "../models/best_checkpoint.pt";

Coor get_action(int board_[3][15][15]) {
    try {
        torch::Device device(torch::kCUDA);
        if (!torch::cuda::is_available()) {
            std::cout << "CUDA 不可用，使用 CPU" << std::endl;
            device = torch::Device(torch::kCPU);
        }

        torch::jit::script::Module module = torch::jit::load(modelPath, device);
        module.to(device);
        module.eval();

        torch::Tensor board = torch::zeros({1, 3, 15, 15}, torch::kInt32);
        for (int i1 = 0; i1 < 3; ++i1) {
            for (int i2 = 0; i2 < 15; ++i2) {
                for (int i3 = 0; i3 < 15; ++i3) {
                    board[0][i1][i2][i3] = board_[i1][i2][i3];
                }
            }
        }
        // board[0] = board_;

        board = board.to(torch::kFloat32).to(device);

        auto output = module.forward({board});
        auto p = output.toTuple()->elements()[0].toTensor();
        auto v = output.toTuple()->elements()[1].toTensor().item<float>();
        auto maxIdxList = std::get<1>(p.slice(1, 0, 255).sort(1, true));
        int max_idx = 0;
        for (int i = 0; i < maxIdxList.size(1); ++i) {
            auto j = maxIdxList[0][i].item<int>();
            if (board_[0][j / 15][j % 15] == 0 &&
                board_[1][j / 15][j % 15] == 0) {
                max_idx = j;
                if (false) {
                    std::cout << maxIdxList << std::endl;
                    std::cout << j << std::endl;

                    [](int pcc[3][15][15]) {
                        for (int i = 0; i < 3; i++) {
                            for (int j = 0; j < 15; j++) {
                                for (int k = 0; k < 15; k++) {
                                    std::cout << pcc[i][j][k] << ' ';
                                }
                                std::cout << std::endl;
                            }
                            std::cout << std::endl;
                        }
                    }(board_);
                }
                break;
            }
        }
        return {0, max_idx / 15, max_idx % 15, v};

    } catch (const c10::Error &e) {
        std::cerr << "加载模型失败: " << e.what() << std::endl;
        return {-1, -1, -1, -1};
    }
}

// signed main() {
//     torch::Tensor board_ = torch::zeros({3, 15, 15});

//     auto out = get_action(board_);
//     std::cout << '{' << out.x << ',' << out.y << ',' << out.p << '}'
//               << std::endl;
// }

coord invokeBot(int piece[16][16], coord pos, bool isPlayerFirst) {
    int board[3][15][15];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 15; ++j) {
            for (int k = 0; k < 15; k++) {
                board[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (piece[i + 1][j + 1] == 1) {
                board[isPlayerFirst][i][j] = 1;
            } else if (piece[i + 1][j + 1] == 2) {
                board[!isPlayerFirst][i][j] = 1;
            }
        }
    }
    board[2][pos.first - 1][pos.second - 1] = 1;
    auto ret = get_action(board);
    return {ret.x + 1, ret.y + 1};
}
