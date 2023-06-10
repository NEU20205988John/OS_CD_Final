#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "filesys_ds.h"
#include "filesys_func.h"
using namespace std;

vector<inode> curInodeBlock; // 当前读入到内存的inode磁盘块vector
int curInodeBlockNum; // 当前读入到内存的inode磁盘块号

vector<inode> readInodeBlock(int No) { //输入磁盘块号，返回该磁盘块内所有inode的一个vector
    vector<inode> vc;
    ifstream ifs("../blocks/"+to_string(No));
    int num_inodes;
    ifs>>num_inodes;
    
    for(int i=0; i<num_inodes; i++){
        inode node;
        ifs>>node.fileId;
        ifs>>node.BlockCnt;
        ifs>>node.isDir;
        ifs>>node.dirCnt;
        for(int j=0;j<10;j++){//直接寻址
            ifs>>node.physicalAddr.direct[j];
        }
        ifs>>node.physicalAddr.indirect;//间接寻址
        ifs>>node.ownerName;
        ifs>>node.accessTable.owner.canRead;
        ifs>>node.accessTable.owner.canWrite;
        ifs>>node.accessTable.owner.canExecute;
        ifs>>node.accessTable.owner.canDelete;
        ifs>>node.accessTable.owner.canChangePermit;
        ifs>>node.accessTable.nonOwner.canRead;
        ifs>>node.accessTable.nonOwner.canWrite;
        ifs>>node.accessTable.nonOwner.canExecute;
        ifs>>node.accessTable.nonOwner.canDelete;
        ifs>>node.accessTable.nonOwner.canChangePermit;
        ifs>>node.linkCnt;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）
        vc.push_back(node);
    }
    ifs.close();
    return vc;
}

void writeInodeBlock(vector<inode> &vc,int No) {
    ofstream ofs("../blocks/"+to_string(No));
    int num_inodes = vc.size();
    ofs<<num_inodes<<endl;
    for(int i=0; i<num_inodes; i++){
        ofs<<vc[i].fileId<<" ";
        ofs<<vc[i].BlockCnt<<" ";
        ofs<<vc[i].isDir<<" ";
        ofs<<vc[i].dirCnt<<" ";
        for(int j=0;j<10;j++){//直接寻址
            ofs<<vc[i].physicalAddr.direct[j]<<" ";
        }
        ofs<<vc[i].physicalAddr.indirect<<" ";//间接寻址
        ofs<<vc[i].ownerName<<" ";
        ofs<<vc[i].accessTable.owner.canRead<<" ";
        ofs<<vc[i].accessTable.owner.canWrite<<" ";
        ofs<<vc[i].accessTable.owner.canExecute<<" ";
        ofs<<vc[i].accessTable.owner.canDelete<<" ";
        ofs<<vc[i].accessTable.owner.canChangePermit<<" ";
        ofs<<vc[i].accessTable.nonOwner.canRead<<" ";
        ofs<<vc[i].accessTable.nonOwner.canWrite<<" ";
        ofs<<vc[i].accessTable.nonOwner.canExecute<<" ";
        ofs<<vc[i].accessTable.nonOwner.canDelete<<" ";
        ofs<<vc[i].accessTable.nonOwner.canChangePermit<<" ";
        ofs<<vc[i].linkCnt<<endl;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）
    }
    ofs.flush();
    ofs.close();
    return;
}

void updateInode(inode &u) { // 将内存中更改过的inode写回磁盘块
    vector<inode> vc = readInodeBlock(u.fileId / INODE_PER_BLOCK + 1);
    for (auto i = vc.begin(); i < vc.end(); i++) {
        if (i->fileId == u.fileId) {
            *i = u;
            break;
        }
    }
    writeInodeBlock(vc, u.fileId / INODE_PER_BLOCK + 1);
}

void delInode(inode &u) { // 从磁盘块中删除传入的inode
    vector<inode> vc = readInodeBlock(u.fileId / INODE_PER_BLOCK + 1);
    for (auto i = vc.begin(); i < vc.end(); i++) {
        if (i->fileId == u.fileId) {
            vc.erase(i);
            break;
        }
    }
    writeInodeBlock(vc, u.fileId / INODE_PER_BLOCK + 1);
}

