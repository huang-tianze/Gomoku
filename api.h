#ifndef API_H_
#define API_H_
// #include "define.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utility>

enum class API_MODE {
    analyze_opponent_move = 1,
    analyze_overall_situation = 2,
    get_strategy_recommendation = 3
};

#include <cstdlib>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

// 设置你的 API Token

std::string api_token = std::getenv("api_token");
const std::string url = "https://api.deepseek.com/chat/completions";
const std::string model_name = "deepseek-chat";

// const std::string prompt = "Hi";

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    if (userdata == nullptr || ptr == nullptr) {
        return 0; // 返回0表示写入失败
    }

    try {
        std::string *response = static_cast<std::string *>(userdata);
        response->append(ptr, size * nmemb);
        return size * nmemb;
    } catch (...) {
        return 0; // 如果发生任何异常，返回0
    }
}

std::string convert_board2string(int piece[16][16], std::pair<int, int> pos,
                                 bool Black_White) {
    std::string ret = "1 代表黑棋, 2 代表白棋\n";
    for (int i = 1; i < 16; ++i) {
        for (int j = 1; j < 16; ++j) {
            ret += std::to_string(piece[i][j]) + " ";
        }
        ret += "\n";
    }
    ret += std::string("") + (Black_White ? "你是黑棋" : "你是白棋") + "\n";
    ret += "上一步对方落子的位置是";
    ret += [](std::pair<int, int> pos) -> std::string {
        std::string ret = "x: " + std::to_string(pos.first) +
                          " y: " + std::to_string(pos.second) +
                          " 索引从1开始\n";
        return ret;
    }(pos);
    return ret;
}

std::string call_api(std::string sys_prompt, std::string user_prompt) {
    // std::cout << "start" << std::endl;
    // 创建 JSON 请求体
    // NOLINTBEGIN
    nlohmann::json request_body = R"({
"messages": [
    {
        "content": "",
        "role": "system"
    },
    {
        "content": "",
        "role": "user"
    }
],
"model": "deepseek-chat",
"frequency_penalty": 0,
"max_tokens": 2048,
"presence_penalty": 0,
"response_format": {
    "type": "text"
},
"stop": null,
"stream": false,
"stream_options": null,
"temperature": 1,
"top_p": 1,
"tools": null,
"tool_choice": "none",
"logprobs": false,
"top_logprobs": null
})"_json;

    request_body["messages"][0]["content"] = sys_prompt;
    request_body["messages"][1]["content"] = user_prompt;
    request_body["model"] = model_name;

    // std::cout << "curl" << std::endl;
    // NOLINTEND
    CURL *curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = nullptr;

        // 设置请求头

        const std::string Auth = "Authorization: Bearer " + api_token;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, Auth.c_str());

        std::string response_data;

        // 设置回调函数接收响应
        // auto write_callback = [](char *ptr, size_t size, size_t nmemb,
        //                          void *userdata) -> size_t {
        //     std::string *response = static_cast<std::string *>(userdata);
        //     response->append(ptr, size * nmemb);
        //     return size * nmemb;
        // };

        // 配置 curl 请求
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string request_body_dump = request_body.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body_dump.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // std::cout << "Request body: " << request_body_dump << std::endl;

        // 执行请求
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // std::cout << "Response:\n" << response_data << std::endl;
        } else {
            std::cerr << "Curl request failed: " << curl_easy_strerror(res)
                      << std::endl;
        }

        // 清理资源
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return response_data;
    } else {
        std::cerr << "Failed to initialize curl" << std::endl;
        return std::string("");
    }

    // return 0;
}

std::string call_deepseek(int pieces[16][16], std::pair<int, int> pos,
                          bool Black_White, API_MODE api_mode) {
    auto boardStr = convert_board2string(pieces, pos, Black_White);
    std::string sys_prompt, user_prompt;
    switch (api_mode) {
    case API_MODE::analyze_opponent_move:

        sys_prompt =
            "你是五子棋世界冠军，请全面分析当前棋局的整体"
            "局势，评估双方优劣势和关键控制点。你的返回的文本应该使用UTF-8编码";
        user_prompt = "当前棋盘状态(15 * 15):\n" + boardStr + R"(
分析要求：
1. 分析对手这一步的战略意图和战术目的
2. 识别对手形成的潜在威胁（活三、冲四、活四等）
3. 评估对手的整体布局策略（如：进攻方向、控制区域）
4. 预测对手下一步可能的进攻路线
5. 指出对手需要特别注意的防守区域

请用以下格式回答：
### 战略意图分析
[内容]
### 威胁评估
[内容]
### 布局策略
[内容]
### 下一步预测
[内容]
### 防守重点
[内容]"""
        )";
        break;

    case API_MODE::analyze_overall_situation:
        sys_prompt = "你是五子棋世界冠军，请全面分析当前棋局的整体局势，评估双"
                     "方优劣势和关键控制点。你的返回的文本应该使用UTF-8编码";
        user_prompt = "当前棋盘状态(15 * 15):\n" + boardStr + R"(
分析要求：
1. 评估当前棋局阶段（开局、中局、残局）
2. 分析双方棋子的分布和势力范围
3. 识别棋盘上的关键战略要地
4. 评估双方的优势和劣势（如：连接性、空间控制、进攻能力）
5. 预测未来5步内可能的发展趋势
6. 给出当前局势评分（黑棋优势/白棋优势/均势）

请用以下格式回答：
### 棋局阶段
[内容]
### 势力分析
[内容]
### 战略要地
[内容]
### 优劣势评估
[内容]
### 发展趋势
[内容]
### 局势评分
[内容]""")";
        break;
    case API_MODE::get_strategy_recommendation:
        sys_prompt = "你是五子棋顶级AI，请为棋手提供最佳策略和具体的落子位置建"
                     "议。你的返回的文本应该使用UTF-8编码";
        user_prompt = "当前棋盘状态(15 * 15):\n" + boardStr + R"(
任务：
1. 分析你代表的棋手面临的即时威胁和机会
2. 提出2-3种可行的应对策略（进攻、防守或平衡）
3. 推荐3个最佳落子位置（按优先级排序），格式必须为：列字母+行数字（如：h8）
4. 为每个推荐位置详细解释理由（防守价值、进攻潜力、战略重要性）
5. 评估每个位置的风险/收益比

要求：
- 落子位置必须在棋盘的空位上
- 优先考虑既能防守又能进攻的关键点
- 考虑短期战术和长期战略的平衡

请用以下格式回答：
### 威胁与机会分析
[内容]
### 应对策略
1. [策略1]
2. [策略2]
3. [策略3]
### 落子位置推荐
1. 位置: [坐标]
   理由: [详细解释]
   风险/收益: [评估]
2. 位置: [坐标]
   理由: [详细解释]
   风险/收益: [评估]
3. 位置: [坐标]
   理由: [详细解释]
   风险/收益: [评估]
### 最终建议
[综合建议]"""
        )";
    }
    std::string ret;
    try {
        nlohmann::json parsed_json =
            nlohmann::json::parse(call_api(sys_prompt, user_prompt));
        ret = parsed_json["choices"][0]["message"]["content"];
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    return ret;
}

// int pieces[16][16];

// signed main() {
//     auto api_mode = API_MODE::get_strategy_recommendation;
//     pieces[8][8] = 1;
//     std::cout << call_deepseek(pieces, {8, 8}, false, api_mode);
//     return 0;
// }

#endif
