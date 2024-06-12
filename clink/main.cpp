#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include "sshUtils.h"
#include "utils.h"
#include "Constants.h"




//设置字体颜色
#define COLOR_RED "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_BLUE "\033[1;36m"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    //std::string username=nullptr;
    //Clink功能描述
    boost::program_options::options_description desc(
            "Clink Ver: 0.1\n\n"
            "░█▀▀░█░░░▀█▀░█▀█░█░█\n"
            "░█░░░█░░░░█░░█░█░█▀▄\n"
            "░▀▀▀░▀▀▀░▀▀▀░▀░▀░▀░▀\n"
            "\n 用法:  批量建立ssh连接并执行命令.(使用前请保证针对目标机器已经做了免密，默认登录work账户，可通过-u设置登录用户)\n"
              " 批量执行:  ./clink 文件名(保存目标host的列表, 文件名请以list-开头, .txt结尾) '待执行命令'\n"
              " 单独执行: ./clink 目标host '待执行命令'\n"
              " 登录目标机器:  ./clink 目标host\n"
              " 选项");

    //添加命令行选项，目前只支持-h打印帮助
    desc.add_options()
            ("help,h", "打印帮助")
            ("user,u", po::value<std::string>(), "设置登录用户")
            ("ip,i", po::value<std::string>(), "设置待登录的host，list-开头的文件名，批量执行")
            ("exec,e", po::value<std::string>(), "待执行的命令")
            ("color,c", po::value<std::string>(), "设置高亮词汇");

    std::string host,username,cmd,color;
    //定义期望的参数位置，非选项参数，可以有可以没有，但是设定的位置参数必须是选项参数中定义过的
    po::positional_options_description p;
    p.add("ip", 1); //位置参数1是username，读取一个参数
    p.add("exec", 1); //位置参数2是cmd，读取一个参数
    p.add("color", 1); //位置参数3是highlight，读取一个参数


    //通过po库来解析选项类型参数和非选项类型参数，值得注意的是，这个方法中positional和allow_unregistered不能同时出现，否则会抛出异常
    po::variables_map varMap;
    po::store(po::command_line_parser(argc, argv).positional(p).options(desc).run(), varMap);

    po::notify(varMap);

    //检查帮助选项，如果有就打印帮助
    if(varMap.count("help")){
        std::cout << desc << std::endl;
        return Constants::SUCCESS;
    }

    //检查user选项，如果有就为username设定值
    username = varMap.count("user") ? varMap["user"].as<std::string>() : "";
    //检查ip选项，如果有就为host设定值
    host = varMap.count("ip") ? varMap["ip"].as<std::string>() : "";
    //检查exec选项，如果有就为cmd设定值
    cmd = varMap.count("exec") ? varMap["exec"].as<std::string>() : "";
    //检查color选项，如果有就为color设定值
    color = varMap.count("color") ? varMap["color"].as<std::string>() : "";

    //给username一个默认值
    username = username == "" ? "work" : username;


    //参数分为几种情况，首先只有ip
    //1、只有ip
    //2、有ip并且又有待执行的命令
    //3、ip是一个list开头的字符串

    //1、只有ip
    if(varMap.count("ip") && !varMap.count("exec")) {
        //判断ip是否合法
        if (!utils::isHost(host.c_str())) {
            std::cout << Constants::MSG_NOT_VALID_HOST << std::endl;
            return Constants::NOT_VALID_HOST;
        }
        //直接登录该地址
        sshUtils::sshUtils::loginHost(host.c_str(), username.c_str());
        return Constants::SUCCESS;
    }

    //2、3、这两个都是需要执行单条命令的，只不过是单个还是批量的区别
    if(varMap.count("ip") && varMap.count("exec")){
        boost::regex pattern(R"(^list-.*$)");
        //如果ip以list开头，那么批量执行
        if(boost::regex_match(host, pattern)){
            sshUtils::sshUtils::exec_batch(username.c_str(), host.c_str(), cmd.c_str(), color.c_str());
        }
        //否则单个执行
        else{
            sshUtils::sshUtils::exec_single(username.c_str(), host.c_str(), cmd.c_str(), color.c_str());
        }
    }
    return Constants::SUCCESS;
}
