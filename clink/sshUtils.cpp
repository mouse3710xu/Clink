//
// Created by root on 2/28/24.
//

//设置黑体字
#define BOLD_ON "\033[1m"
//关闭字体样式
#define STYLE_OFF "\033[0m"

#include "sshUtils.h"
#include <iostream>
#include "Constants.h"
#include "utils.h"
#include <libssh/libssh.h>
#include <ncurses.h>
#include <fstream>
#include <sstream>
#include <cstring>

namespace sshUtils {
    void sshUtils::exec_batch(const char* username, const char* host, const char* cmd, const char* color){
        int listNum = 0;
        std::string remoteHost = "";
        std::cout << "> 正在查询 " << host << "..." << std::endl;
        std::ifstream inFile(host);
        std::string line;
        std::string behindStr;
        while(std::getline(inFile, line)){
            ++listNum;
            if(listNum%10 == 2){
                behindStr = "nd";
            }else if (listNum%10 == 3) {
                behindStr = "rd";
            }else{
                behindStr = "st";
            }
            std::istringstream iss(line);
            if(iss >> remoteHost){
                std::cout << BOLD_ON << "--->  \t正在输出 [" << remoteHost << "] 返回的结果  \t<--- \t[" << listNum << "] \n" << STYLE_OFF << std::endl;
                if(!utils::isHost(remoteHost.c_str())){
                    std::cout << Constants::MSG_NOT_VALID_HOST << std::endl;
                    continue;
                }
                sshUtils::sshUtils::ssh_execute_cmd(username, remoteHost.c_str(), cmd, color);
            }
        }
    }

    void sshUtils::exec_single(const char* username, const char* host, const char* cmd, const char* color){
        if(!utils::isHost(host)){
            std::cout << Constants::MSG_NOT_VALID_HOST << std::endl;
            return;
        }
        sshUtils::sshUtils::ssh_execute_cmd(username, host, cmd, color);
    };


    /*
     * 功能：检测目标host和当前登录机器之间是否免密
     * 输入：目标机器host
     * 输出：免密返回0,非免密返回1000
     * */
    int sshUtils::checkAutoPubKey(const char* username, const char* host){
        ssh_session session;
        int rc;

        // 创建SSH会话
        session = ssh_new();
        if (session == nullptr) {
            return Constants::SSH_FAIL;
        }

        // 设置目标服务器连接信息
        ssh_options_set(session, SSH_OPTIONS_HOST, host);

        // 连接到目标服务器
        rc = ssh_connect(session);
        if (rc != SSH_OK) {
            printf("> 无法连接到该服务器，请检查网络是否通畅\n");
            ssh_free(session);
            return Constants::SSH_FAIL;
        }

        // 检测是否是免密登录
        rc = ssh_userauth_publickey_auto(session, username, nullptr);
        ssh_disconnect(session);
        ssh_free(session);
        if (rc != SSH_AUTH_SUCCESS) {
            printf("> 目标机器未进行免密！\n");
            return Constants::FAIL;
        } else {
            return Constants::SUCCESS;
        }
    }


    /*
     * 功能：该方法用来免密登录一个目标机器
     * 输入：目标机器的host
     * 输出：程序执行状态，0表示成功，-1表示失败
     * */
    int sshUtils::loginHost(const char* host, const char* username){
        int returnCode = -1;
        std::string name = std::string(username);
        if(!utils::isHost(host)){
            std::cout << Constants::MSG_NOT_VALID_HOST << std::endl;
            return Constants::NOT_VALID_HOST;
        }

        //检查针对目标机器是否免密, 免密的话就在控制台执行ssh命令登录该机器
        int ret = sshUtils::sshUtils::checkAutoPubKey(username, host);
        std::string sshCommand;
        if (ret == Constants::SUCCESS) {
            sshCommand = "ssh " + name + "@" + std::string(host);
            returnCode = system(sshCommand.c_str());
        } else {
            //如果没有免密，那么返回错误码
            returnCode = ret;
        }
        return returnCode;
    }



    /*
     * 功能：登录机器执行一次命令并返回结果字符串
     * 输入：1、目标机器ip
     *      2、登录用户名
     *      3、登录密码
     *      4、需要执行的命令
     * 输出：成功:打印目标机器执行命令返回的结果
     *      失败：以状态码-1退出
    */
    void sshUtils::ssh_execute_cmd(const char* username, const char *host, const char *cmd, const char* color) {
        ssh_session session;
        int rc;

        //创建一个session会话
        session = ssh_new();
        if (session == nullptr) {
            exit(Constants::FAIL);
        }

        //配置session会话
        ssh_options_set(session, SSH_OPTIONS_HOST, host);
        ssh_options_set(session, SSH_OPTIONS_USER, username);
        ssh_options_set(session, SSH_OPTIONS_PORT_STR, "22");

        //连接到目标服务器
        rc = ssh_connect(session);
        if (rc != SSH_OK) {
            std::cout << Constants::MSG_SSH_FAIL << std::endl;
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //使用公钥进行认证
        int ret = ssh_userauth_publickey_auto(session, username, nullptr);
        if(ret != SSH_AUTH_SUCCESS){
            std::cout << Constants::MSG_NOT_AUTO_PUBKEY << std::endl;
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //创建一个ssh连接通道
        ssh_channel channel = ssh_channel_new(session);
        if (channel == nullptr) {
            fprintf(stderr, "> 创建ssh通道失败: %s\n", ssh_get_error(session));
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //开启ssh通道
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
            fprintf(stderr, "> 开启ssh通道失败: %s\n", ssh_get_error(session));
            ssh_channel_free(channel);
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //执行命令
        rc = ssh_channel_request_exec(channel, cmd);
        if (rc != SSH_OK) {
            fprintf(stderr, "> 执行远程命令失败: %s\n", ssh_get_error(session));
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //打印命令的执行结果
        char buffer[256];
        int nBytes;
        while ((nBytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
            if((*color) == '\0') {
                //std::cout << color << std::endl;
                std::printf("%.*s", nBytes, buffer);
                //std::cout<< std::noshowpos << std::string(buffer) << std::endl;
                //memset(buffer, 0, 256);
            }
            else {
                char* highlightedStr = utils::highlightKeyword(buffer, color);
                std::printf("%.*s", std::strlen(highlightedStr),highlightedStr);
                delete highlightedStr;
                memset(buffer, 0, 256);

            }
        }
        std::cout << std::endl;

        if (nBytes < 0) {
            fprintf(stderr, "> 读取ssh通道失败: %s\n", ssh_get_error(session));
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        //释放资源
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
    }



}