vector<sfdItem> readSFDBlock(int blockNo) { // 读取磁盘块blockNo，生成对应的vector<sfdItem>
    ifstream ifs("../blocks/"+to_string(blockNo));
    vector<sfdItem> vs;
    sfdItem tmp;
    int num;
    ifs >> num;
    for (int i = 0; i < num; i++) {
        ifs >> tmp.filename;
        ifs >> tmp.fileId;
        vs.push_back(tmp);
    }
    ifs.close();
    return vs;
}

void writeSFDBlock(vector<sfdItem> &vs, int blockNo) { // 将修改过的vector<sfdItem>写回磁盘块blockNo中
    ofstream ofs("../blocks/"+to_string(blockNo));
    ofs << vs.size() << endl;
    for (int i = 0; i < vs.size(); i++) {
        ofs << vs[i].filename << " ";
        ofs << vs[i].fileId << endl;
    }
    ofs.flush();
    ofs.close();
}

void deleteFromSFD(inode path, int fileId) { // 删除指定fileId的SFD表项
    vector<sfdItem> vs = readSFDBlock(path.physicalAddr.direct[0]);
    for (auto i = vs.begin(); i < vs.end(); i++) {
        if (i->fileId == fileId) {
            cout << "Deleting: " << i->filename << endl;
            vs.erase(i);
            break;
        }
    }
    writeSFDBlock(vs, path.physicalAddr.direct[0]);
}

void updateSFD(inode path, sfdItem toModify) { // 将内存中更改过的SFD表项写回
    vector<sfdItem> vs = readSFDBlock(path.physicalAddr.direct[0]);
    for (auto i = vs.begin(); i < vs.end(); i++) {
        if (i->fileId == path.fileId) {
            *i = toModify;
            break;
        }
    }
    writeSFDBlock(vs, path.physicalAddr.direct[0]);
}

void addToSFD(inode path, sfdItem toAdd) { // 向指定目录的SFD表中增加一项
    vector<sfdItem> vs = readSFDBlock(path.physicalAddr.direct[0]);
    vs.push_back(toAdd);
    writeSFDBlock(vs, path.physicalAddr.direct[0]);
}

sfdItem getSFDItem(inode path, string filename) { // 根据文件名获取SFD表项
    vector<sfdItem> vs = readSFDBlock(path.physicalAddr.direct[0]);
    for (auto i = vs.begin(); i < vs.end(); i++) {
        if (i->filename == filename) {
            return *i;
        }
    }
    return sfdItem("-----", -1); // 若未找到，返回fileId值为-1的sfdItem
}

sfdItem getSFDItemByFileId(inode path, int fileId) { // 根据文件Id获取SFD表项
    vector<sfdItem> vs = readSFDBlock(path.physicalAddr.direct[0]);
    for (auto i = vs.begin(); i < vs.end(); i++) {
        if (i->fileId == fileId) {
            return *i;
        }
    }
    return sfdItem("-----", -1); // 若未找到，返回fileId值为-1的sfdItem
}

sfdItem searchSFD(inode path, string filename) {
    // 在path下，根据文件名查找对应的sfdItem，找到返回对应的sfdItem
    int sfdBlock = path.physicalAddr.direct[0];
    vector<sfdItem> sfdList = readSFDBlock(sfdBlock);
    for (int i = 0; i < sfdList.size(); i++) {
        if (sfdList[i].filename == filename) {
            return sfdList[i];
        }
    }
    // 若未找到，返回一个fileId为-1的sfdItem
    return sfdItem("-----", -1);
}

