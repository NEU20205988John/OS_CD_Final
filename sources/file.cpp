#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include "filesys_ds.h"
#include "filesys_func.h"

using namespace std;

bool open(string openstr) {
    string curpathstr;//绝对路径字符串 
    int firstspace = openstr.find(' ');
    string openmode;//打开方式
    curpathstr = openstr.substr(0, firstspace - 0);
    openmode = openstr.substr(firstspace + 1, openstr.length() - (firstspace + 1));
    
    searchFile(curpathstr);//搜索文件提取文件的inode
    inode curFCB = dir_curPath;
    
    bool opened = false;
    int systemtableidx = 0;//系统打开文件表入口
    bool canr = false;
    if(curFCB.ownerName == curUser.username){
        if(curFCB.accessTable.owner.canRead){
            canr = true;
        }
    }
    else if(curFCB.ownerName != curUser.username){
        if(curFCB.accessTable.nonOwner.canRead){
            canr = true;
           
        }
    }
    if(canr){
        for(int i=0; i<MAX_NUM_OF_OPENED_FILE; i++){
            if(systemOpenTable[i].isOccupied == true ){
                if(systemOpenTable[i].FCB.fileId == curFCB.fileId ){
                    systemOpenTable[i].openCnt++;
                    opened = true;
                    systemtableidx = i;
                }
                break;
            }
        }//首次遍历系统打开文件表，如果已被打开，openCnt++;未被打开，再次遍历
        
        if(!opened){
            for(int i=0; i<MAX_NUM_OF_OPENED_FILE; i++){
                if(systemOpenTable[i].isOccupied == false){
                    systemOpenTable[i].FCB = curFCB;
                    systemOpenTable[i].isOccupied = true;
                    systemOpenTable[i].openCnt = 1;
                    systemOpenTableCnt++;//系统打开文件表的被占用单元数+1
                    systemtableidx = i;
                    break;
                }
            }
        }

        for(int i=0; i<userOpenTable.size();i++){
            if(curUser.username == userOpenTable[i].username){

                userOpenTable[i].singleTableCnt++;// 用户打开文件表的被占用单元数+1
                for(int j=0; j<MAX_NUM_OF_OPENED_FILE_PER_USER;j++){
                    if(userOpenTable[i].singleTable[j].isOccupied == false){
                        userOpenTable[i].singleTable[j].rwpointer = 0;
                        userOpenTable[i].singleTable[j].systemTableIdx = systemtableidx;
                        userOpenTable[i].singleTable[j].isOccupied = true;
                        userOpenTable[i].singleTable[j].fd = chrono::system_clock::now().time_since_epoch().count()/chrono::system_clock::period::den;//时间戳命名文件描述符
                        cout<<"文件描述符为："<<userOpenTable[i].singleTable[j].fd<<endl;
                        if(openmode == "r"){
                            userOpenTable[i].singleTable[j].mode = 1;
                        }
                        else if(openmode == "w"){
                            userOpenTable[i].singleTable[j].mode = 2;

                        }
                        else if(openmode == "rw"){
                            userOpenTable[i].singleTable[j].mode = 3;
                        }
                    break;
                    }
                }
                break;
                }
        }
        cout<<"打开成功"<<endl;
        return true;
    }

    else {//canr等于false
        cout<<"没有访问权限"<<endl;
        return false;
    }
}

bool close() {
    cout<<"请输入所要关闭文件的文件描述符：";
    long long curfd;
    cin>>curfd;
    cout<<endl; 
    inode curFCB = dir_curPath;
    int systemtableidx;// 系统打开文件表入口
    for(int i=0; i<userOpenTable.size();i++){
        if(curUser.username == userOpenTable[i].username){
            userOpenTable[i].singleTableCnt--;// 用户打开文件表的被占用单元数-1
            for(int j=0; j<MAX_NUM_OF_OPENED_FILE_PER_USER;j++){
                if(curfd == userOpenTable[i].singleTable[j].fd ) {
                    userOpenTable[i].singleTable[j].isOccupied = false;
                    systemtableidx = userOpenTable[i].singleTable[j].systemTableIdx;
                    if(systemOpenTable[systemtableidx].openCnt > 1){//当前访问数大于1
                        systemOpenTable[systemtableidx].openCnt--;
                    }
                    else{ //访问数为1，需要从系统打开文件表中删去
                        systemOpenTable[systemtableidx].openCnt = 0;
                        systemOpenTable[systemtableidx].isOccupied = false;
                        systemOpenTableCnt--;//系统打开文件表的被占用单元数-1
                    }
                    cout<<"关闭成功"<<endl;
                    return true;
                }
            }
            cout<<"输入文件描述符错误"<<endl;
            return false;
        }
    }
    return false;
}

// 输入：long long fd -> std::vector<singleUserTable> userOpenTable 
//全局变量curUser.username和singleUserTable::username匹配
//找对对应的这张表，进入后匹配fd
//rwpointer、写入的内容 均提示用户键盘输入
//rwpointer / BLOCK_SIZE 得到 向pyhsicalAddress中的第几个块写入，直接打开块写入，若rwpointer / BLOCK_SIZE超过BlockCnt则报错
//访问权限控制：1.文件本身的accessTable  2.用户打开文件表里的mode
// 写入成功输出提示信息
//

