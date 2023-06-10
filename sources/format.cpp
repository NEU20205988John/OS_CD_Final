#include <bits/stdc++.h>
#include "filesys_ds.h"
#include "filesys_func.h"
using namespace std;

void initSystem() {
    ifstream ifs("../blocks/0");
    if (ifs.is_open()) {
        cout << "/blocks/0 opened successfully" << endl;
    } else{
        cout << "/blocks/0 open failed" <<endl;
    }
    // 1. 读入超级块
    ifs >> spb.freeInodeCnt >> spb.freeBlockCnt >> spb.freeStackPointer;
    for (int i = 0; i < spb.freeStackPointer; i++) {
        ifs >> spb.freeStack[i];
    }
    // 2. 读入用户列表并创建用户打开文件表, 初始化打开文件表
    int ul;
    ifs >> ul;
    for (int i = 0; i < ul; i++) {
        user tmp;
        ifs >> tmp.username >> tmp.password;
        userList.push_back(tmp);
        // 为每个用户创建用户打开文件表
        singleUserTable newUserTable;
        newUserTable.username = tmp.username;
        newUserTable.singleTableCnt = 0;
        for (int i = 0; i < MAX_NUM_OF_OPENED_FILE_PER_USER; i++) {
            newUserTable.singleTable[i].isOccupied = false;
        }
        for (int i = 0; i < MAX_NUM_OF_OPENED_FILE; i++) {
            systemOpenTable[i].isOccupied= false;
        }
        userOpenTable.push_back(newUserTable);
    }


    // 3. 保存根目录的inode
    ifs >> root.fileId;
    ifs >> root.BlockCnt;
    ifs >> root.isDir;
    ifs >> root.dirCnt;
    for (int j = 0; j < 10; j++){//直接寻址
        ifs >> root.physicalAddr.direct[j];
    }
    ifs >> root.physicalAddr.indirect;//间接寻址
    ifs >> root.ownerName;
    ifs >> root.accessTable.owner.canRead;
    ifs >> root.accessTable.owner.canWrite;
    ifs >> root.accessTable.owner.canExecute;
    ifs >> root.accessTable.owner.canDelete;
    ifs >> root.accessTable.owner.canChangePermit;
    ifs >> root.accessTable.nonOwner.canRead;
    ifs >> root.accessTable.nonOwner.canWrite;
    ifs >> root.accessTable.nonOwner.canExecute;
    ifs >> root.accessTable.nonOwner.canDelete;
    ifs >> root.accessTable.nonOwner.canChangePermit;
    ifs >> root.linkCnt;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）

    curPath = root;
    // curPathSeq;

    ifs.close();
}

void saveSystem() {
    ofstream ofs("../blocks/0");
    if (ofs.is_open()) {
        cout << "/blocks/0 opened successfully" << endl;
    } else{
        cout << "/blocks/0 open failed" <<endl;
    }
    // 1. 保存超级块
    ofs << spb.freeInodeCnt << endl << spb.freeBlockCnt << endl << spb.freeStackPointer << endl;
    for (int i = 0; i < spb.freeStackPointer; i++) {
        ofs << spb.freeStack[i] <<endl;
    }
    // 2. 保存用户列表
    ofs << userList.size() << endl;
    for (int i = 0; i < userList.size(); i++) {
        ofs << userList[i].username << " " << userList[i].password << endl;
    }
    // 3. 保存根目录的inode
    ofs << root.fileId << " ";
    ofs << root.BlockCnt << " ";
    ofs << root.isDir << " ";
    ofs << root.dirCnt << " ";
    for (int j = 0; j < 10; j++){//直接寻址
        ofs << root.physicalAddr.direct[j] << " ";
    }
    ofs << root.physicalAddr.indirect << " ";//间接寻址
    ofs << root.ownerName << " ";
    ofs << root.accessTable.owner.canRead << " ";
    ofs << root.accessTable.owner.canWrite << " ";
    ofs << root.accessTable.owner.canExecute << " ";
    ofs << root.accessTable.owner.canDelete << " ";
    ofs << root.accessTable.owner.canChangePermit << " ";
    ofs << root.accessTable.nonOwner.canRead << " ";
    ofs << root.accessTable.nonOwner.canWrite << " ";
    ofs << root.accessTable.nonOwner.canExecute << " ";
    ofs << root.accessTable.nonOwner.canDelete << " ";
    ofs << root.accessTable.nonOwner.canChangePermit << " ";
    ofs << root.linkCnt << endl;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）

    ofs.flush(); // 立即输出到文件
    ofs.close();
}

