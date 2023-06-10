#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "filesys_ds.h"
#include "filesys_func.h"
using namespace std;

// int curPathId; // 当前目录的inode在BFD中的索引
inode curPath; // 当前目录的inode
vector<sfdItem> curPathSeq; // 当前目录的绝对路径序列

inode root; // 根目录的inode

inode dir_curPath; // search过后目录的inode
vector<sfdItem> dir_curPathSeq; // search过后目录的绝对路径序列

// std::string curPathString = "/"; // 绝对路径字符串
superBlock spb; // 内存中的超级块

std::vector<user> userList; // 所有用户列表
std::vector<singleUserTable> userOpenTable; // 所有的用户打开文件表集合
systemOpenItem systemOpenTable[MAX_NUM_OF_OPENED_FILE]; // 系统打开文件表
int systemOpenTableCnt = 0; // 系统打开文件表的被占用单元数

user anonymous("-----", "123456"); // 默认匿名用户
bool isLoggedIn = false; // 是否已登录 Flag
user curUser = anonymous; // 当前登录的用户

string absPathString() { // 生成绝对路径字符串
    string u;
    for (int i = 0; i < curPathSeq.size(); i++) {
        u += "/";
        u += curPathSeq[i].filename;
    }
    if (u == "") {
        u += "/";
    }
    return u;
}

int main() {
    string tmp, command, parameter;
    initSystem();
    cout << "Initializing..." << endl;
    cout << curUser.username << "@" << absPathString() << "> "; // 第一句提示符
    while (getline(cin, tmp)) {
        int firstSpaceIndex = tmp.find(' ');
        if (firstSpaceIndex == string::npos) { // 若字符串中没有空格
            command = tmp;
            parameter = "";
        } else { // 若字符串中有空格，以第一个空格的位置将其分开
            command = tmp.substr(0, firstSpaceIndex - 0);
            parameter = tmp.substr(firstSpaceIndex + 1, tmp.length() - (firstSpaceIndex + 1));
        }
        cout << "command: \"" << command << "\"\n";
        cout << "parameter: \"" << parameter << "\"\n";
        if (!isLoggedIn) {
            if (command == "login") {
                login();
            } else {
                cout << "请先登录！" << endl;
                cout << curUser.username << "@" << absPathString() << "> "; // 下一句提示符
            }
            continue;
        }

        if (command == "login") {
            login();
        } else if (command == "logout") {
            logout();
        } else if (command == "useradd") {
            useradd();
        } else if (command == "userdel") {
            userdel();
        } else if (command == "format") {
            format();
        } else if (command == "cd") { // cd "Path"
            cd(parameter);
        } else if (command == "ls") {
            ls();
        } else if (command == "mkdir") { // mkdir "newFolderName"
            mkdir(parameter);
        } else if (command == "touch") { // touch "newFileName"
            touch(parameter);
        } else if (command == "rm") { // rm [-d | -r] "toDeleteFolderName"
            rm(parameter);
        } else if (command == "open") { // open "toDeleteFolderName" "r | w | rw"
            open(parameter);
        } else if (command == "close") {
            close();
        } else if (command == "write") {
            write();
        } else if (command == "read") {
            read();
        } else if (command == "useropened") {
            useropened();
        } else if (command == "systemopened") {
            systemopened();
        } else if (command == "editpermit") { // editpermit "toEditFilename"
            editPermit(parameter);
        } else if (command == "viewpermit") { // viewpermit "toEditFilename"
            viewPermit(parameter);
        // } else if (command == "mklink") {
        //     mklink();
        // } else if (command == "cp") {
        //     // cp();
        } else if (command == "mv") { // mv "-r "source" "newFilename"" | "source" "destFolder"
            mv(parameter);
        } else if (command == "exit") {
            break;
        }
        cout << curUser.username << "@" << absPathString() << "> "; // 下一句提示符
    }
    cout << "Shutting down..." << endl;
    saveSystem();
    return 0;
}