void write() {
    string s;
    long long fd;
    cout << "请输入需读文件的文件描述符：";
    cin >> fd;
    for(int i=0;i<userOpenTable.size();i++){
        if(curUser.username == userOpenTable[i].username){
            for(int j=0;j<MAX_NUM_OF_OPENED_FILE_PER_USER;j++){
                if(fd == userOpenTable[i].singleTable[j].fd){
                    int index = userOpenTable[i].singleTable[j].systemTableIdx;
                    inode node = systemOpenTable[index].FCB;

                    if (node.ownerName == curUser.username) {
                        if (!node.accessTable.owner.canWrite) {
                            cout << "Error: 当前用户无写权限" << endl;
                            return;
                        }
                    } else {
                        if (!node.accessTable.nonOwner.canWrite) {
                            cout << "Error: 当前用户无写权限" << endl;
                            return;
                        }
                    }
                    if (userOpenTable[i].singleTable[j].mode == 1) {
                        cout << "Error: 文件的打开类型为“只读”" << endl;
                        return;
                    }

                    cout<<"请输入读写指针偏移量: "<<endl;
                    cin>>userOpenTable[i].singleTable[j].rwpointer;
                    cout<<"请输入写入内容: "<<endl;
                    cin>>s;
                    int No = userOpenTable[i].singleTable[j].rwpointer / BLOCK_SIZE;
                    if (No > node.BlockCnt) {
                        cout << "Error: 指针偏移量超过文件大小" << endl;
                        return;
                    }
                    if(No >= NUM_OF_DIRECT_ADDR){//no direct 间接寻址
                        ifstream ifs("../blocks/"+to_string(node.physicalAddr.indirect));
                        vector<int> vc;
                        int x;
                        while(ifs>>x){
                            vc.push_back(x);
                        }
                        ifs.close();
                        ofstream ofs("../blocks/"+to_string(vc[No-NUM_OF_DIRECT_ADDR]));
                        ofs << s;
                        ofs.close();
                    } else {
                        ofstream ofs("../blocks/"+to_string(node.physicalAddr.direct[No]));
                        ofs << s;
                        ofs.close();
                    }
                    return;
                }
            }
            cout << "Error: 未找到对应文件" << endl;
            return;
        }
    }
    return;
}

void read() {
    string s;
    long long fd;
    cout << "请输入需读文件的文件描述符：";
    cin >> fd;
    for(int i=0;i<userOpenTable.size();i++){
        if(curUser.username == userOpenTable[i].username){
            for(int j=0;j<MAX_NUM_OF_OPENED_FILE_PER_USER;j++){
                if(fd == userOpenTable[i].singleTable[j].fd){
                    int index = userOpenTable[i].singleTable[j].systemTableIdx;
                    inode node = systemOpenTable[index].FCB;

                    if (node.ownerName == curUser.username) {
                        if (!node.accessTable.owner.canRead) {
                            cout << "Error: 当前用户无读权限" << endl;
                            return;
                        }
                    } else {
                        if (!node.accessTable.nonOwner.canRead) {
                            cout << "Error: 当前用户无读权限" << endl;
                            return;
                        }
                    }
                    if (userOpenTable[i].singleTable[j].mode == 2) {
                        cout << "Error: 文件的打开类型为“只写”" << endl;
                        return;
                    }

                    cout<<"请输入读写指针偏移量: "<<endl;
                    cin>>userOpenTable[i].singleTable[j].rwpointer;
                    int No = userOpenTable[i].singleTable[j].rwpointer / BLOCK_SIZE;
                    if (No > node.BlockCnt) {
                        cout << "Error: 指针偏移量超过文件大小" << endl;
                        return;
                    }
                    cout << "-------文件内容-------" << endl;
                    if(No >= NUM_OF_DIRECT_ADDR){ //no direct 间接寻址
                        ifstream ifs1("../blocks/"+to_string(node.physicalAddr.indirect));
                        vector<int> vc;
                        int x;
                        while(ifs1>>x){
                            vc.push_back(x);
                        }
                        ifs1.close();

                        ifstream ifs2("../blocks/"+to_string(vc[No-NUM_OF_DIRECT_ADDR]));
                        while (getline(ifs2, s)) {
                            cout << s;
                        }
                        ifs2.close();
                    } else {
                        ifstream ifs("../blocks/"+to_string(node.physicalAddr.direct[No]));
                        while (getline(ifs, s)) {
                            cout << s;
                        }
                        ifs.close();
                    }
                    return;
                }
            }
            cout << "Error: 未找到对应文件" << endl;
            return;
        }
    }
    return;
}

bool useropened() { // 输出当前用户的打开文件表
    for(int i=0; i<userOpenTable.size();i++){
        cout<< i <<"  用户名:"<< userOpenTable[i].username<<endl;
        for(int j=0; j<MAX_NUM_OF_OPENED_FILE_PER_USER;j++){
            if(userOpenTable[i].singleTable[j].isOccupied == true){
                cout << "--------------------------------------------" << endl;
                cout<<"    文件描述符:"<< userOpenTable[i].singleTable[j].fd<<endl;
                cout<<"    打开方式:"<< userOpenTable[i].singleTable[j].mode<<endl;
                cout<<"    读写指针:"<< userOpenTable[i].singleTable[j].rwpointer<<endl;
                cout<<"    系统打开文件表入口:"<< userOpenTable[i].singleTable[j].systemTableIdx<<endl;
            }
        }
    }
    return true;
}


bool systemopened() { // 输出系统打开文件表
    for(int i=0; i<MAX_NUM_OF_OPENED_FILE; i++){
        if(systemOpenTable[i].isOccupied == true){
            cout << i <<" 文件标号:"<< systemOpenTable[i].FCB.fileId;
            cout<<"  访问次数:"<< systemOpenTable[i].openCnt;
            cout<<"  修改标志:"<< systemOpenTable[i].modifiedFlag <<endl;
            
        }
    }
    return true;
}