int findAvaliableInode(vector<inode>::iterator &it) { // 返回一个可用的inode编号，以及要插入位置
    for (int i = 1; i < NUM_OF_RESERVED_BLOCK; i++) {
        curInodeBlock = readInodeBlock(i); // 每次将i号磁盘块读入
        curInodeBlockNum = i;

        if (curInodeBlock.size() == 0) { // 块为空，特判
            it = curInodeBlock.begin();
            return (i - 1) * INODE_PER_BLOCK;
        }
        auto k = curInodeBlock.begin();
        auto j = ++(curInodeBlock.begin());
        for (; j < curInodeBlock.end(); j++, k++) {
            if (j->fileId != (k->fileId + 1)) {
                break; // j为需要插入的位置在curInodeBlock的下标
            }
        }
        if (curInodeBlock.size() < INODE_PER_BLOCK) {
            it = j;
            return (k->fileId + 1);
        }
        // auto k = 0;
        // auto j = 1;
        // for (; j < curInodeBlock.size(); j++, k++) {
        //     if (curInodeBlock[j].fileId != (curInodeBlock[k].fileId + 1)) {
        //         break; // j为需要插入的位置在curInodeBlock的下标
        //     }
        // }
        // if (j < INODE_PER_BLOCK) {
        //     vector<inode>::iterator jit = curInodeBlock.begin();
        //     for (int p = 0; p < j; p++) {
        //         jit++;
        //     }
        //     it = jit;
            // return (curInodeBlock[k].fileId + 1);
            // return (j + (i-1) * INODE_PER_BLOCK);
        //}
    }
    return -1; // 没有空闲的inode
}

inode createNewInode(int fileId, string ownerName, bool isDir, int blockCnt = 1) {
    // 使用找到的空闲inode编号、文件所有者用户名及isDir，创建全新的inode节点
    inode _1st_inode;

    _1st_inode.fileId = fileId;//fileId=0代表这是根目录,SFD中索引为0
    _1st_inode.isDir = isDir; //根目录
    _1st_inode.dirCnt = 0; //初始化时为0
    _1st_inode.BlockCnt = blockCnt; //所占磁盘块数，初始时为blockCnt
    int numDirect = (blockCnt <= NUM_OF_DIRECT_ADDR ? blockCnt : NUM_OF_DIRECT_ADDR);

    // 为新inode分配磁盘块
    for (int i = 0; i < numDirect; i++) {
        _1st_inode.physicalAddr.direct[i] = allocate_block(); // 分配直接寻址的内容磁盘块
    }
    for (int j = numDirect; j < NUM_OF_DIRECT_ADDR; j++) {
        _1st_inode.physicalAddr.direct[j] = -1; // 未用到的直接地址填-1
    }
    if (blockCnt <= NUM_OF_DIRECT_ADDR) {
        _1st_inode.physicalAddr.indirect = -1; //直接地址够用，不使用间接寻址
    } else {
        _1st_inode.physicalAddr.indirect = allocate_block(); // 分配间址磁盘块
        ofstream ofs("../blocks/"+to_string(_1st_inode.physicalAddr.indirect));
        for (int k = NUM_OF_DIRECT_ADDR; k < blockCnt; k++) {
            int t = allocate_block(); // 分配间接寻址的内容磁盘块并写入间址磁盘块
            ofs << t << endl;
        }
        ofs.flush();
        ofs.close();
    }

    _1st_inode.ownerName = ownerName;
    permitCtrl pc1(true,true,true,true,true);
    permitCtrl pc2(true,true,true,true,true);
    _1st_inode.accessTable = accessCtrl(pc1,pc2);
    _1st_inode.linkCnt = 1;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）
    
    return _1st_inode;
}

void createNewInodeSFD(inode &iNode) {
    vector<sfdItem> tmp;

    sfdItem sfd1;
    sfd1.filename = ".";
    sfd1.fileId = iNode.fileId;
    tmp.push_back(sfd1);

    sfdItem sfd2;
    sfd2.filename = "..";
    sfd2.fileId = curPath.fileId;
    tmp.push_back(sfd2);

    writeSFDBlock(tmp, iNode.physicalAddr.direct[0]); // 对于目录文件的SFD，简化为只使用一个块存放整个SFD
}

