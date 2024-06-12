//
// Created by root on 2/29/24.
//

#ifndef CLINK_UTILS_H
#define CLINK_UTILS_H




#include <string>

class utils {
public:
    static bool isHost(const char* host);
    static char* highlightKeyword(const char* str, const char* keyword);
};


#endif //CLINK_UTILS_H
