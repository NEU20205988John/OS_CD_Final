// #ifndef __FILESYS_FUNC
// #define __FILESYS_FUNC

#include "filesys_ds.h"

extern superBlock spb;
extern int curPathId; // 当前目录的inode在BFD中的索引
extern inode curPath; // 当前目录的inode
extern vector<sfdItem> curPathSeq; // 当前目录的绝对路径序列
extern std::string curPathString; // 绝对路径字符串
extern inode root; // 根目录的inode

extern inode dir_curPath; // search过后目录的inode
extern vector<sfdItem> dir_curPathSeq; // search过后目录的绝对路径序列

extern std::vector<user> userList; // 所有用户列表
extern std::vector<singleUserTable> userOpenTable; // 所有的用户打开文件表集合
extern int systemOpenTableCnt; // 系统打开文件表的栈指针
extern systemOpenItem systemOpenTable[MAX_NUM_OF_OPENED_FILE]; // 系统打开文件表

extern user anonymous;
extern bool isLoggedIn; // 是否已登录 Flag
extern user curUser; // 当前登录的用户

extern void format();
extern int allocate_block();
extern void FreeABlock(int BlockNo);

extern void editPermit(string params);
extern void viewPermit(string params);

extern vector<inode> readInodeBlock(int No);
extern void writeInodeBlock(vector<inode> &vc,int No);
extern vector<sfdItem> readSFDBlock(int blockNo);
extern void writeSFDBlock(vector<sfdItem> &vs, int blockNo);
extern int findAvaliableInode(vector<inode>::iterator &it);
extern inode createNewInode(int fileId, string ownerName, bool isDir, int blockCnt);
extern void createNewInodeSFD(inode &inode);
extern sfdItem searchSFD(inode path, string filename);
extern void searchFile(string filename);

extern vector<inode> curInodeBlock; // 当前读入到内存的inode磁盘块vector
extern int curInodeBlockNum; // 当前读入到内存的inode磁盘块号

extern void initSystem();
extern void saveSystem();

extern bool login();
extern void logout();
extern bool useradd();
extern bool userdel();

extern void cd(string filename);
extern void ls();
extern void mkdir(string params);
extern void touch(string params);
extern void rm(string params);
extern void mv(string params);

extern bool open(string openstr);
extern bool close();
extern void write();
extern void read();
extern bool useropened();
extern bool systemopened();

extern void updateInode(inode &u);

// #endif