void searchFile(string filename) {
    // 先查找根目录？
    string s1 = filename, s;
    for (int i = 0; i < s1.length(); i++) {
        if (s1[i] == '/')  {
            s += ' ';
        } else {
            s += s1[i];
        }
    }
    // cout<<"s = "<<s<<endl;
    stringstream ss;
    ss.str(s);
    vector<string> vc;
    while (!ss.eof()) {
        ss >> s1;
        // cout<<"s1 = "<<s1<<endl;
        vc.push_back(s1);
    }
    
    if (filename[0] == '/') { // 如果当前要查找绝对路径
        dir_curPathSeq.clear();
        dir_curPath = root;
        if (filename == "/") { // 若路径为根目录，即可直接返回
            return;
        }
    } else {
        dir_curPathSeq = curPathSeq;
        dir_curPath = curPath;
    }

    // 当前文件名是绝对路径，应当从根目录开始查找
    // SFD->BFD交替查找进行
    int depth = vc.size();
    // int len = sfd.size();
    // 先存起来 searchSFD(dir_curPath,vc[i]) 再打开找 fileID
    // cout << "depth: " << depth << endl;
    // cout << "vc[0]: \"" << vc[0] << "\"" << endl;
    for (int i = 0; i < depth; i++) {
        sfdItem sfd = searchSFD(dir_curPath, vc[i]);
        if (sfd.fileId == -1) {   
            dir_curPath.fileId = -1;
            // not found
            // iNode.fileId = -1;
            return;
        } else {
            int j;
            vector<inode> vec = readInodeBlock(sfd.fileId / INODE_PER_BLOCK + 1);
            for (j = 0; j < vec.size(); j++) {
                if (vec[j].fileId == sfd.fileId) {
                    break;
                }
            }
            dir_curPath = vec[j];
        }
        if (vc[i] == ".") {
            ; // 保持在当前目录不动 无操作NOP
        } else if (vc[i] == "..") {
            // 返回上一级目录
            if (dir_curPathSeq.size() != 0) { // 若为根目录，则不用pop_back
                dir_curPathSeq.pop_back();
            }
        } else {
            dir_curPathSeq.push_back(sfd);
        }
    }
}



void mkdir(string params) { // 在当前目录下，创建名为${params}的目录
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex != string::npos) { // 若输入参数字符串中有空格，则认为是不合法文件名
        cout << "Invalid arguments!" << endl;
        return;
    } else {
        // 1. 查询是否重名
        sfdItem test = searchSFD(curPath, params);
        if (test.fileId != -1) {
            cout << "Error: Duplicated filename: " << params << endl;
            return;
        }
        // 2. 创建新目录的inode
        vector<inode>::iterator newFileIter;
        int newFileId = findAvaliableInode(newFileIter); // 在磁盘的inode表中查找一个可用的inode编号
        inode newInode = createNewInode(newFileId, curUser.username, true);
        createNewInodeSFD(newInode);
        curInodeBlock.push_back(newInode);
        writeInodeBlock(curInodeBlock, curInodeBlockNum);
        // 3. 改变curPath的文件内容（SFD）
        vector<sfdItem> vs = readSFDBlock(curPath.physicalAddr.direct[0]);
        vs.push_back(sfdItem(params, newFileId));
        writeSFDBlock(vs, curPath.physicalAddr.direct[0]);
        // 4. curPath.dirCnt++
        curPath.dirCnt++;
        updateInode(curPath);
    }
}

void touch(string params) { // 在当前目录下，创建名为${params}的文件
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex != string::npos) { // 若输入参数字符串中有空格，则认为是不合法文件名
        cout << "Invalid arguments!" << endl;
        return;
    } else {
        int blkCnt = 0;
        while (true) {
            cout << "请给出所占磁盘块数：";
            cin >> blkCnt;
            if (blkCnt == 0) {
                cout << "输入无效，退出" << endl;
                return;
            } else {
                cout << "blkCnt: " << blkCnt << endl;
                break;
            }
        }
        
        // 1. 查询是否重名
        sfdItem test = searchSFD(curPath, params);
        if (test.fileId != -1) {
            cout << "Error: Duplicated filename: " << params << endl;
            return;
        }
        // 2. 创建新目录的inode
        vector<inode>::iterator newFileIter;
        int newFileId = findAvaliableInode(newFileIter); // 在磁盘的inode表中查找一个可用的inode编号
        inode newInode = createNewInode(newFileId, curUser.username, false, blkCnt);
        createNewInodeSFD(newInode);
        curInodeBlock.push_back(newInode);
        writeInodeBlock(curInodeBlock, curInodeBlockNum);
        // 3. 改变curPath的文件内容（SFD）
        vector<sfdItem> vs = readSFDBlock(curPath.physicalAddr.direct[0]);
        vs.push_back(sfdItem(params, newFileId));
        writeSFDBlock(vs, curPath.physicalAddr.direct[0]);
        // 4. curPath.dirCnt++
        curPath.dirCnt++;
        updateInode(curPath);
    }
}

