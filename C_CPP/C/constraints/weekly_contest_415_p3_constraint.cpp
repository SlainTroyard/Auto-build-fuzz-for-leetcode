#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

extern "C" {
    // TODO: Add your function declaration here
    // Example: bool solutionFunction(type1 param1, type2 param2);
    int minValidStrings(char** words, int wordsSize, const char* target);
}

// 输入验证函数
bool isValidInput(const uint8_t* data, size_t size, std::vector<std::string>& words, std::string& target) {
    size_t pos = 0;

    // 检查至少有1个字节用于表示单词数量
    if (size < 1) return false;

    uint8_t num_words = data[pos];
    pos += 1;

    if (num_words < 1 || num_words > 100) return false;

    words.clear();

    int total_length = 0;

    for (uint8_t i = 0; i < num_words; i++) {
        // 检查是否有足够的字节表示单词长度（2字节）
        if (pos + 2 > size) return false;
        uint16_t word_length = (data[pos] << 8) | data[pos + 1];
        pos += 2;

        if (word_length < 1 || word_length > 5000) return false;

        // 检查是否有足够的字节表示单词内容
        if (pos + word_length > size) return false;

        std::string word;
        for (uint16_t j = 0; j < word_length; j++) {
            char c = data[pos++];
            if (c < 'a' || c > 'z') return false;
            word += c;
        }
        words.push_back(word);
        total_length += word_length;
        if (total_length > 100000) return false;
    }

    // 解析目标字符串
    // 检查是否有足够的字节表示目标长度（2字节）
    if (pos + 2 > size) return false;
    uint16_t target_length = (data[pos] << 8) | data[pos + 1];
    pos += 2;

    if (target_length < 1 || target_length > 5000) return false;

    // 检查是否有足够的字节表示目标内容
    if (pos + target_length > size) return false;

    target.clear();
    for (uint16_t j = 0; j < target_length; j++) {
        char c = data[pos++];
        if (c < 'a' || c > 'z') return false;
        target += c;
    }

    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    std::vector<std::string> words;
    std::string target;
    if (!isValidInput(data, size, words, target)) return 0;
    
    // TODO: Transform fuzzer input into valid function parameters
    // 转换words为char**形式
    char** c_words = (char**)malloc(sizeof(char*) * words.size());
    if (!c_words) return 0; // 内存分配失败
    for (size_t i = 0; i < words.size(); i++) {
        c_words[i] = strdup(words[i].c_str());
        if (!c_words[i]) {
            // 释放已分配的内存
            for (size_t j = 0; j < i; j++) {
                free(c_words[j]);
            }
            free(c_words);
            return 0;
        }
    }

    // TODO: Execute the test function
    int result = minValidStrings(c_words, words.size(), target.c_str());
    
    // Save test case
    std::string outputDir = "fuzz_outputs/C/weekly_contest_415_p3/output/";
    // 创建输出目录（如果不存在）
    std::filesystem::create_directories(outputDir);
    std::ofstream outFile;
    outFile.open(outputDir + "test_cases.txt", std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "无法打开输出文件！" << std::endl;
        return 0;
    }
    outFile << "Input: " << "words = [";
    for (size_t i = 0; i < words.size(); i++) {
        outFile << "\"" << words[i] << "\"";
        if (i != words.size() - 1) outFile << ", ";
    }
    outFile << "], target = \"" << target << "\"" << std::endl;
    outFile << "Output: " << result << std::endl;
    outFile << "--------------------------" << std::endl;
    outFile.close();
    
    // 释放内存
    for (size_t i = 0; i < words.size(); i++) {
        free(c_words[i]);
    }
    free(c_words);

    return 0;
}
