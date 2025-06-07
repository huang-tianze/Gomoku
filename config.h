#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <string>

#if _WIN32
#ifdef _WIN64
const std::string modelPath = R"(..\..\..\models\best_checkpoint.pt)";
const std::string savePath = R"(.\)"
#endif
#elif __linux__
const std::string modelPath = R"(../models/best_checkpoint.pt)";
const std::string savePath = R"(./manual/)";
#else
assert<false>
#endif

#endif