void cd(string filename) {   
    searchFile(filename);
    if(dir_curPath.fileId == -1){
        cout<<"找不到目录！"<<endl;
        return;
    }
    else{
        //检查是否是目录
        if(dir_curPath.isDir){
            curPath = dir_curPath;
            curPathSeq = dir_curPathSeq;
            ;//成功cd到指定路径
        }
        else{
            cout<<"当前路径不是目录！"<<endl;
        }
    }
}

// curpath -> physicalAddr(direct[0]) -> No -> readSFDBlock()  ::vector<sfdItem> , （name） 
//for each fileID都得找对应的inode ReadInodeBlock(fileID/inodeperblock+1) -> vector<inode>.isDir true/false（isDir<DIR> ）->print   
//name & isDir<DIR> ->
void ls() {
    vector<sfdItem> vc = readSFDBlock(curPath.physicalAddr.direct[0]);
    for(int i=0; i<vc.size(); i++){
        cout<<vc[i].filename<<" ";
        vector<inode> inodes = readInodeBlock(vc[i].fileId/INODE_PER_BLOCK+1);
        for(int j=0; j<inodes.size(); j++){
            if(vc[i].fileId==inodes[j].fileId){
                if(inodes[j].isDir==true){
                    cout<<"<DIR>"<<endl;
                }
                else{
                    cout<<endl;
                }
                break;
            }
        }
    }
    return;
}

string absPathStringFromSFDList(vector<sfdItem> &vs) { // 生成绝对路径字符串
    string u;
    for (int i = 0; i < vs.size(); i++) {
        u += "/";
        u += vs[i].filename;
    }
    if (u == "") {
        u += "/";
    }
    return u;
}

void deleteAFile(inode toDeleteInode) {
    // 1.不是普通文件
        if (toDeleteInode.isDir) {
            cout << "Error: 要删除的是目录，请使用rm -d 或rm -f" << endl;
            return;
        }
        // 2.文件本身访问权限控制
        if (toDeleteInode.ownerName == curUser.username) {
            if (!toDeleteInode.accessTable.owner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        } else {
            if (!toDeleteInode.accessTable.nonOwner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        }
        // .. 所在目录访问权限控制
        // 3. 正在系统打开表中打开
        for (int i = 0; i < MAX_NUM_OF_OPENED_FILE; i++) {
            if (systemOpenTable[i].FCB.fileId == toDeleteInode.fileId) {
                cout << "Error: 文件正被打开，无法删除" << endl;
                return;
            }
        }
        
        // 获得要删除文件所在目录的inode
        if (dir_curPathSeq.size() > 0) {
            cout << "dir_curPathSeq.pop_back()" << endl;
            dir_curPathSeq.pop_back();
        }
        string toDeletePath = absPathStringFromSFDList(dir_curPathSeq);
        
        searchFile(toDeletePath);
        inode toDeletePathInode = dir_curPath; // 要删除文件所在目录的inode
        
        // 1. 释放要删除文件所占的所有磁盘块
        if (toDeleteInode.BlockCnt <= NUM_OF_DIRECT_ADDR) {
            for (int i = 0; i < toDeleteInode.BlockCnt; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]);
            }
        } else {
            for (int i = 0; i < NUM_OF_DIRECT_ADDR; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]); // 1.1 释放直接寻址的磁盘块
            }
            ifstream ifs("../blocks/" + to_string(toDeleteInode.physicalAddr.indirect));
            // vector<int> vc;
            int x;
            while(ifs >> x){
                // vc.push_back(x);
                FreeABlock(x); // 1.2 释放间接寻址的磁盘块
            }
            ifs.close();
            FreeABlock(toDeleteInode.physicalAddr.indirect); // 1.3 释放间址磁盘块
        }
        // 2. 删除被删文件的inode
        delInode(toDeleteInode);
        // 3. 从所在目录的SFD删除被删文件对应项
        deleteFromSFD(toDeletePathInode, toDeleteInode.fileId);
        // 4. toDeletePathInode.dirCnt--;
        toDeletePathInode.dirCnt--;
        updateInode(toDeletePathInode);
}

