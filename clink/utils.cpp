//
// Created by root on 2/29/24.
//

#include "utils.h"
#include <boost/regex.hpp>
#include <iostream>

//设置字体颜色
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_BLUE "\033[1;36m"
#define STYLE_OFF "\033[0m"
/*
 * 功能：判断一个字符串是否符合ip定义
 * 输入：字符串的地址
 * 输出：是或否
 * */
bool utils::isHost(const char* host) {
    //使用C11添加的R特性来引入生字符，也就是不需要转义
    //判断一个字符串是否符合ip的定义
    boost::regex pattern("^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$");
    return boost::regex_match(host, pattern);
}



char* utils::highlightKeyword(const char* str, const char* keyword) {

    int keywordLen = strlen(keyword);

    std::vector<char> result;


    const char* ptr = nullptr;
    std::string green(COLOR_GREEN);
    std::string styleOff(STYLE_OFF);

    while ((ptr = strstr(str, keyword)) != NULL) {

        std::copy(str, ptr, std::back_inserter(result));
        std::copy(green.begin(), green.end(), std::back_inserter(result));
        std::copy(ptr, ptr + keywordLen, std::back_inserter(result));
        std::copy(styleOff.begin(), styleOff.end(), std::back_inserter(result));

        str = ptr + keywordLen;
    }

    std::copy(str, str + strlen(str), std::back_inserter(result));
    size_t totalSize = result.size();
    char* charArray = new char[totalSize];
    std::copy(result.begin(), result.end(), charArray);
    charArray[totalSize-1] = '\0';

    return charArray;
}