void format() {
    leaderBlock leaderblock;
    
    //初始化磁盘块区，根据成组链接法组织
    int num_left;
    int cur_id;
    for(int i = LEADER_BLOCK_NUM; i>=1; i--) {
        leaderblock.S[1] = BLOCK_NUM - (LEADER_BLOCK_NUM-i)*BLOCK_GROUP_SIZE;//指向下一个空闲块
        leaderblock.S[0] = BLOCK_GROUP_SIZE;//这些组长块能填满，均为50个
        for(int j=2; j<=BLOCK_GROUP_SIZE; j++){//2-50 49个
            leaderblock.S[j] = BLOCK_NUM-(LEADER_BLOCK_NUM-i+1)*BLOCK_GROUP_SIZE+j - 1;
        }
        if(i==LEADER_BLOCK_NUM){
            leaderblock.S[1]=-1;
        }
        cur_id = BLOCK_NUM-(LEADER_BLOCK_NUM-i+1)*BLOCK_GROUP_SIZE ;
        ofstream ofs("../blocks/"+to_string(BLOCK_NUM-(LEADER_BLOCK_NUM-i+1)*BLOCK_GROUP_SIZE));
        if (ofs.is_open()) {
            cout << "/blocks/" << cur_id << " opened successfully" << endl;
        } else {
            cout << "/blocks/" << cur_id << " open failed" << endl;
        }
        for(int i=0;i<=BLOCK_GROUP_SIZE;i++){
            ofs<<leaderblock.S[i]<<endl;
        }
        ofs.close();
    }

    // 设置超级块
    // cout<<cur_id<<endl;
    num_left =  (cur_id-1)- (NUM_OF_RESERVED_BLOCK-1);
    // cout<<num_left<<endl;
    spb.freeStackPointer = num_left+1;
    spb.freeStack[0] = cur_id;
    for(int i=1;i<=num_left;i++){
        spb.freeStack[i] = NUM_OF_RESERVED_BLOCK+i-1;
        // cout<<" "<<spb.freeStack[i]<<endl;
    }
    spb.freeInodeCnt = MAX_INODE_NUM;
    spb.freeBlockCnt = BLOCK_NUM - NUM_OF_RESERVED_BLOCK;

    // 向每个BFD保留块中写入一个字符0
    for(int i = 1; i < NUM_OF_RESERVED_BLOCK; i++){
        ofstream ofs2("../blocks/"+to_string(i));
        ofs2 << "0" << endl;
    }

    // 建立根目录
    vector<inode>::iterator newFileIter;
    int newFileId = findAvaliableInode(newFileIter); // 在磁盘的inode表中查找一个可用的inode编号
    inode newInode = createNewInode(newFileId, "admin", true, 1);
    root = newInode;
    curPath = newInode; // 设置当前路径为根目录
    curPathSeq.clear(); // 清空绝对路径序列
    createNewInodeSFD(newInode); // 对于根目录，"." 与 ".."的BFD编号相同
    curInodeBlock.insert(newFileIter, newInode); // 在可用的inode编号处插入inode
    writeInodeBlock(curInodeBlock, curInodeBlockNum); // 将含有inode的磁盘块写回

    // 建立用户列表
    // userList.clear();
    // userList.push_back(user("admin", "123456"));

    // 保存系统状态
    saveSystem();

    // inode _1st_inode;
    // _1st_inode.fileId=0;//fileId=0代表这是根目录,SFD中索引为0
    // _1st_inode.BlockCnt = 1;//先分配一个
    // _1st_inode.isDir = true;//根目录
    // _1st_inode.dirCnt = 0;//初始化时为空
    // _1st_inode.physicalAddr.direct[0] = allocate_block();
    // for(int j=1;j<10;j++){
    //     _1st_inode.physicalAddr.direct[j]=-1;
    // }
    // _1st_inode.physicalAddr.indirect=-1;//不使用间接寻址
    // _1st_inode.ownerName = "admin";
    // permitCtrl pc1(true,true,true,true,true);
    // permitCtrl pc2(false,false,false,false,false);
    // _1st_inode.accessTable = accessCtrl(pc1,pc2);
    // _1st_inode.linkCnt = 1;//一开始的时候只有一个SFD 链接计数（有多少SFD表项链接到这个inode）

    // sfdItem sfd1;
    // sfd1.filename=".";//指向当前目录的BFD的指针应当是 "."
    // sfd1.fileId=0;//根目录，指向自己

    // sfdItem sfd2;
    // sfd2.filename="..";//指向当前目录的BFD的指针应当是 "."
    // sfd2.fileId=0;//根目录，指向自己
    
}