void deleteAFolder(inode toDeleteInode) {
        // 1.不是目录或不是空目录
        if (!toDeleteInode.isDir) {
            cout << "Error: 要删除的是普通文件，请勿添加参数" << endl;
            return;
        }
        if (toDeleteInode.dirCnt > 0) {
            cout << "Error: 要删除的目录非空，请使用rm -r" << endl;
        }
        // 2.目录本身访问权限控制
        if (toDeleteInode.ownerName == curUser.username) {
            if (!toDeleteInode.accessTable.owner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        } else {
            if (!toDeleteInode.accessTable.nonOwner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        }
        // .. 所在目录访问权限控制
        // 3. 是根目录
        if (toDeleteInode.fileId == root.fileId) {
            cout << "Error: 要删除目录为根目录，无法删除" << endl;
            return;
        }
        
        // 获得要删除目录所在目录的inode
        if (dir_curPathSeq.size() > 0) {
            cout << "dir_curPathSeq.pop_back()" << endl;
            dir_curPathSeq.pop_back();
        }
        string toDeletePath = absPathStringFromSFDList(dir_curPathSeq);
        
        searchFile(toDeletePath);
        inode toDeletePathInode = dir_curPath; // 要删除目录所在目录的inode
        
        // 1. 释放要删除目录所占的所有磁盘块
        if (toDeleteInode.BlockCnt <= NUM_OF_DIRECT_ADDR) {
            for (int i = 0; i < toDeleteInode.BlockCnt; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]);
            }
        } else {
            for (int i = 0; i < NUM_OF_DIRECT_ADDR; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]); // 1.1 释放直接寻址的磁盘块
            }
            ifstream ifs("../blocks/" + to_string(toDeleteInode.physicalAddr.indirect));
            // vector<int> vc;
            int x;
            while(ifs >> x){
                // vc.push_back(x);
                FreeABlock(x); // 1.2 释放间接寻址的磁盘块
            }
            ifs.close();
            FreeABlock(toDeleteInode.physicalAddr.indirect); // 1.3 释放间址磁盘块
        }
        // 2. 删除被删目录的inode
        delInode(toDeleteInode);
        // 3. 从所在目录的SFD删除被删目录对应项
        deleteFromSFD(toDeletePathInode, toDeleteInode.fileId);
        // 4. toDeletePathInode.dirCnt--;
        toDeletePathInode.dirCnt--;
        updateInode(toDeletePathInode);
}

void deleteAFolderRecursively(inode toDeleteInode) {
    // 1.不是目录
        if (!toDeleteInode.isDir) {
            cout << "Error: 要删除的是普通文件，请勿添加参数" << endl;
            return;
        }
        // 2.目录本身访问权限控制
        if (toDeleteInode.ownerName == curUser.username) {
            if (!toDeleteInode.accessTable.owner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        } else {
            if (!toDeleteInode.accessTable.nonOwner.canDelete) {
                cout << "Error: 当前用户无删除权限" << endl;
                return;
            }
        }
        // .. 所在目录访问权限控制
        // 3. 是根目录
        if (toDeleteInode.fileId == root.fileId) {
            cout << "Error: 要删除目录为根目录，无法删除" << endl;
            return;
        }

        // 获得要删除目录所在目录的inode
        if (dir_curPathSeq.size() > 0) {
            cout << "dir_curPathSeq.pop_back()" << endl;
            dir_curPathSeq.pop_back();
        }
        string toDeletePath = absPathStringFromSFDList(dir_curPathSeq);
        searchFile(toDeletePath);
        inode toDeletePathInode = dir_curPath; // 要删除目录所在目录的inode

        // 获取要删除目录的整个SFD
        vector<sfdItem> vs = readSFDBlock(toDeleteInode.physicalAddr.direct[0]);
        vs.erase(vs.begin()); // 去除 "." 的SFD项
        vs.erase(vs.begin()); // 去除 ".." 的SFD项

        for (auto i = vs.begin(); i < vs.end(); i++) {
            // 对于每一个SFD表项，获取其对应的inode，若为普通文件，执行deleteAFile()；
            // 若为目录，则执行deleteAFolderRecursiveLy()，递归
            vector<inode> vc = readInodeBlock(i->fileId / INODE_PER_BLOCK + 1);
            for (auto j = vc.begin(); j < vc.end(); j++) {
                if (j->fileId == i->fileId) {
                    if (j->isDir) {
                        deleteAFolderRecursively(*j);
                    } else {
                        deleteAFile(*j);
                    }
                    break;
                }
            }
            writeInodeBlock(vc, i->fileId / INODE_PER_BLOCK + 1);
        }
        // 经过循环，当前被删除目录应已变为空目录
        // 递归：以下应与删除一个空目录的操作一致
        
        // 1. 释放要删除目录所占的所有磁盘块
        if (toDeleteInode.BlockCnt <= NUM_OF_DIRECT_ADDR) {
            for (int i = 0; i < toDeleteInode.BlockCnt; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]);
            }
        } else {
            for (int i = 0; i < NUM_OF_DIRECT_ADDR; i++) {
                FreeABlock(toDeleteInode.physicalAddr.direct[i]); // 1.1 释放直接寻址的磁盘块
            }
            ifstream ifs("../blocks/" + to_string(toDeleteInode.physicalAddr.indirect));
            // vector<int> vc;
            int x;
            while(ifs >> x){
                // vc.push_back(x);
                FreeABlock(x); // 1.2 释放间接寻址的磁盘块
            }
            ifs.close();
            FreeABlock(toDeleteInode.physicalAddr.indirect); // 1.3 释放间址磁盘块
        }
        // 2. 删除被删目录的inode
        delInode(toDeleteInode);
        // 3. 从所在目录的SFD删除被删目录对应项
        deleteFromSFD(toDeletePathInode, toDeleteInode.fileId);
        // 4. toDeletePathInode.dirCnt--;
        toDeletePathInode.dirCnt--;
        updateInode(toDeletePathInode);
}

