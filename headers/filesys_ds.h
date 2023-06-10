#ifndef __FILESYS_DS
#define __FILESYS_DS

#include <string>
#include <vector>
using namespace std;

 // 模拟的磁盘块每个大小：1KB
#define DISK_SIZE (1 << 23) //磁盘总容量（字节数）
#define BLOCK_NUM_BITS 13 // 每个磁盘块号所占的位数
#define NUM_OF_DIRECT_ADDR 10 //每个文件直接寻址的磁盘块数

#define BLOCK_NUM (1 << BLOCK_NUM_BITS) // 系统磁盘块总数
#define BLOCK_SIZE (DISK_SIZE / BLOCK_NUM) // 每个磁盘块的大小（字节数）
#define ADDR_PER_BLOCK ((BLOCK_SIZE << 3) / BLOCK_NUM_BITS) // 一个磁盘块中最多能装入的磁盘块号数（每个磁盘块号13位）
#define MAX_FILE_SIZE ((NUM_OF_DIRECT_ADDR + ADDR_PER_BLOCK) * BLOCK_SIZE) // 单个文件的最大大小（字节数）

#define INODE_PER_BLOCK 64 // 每个磁盘块中能存放的inode信息个数
#define NUM_OF_RESERVED_BLOCK 501 // 系统保留的专用于存放超级块（0号磁盘块）和i节点的磁盘块数 0~500
// 501号块开始为数据区
#define MAX_INODE_NUM (INODE_PER_BLOCK * (NUM_OF_RESERVED_BLOCK - 1)) // 最大i节点数（最大文件个数）

#define BLOCK_GROUP_SIZE 50 // 成组链接法中每组的磁盘块数

#define MAX_NUM_OF_OPENED_FILE 200 // 最大可打开文件数
#define MAX_NUM_OF_OPENED_FILE_PER_USER 100 // 单个用户最大可打开文件数

#define LEADER_BLOCK_NUM ((BLOCK_NUM-NUM_OF_RESERVED_BLOCK)/BLOCK_GROUP_SIZE) // 组长块的个数

// #define df BLOCK_NUM - NUM_OF_RESERVED_BLOCK

class dblock { // 模拟的磁盘块
public:
    dblock() {}
    int blockNo; // 磁盘块号
    bool inUse; // 磁盘块是否正在使用的标识
private:
    std::string blockFileName; // 磁盘块的内容实际存放在./blocks/中的文件名
} ;

// std::vector<dblock> blockList; // 磁盘块列表

class permitCtrl { // 权限控制项类型
public:
    bool canRead;
    bool canWrite;
    bool canExecute;
    bool canDelete;
    bool canChangePermit;
    permitCtrl(){}
    permitCtrl(bool canRead, bool canWrite, bool canExecute, bool canDelete,bool canChangePermit){
        this->canRead = canRead;
        this->canWrite = canWrite;
        this->canExecute = canExecute;
        this->canDelete = canDelete;
        this->canChangePermit = canChangePermit;
    }
    permitCtrl(const permitCtrl& permitctrl){
        this->canRead = permitctrl.canRead;
        this->canWrite = permitctrl.canWrite;
        this->canExecute = permitctrl.canExecute;
        this->canDelete = permitctrl.canDelete;
        this->canChangePermit = permitctrl.canChangePermit;
    }
} ;

class accessCtrl { // 访问控制表类型
public:
    permitCtrl owner;
    permitCtrl nonOwner;
    accessCtrl(){}
    accessCtrl(permitCtrl owner, permitCtrl nonOwner)
     : owner(owner), nonOwner(nonOwner) {
    }
    accessCtrl(const accessCtrl &u) 
     : owner(u.owner), nonOwner(u.nonOwner) {
    }
} ;

class physicalAddress { // 文件的磁盘物理地址（用int模拟）
public:
    physicalAddress() {}
    physicalAddress(const physicalAddress &u) {
        for (int i = 0; i < NUM_OF_DIRECT_ADDR; i++) {
            this->direct[i] = u.direct[i];
        }
        this->indirect = u.indirect;
    }
    int direct[NUM_OF_DIRECT_ADDR]; //直接寻址的NUM_OF_DIRECT_ADDR个块号
    int indirect; //一次间址的块号，若不用间址，值设为-1
} ;

