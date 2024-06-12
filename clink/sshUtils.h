//
// Created by root on 2/28/24.
//

#ifndef CLINK_SSHUTILS_H
#define CLINK_SSHUTILS_H
#include <libssh/libssh.h>

namespace sshUtils {

    class sshUtils {
    public:
        static void ssh_execute_cmd(const char* username, const char* host, const char* cmd, const char* color);
        static int checkAutoPubKey(const char* username, const char* host);
        static int loginHost(const char* host, const char* username);
        static void exec_batch(const char* username, const char* host, const char* cmd, const char* color);
        static void exec_single(const char* username, const char* host, const char* cmd, const char* color);
    };

} // ssh_utils

#endif //CLINK_SSHUTILS_H