void rm(string params) {
    string param1, param2;
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex == string::npos) { // 若字符串中没有空格
        param1 = params;
        param2 = "";
    } else { // 若字符串中有空格，以第一个空格的位置将其分开
        param1 = params.substr(0, firstSpaceIndex - 0);
        param2 = params.substr(firstSpaceIndex + 1, params.length() - (firstSpaceIndex + 1));
    }
    cout << "param1: \"" << param1 << "\"\n";
    cout << "param2: \"" << param2 << "\"\n";

    if (param2 == "") { // 删除单个文件，param1为文件名
        searchFile(param1);
        if (dir_curPath.fileId != -1) {
            inode toDeleteInode = dir_curPath; // 要删除文件的inode
            deleteAFile(toDeleteInode);
        } else {
            cout << "Error: 要删除的文件不存在" << endl;
        }
    } else if (param1 == "-d") { // param1: "-d" 删除单个空目录，"-r" 递归删除
        // 不是空或不是目录；是根目录；访问权限
        searchFile(param2);
        if (dir_curPath.fileId != -1) {
            inode toDeleteInode = dir_curPath; // 要删除文件的inode
            deleteAFolder(toDeleteInode);
        } else {
            cout << "Error: 要删除的文件不存在" << endl;
        }
    } else if (param1 == "-r") { // param1: "-r" 递归删除
        searchFile(param2);
        if (dir_curPath.fileId != -1) {
            inode toDeleteInode = dir_curPath; // 要删除文件的inode
            deleteAFolderRecursively(toDeleteInode);
        } else {
            cout << "Error: 要删除的文件不存在" << endl;
        }
    } else {
        cout << "Invalid argument!" << endl;
    }
}