class inode { // BFD表项类型
public:
    inode() {}
    inode(const inode &u) 
     : physicalAddr(u.physicalAddr), ownerName(u.ownerName), accessTable(u.accessTable) {
        this->fileId = u.fileId;
        this->BlockCnt = u.BlockCnt;
        this->isDir = u.isDir;
        this->dirCnt = u.dirCnt;
        this->linkCnt = u.linkCnt;
    }
    // 磁盘索引节点内容
    int fileId; // 文件的唯一标识符，填在SFD表项中的索引
    // int byteCnt; // 文件占用的磁盘块个数，用于计算所占磁盘块数
    int BlockCnt; // 文件的实际占用空间,磁盘块数
    bool isDir; // 是否是目录文件
    int dirCnt; // 若为目录，这个目录中有多少文件（SFD有多少项）
    physicalAddress physicalAddr; //磁盘块的物理地址
    std::string ownerName; // 所有者的userId
    accessCtrl accessTable; // 访问控制表，实现为所有者和非所有者两类控制
    // int createTime; // 创建时间 #include<time.h> time_t result = time(NULL);
    // int modifyTime; // 修改时间
    int linkCnt; // 链接计数（有多少SFD表项链接到这个inode）
// private:
//     std::string blockFileName; // 磁盘块的内容实际存放在./blocks/中的文件名
} ;

class sfdItem { // SFD表项类型
public:
    sfdItem() {}
    sfdItem(string filename, int fileId)
     : filename(filename) {
        this->fileId = fileId;
    }
    sfdItem(const sfdItem &u)
     : filename(u.filename) {
        this->fileId = u.fileId;
    }
    std::string filename; // 文件名
    int fileId; // 文件的标识符，用于查找BFD
} ;

class systemOpenItem { // 系统打开文件表表项
public:
    systemOpenItem() {}
    // std::string absPath; // 文件的绝对路径（该表项不用时置为空串？）
    
    bool isOccupied; // 标记数组中这个位置是否被占用，关闭文件时置为false
    inode FCB; // 该文件的inode
    // 内存索引节点增加的内容
    int openCnt; // 访问计数（这个inode对应的文件被打开多少次）
    int modifiedFlag; // 修改标志，指示i节点是否上锁或被修改
} ;

class userOpenItem { // 用户打开文件表表项
public:
    userOpenItem() {}
    bool isOccupied; // 标记数组中这个位置是否被占用，关闭文件时置为false
    long long fd; // 文件描述符 - 实现为打开时的时间戳
    int mode; // 打开方式 1：只读 2. 只写 3. 可读可写
    int rwpointer; // 读写指针（byte offset），刚打开文件时默认为0
    int systemTableIdx; // 系统打开文件表入口
} ;

// 输入：long long fd -> std::vector<singleUserTable> userOpenTable 
//全局变量curUser.username和singleUserTable::username匹配
//找对对应的这张表，进入后匹配fd
//rwpointer、写入的内容 均提示用户键盘输入
//rwpointer / BLOCK_SIZE 得到 向pyhsicalAddress中的第几个块写入，直接打开块写入，若rwpointer / BLOCK_SIZE超过BlockCnt则报错
//访问权限控制：1.文件本身的accessTable  2.用户打开文件表里的mode
// 写入成功输出提示信息
//
class singleUserTable { // 单个用户打开文件表
public:
    singleUserTable() {}
    string username; // 此用户打开文件表的username
    int singleTableCnt = 0; // 用户打开文件表的被占用单元数
    userOpenItem singleTable[MAX_NUM_OF_OPENED_FILE_PER_USER]; // 用户打开文件表，实现为每个用户一张
} ;

class superBlock {
public:
    superBlock() {}
    int freeInodeCnt;		// 空闲inode节点数
    int freeBlockCnt;		// 空闲磁盘块数
    int freeStackPointer;		// 空闲块堆栈指针
    int freeStack[BLOCK_GROUP_SIZE];	// 空闲块堆栈
} ;

class leaderBlock {
public:
    leaderBlock() {}
    int S[BLOCK_GROUP_SIZE + 1];	// 空闲块块号栈
} ;

class user { // 用户类型
public:
    user() {}
    user(std::string username, std::string password)
     : username(username), password(password) {
    }
    // int userId;
    std::string username;
    std::string password;
} ;

#endif