void mv(string params) {
    string param1, param2, param3;
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex == string::npos) { // 若字符串中没有空格
        param1 = params;
        param2 = "";
    } else { // 若字符串中有空格，以第一个空格的位置将其分开
        param1 = params.substr(0, firstSpaceIndex - 0);
        param2 = params.substr(firstSpaceIndex + 1, params.length() - (firstSpaceIndex + 1));
    }
    cout << "param1: \"" << param1 << "\"\n";
    // cout << "param2: \"" << param2 << "\"\n";
    string ps = param2;
    int secondSpaceIndex = ps.find(' ');
    if (secondSpaceIndex == string::npos) { // 若字符串中没有空格
        param2 = ps;
        param3 = "";
    } else { // 若字符串中有空格，以第一个空格的位置将其分开
        param2 = ps.substr(0, secondSpaceIndex - 0);
        param3 = ps.substr(secondSpaceIndex + 1, ps.length() - (secondSpaceIndex + 1));
    }
    cout << "param2: \"" << param2 << "\"\n";
    cout << "param3: \"" << param3 << "\"\n";

    // 0. 判断输入合法
    if (param1 == "-r") { // 为文件或目录重命名
        // 获得要重命名文件的inode
        inode toRenameInode;
        vector<sfdItem> toRenameSFDList;
        searchFile(param2);
        if (dir_curPath.fileId != -1) {
            toRenameInode = dir_curPath; // 要删除文件的inode
            // toRenameSFDList = dir_curPathSeq;
            cout << "dir_curPath: " << dir_curPath.fileId << endl;
            cout << "要修改的：" << absPathStringFromSFDList(dir_curPathSeq) << endl;
            if (toRenameInode.fileId == root.fileId) {
                cout << "Error: 根目录不允许重命名" << endl;
                return;
            }
            if (dir_curPathSeq.size() > 0) {
                dir_curPathSeq.pop_back();
            }
        } else {
            cout << "Error: 要重命名的文件或目录不存在" << endl;
            return;
        }
        // 获得要重命名文件所在目录的inode
        string dirPath = absPathStringFromSFDList(dir_curPathSeq);
        searchFile(dirPath);
        inode toRenameDirInode = dir_curPath;
        cout << "所在目录：" << absPathStringFromSFDList(dir_curPathSeq) << endl;
        
        // 1. 查询新名字是否重名
        sfdItem test = searchSFD(toRenameDirInode, param3);
        if (test.fileId != -1) {
            cout << "Error: 新名字 \"" << param3 << "\" 与文件所在目录中其他文件或目录重名" << endl;
            return;
        }
        // 2. 更改文件所在目录的SFD表项
        sfdItem ret = getSFDItemByFileId(toRenameDirInode, toRenameInode.fileId);
        ret.filename = param3;
        cout << ret.fileId << " " << ret.filename << endl;
        updateSFD(toRenameDirInode, ret);
        cout << "重命名成功" << endl;
        
    } else { // 移动文件或目录
        // 获得要移动文件的inode
        inode toMoveInode;
        vector<sfdItem> toMoveSFDList;
        searchFile(param1);
        if (dir_curPath.fileId != -1) {
            toMoveInode = dir_curPath; // 要删除文件的inode
            // toMoveSFDList = dir_curPathSeq;
            if (toMoveInode.fileId == root.fileId) {
                cout << "Error: 根目录不允许移动" << endl;
                return;
            }
            if (dir_curPathSeq.size() > 0) {
                dir_curPathSeq.pop_back();
            }
        } else {
            cout << "Error: 要移动的文件或目录不存在" << endl;
            return;
        }
        // 获得要移动文件所在目录的inode
        string dirPath = absPathStringFromSFDList(dir_curPathSeq);
        searchFile(dirPath);
        inode toMoveDirInode = dir_curPath;

        // 获得目标目录的inode
        inode destDirInode;
        searchFile(param2);
        if (dir_curPath.fileId == -1) {
            if (!destDirInode.isDir) {
                cout << "Error: 目标路径不是目录" << endl;
                return;
            }
            destDirInode = dir_curPath; // 目标目录的inode
        } else {
            cout << "Error: 目标目录不存在" << endl;
            return;
        }

        // 1. 查询新名字是否重名
        // 获得被移动文件在源目录下的SFD表项
        sfdItem srcSFDItem = getSFDItemByFileId(toMoveDirInode, toMoveInode.fileId);
        sfdItem test = searchSFD(destDirInode, srcSFDItem.filename);
        if (test.fileId != -1) {
            cout << "Error: 文件名与目标目录中其他文件或目录重名" << endl;
            return;
        }
        // 2. 更改源目录的SFD表项
        deleteFromSFD(toMoveDirInode, srcSFDItem.fileId);
        // 3. 更改目标目录的SFD表项
        addToSFD(destDirInode, srcSFDItem);
        // 4. dirCnt--、++
        toMoveDirInode.dirCnt--;
        destDirInode.dirCnt++;
        updateInode(toMoveDirInode);
        updateInode(destDirInode);
        
        cout << "移动成功" << endl;
    }
}

