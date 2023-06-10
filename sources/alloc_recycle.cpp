#include <bits/stdc++.h>
#include "filesys_ds.h"
#include "filesys_func.h"
using namespace std;

leaderBlock ReadABlock(int ret){
    leaderBlock block;
    ifstream ofs("./blocks/"+to_string(ret));//仅供测试
    for(int i=0;i<=BLOCK_GROUP_SIZE;i++){//0 - 50
        ofs>>block.S[i];
    }
    return block;
}

void WriteABlock(superBlock spb,int BlockNo){
    ofstream ofs("./blocks/"+to_string(BlockNo));
    ofs<<spb.freeStackPointer<<endl;
    for(int i=0;i<spb.freeStackPointer;i++){
        ofs<<spb.freeStack[i]<<endl;
    }
}


int allocate_block(){
    int ret;
    if(spb.freeStackPointer==1){//当前超级块已无空闲块，读取组长块
        if(spb.freeStack[0]==-1){
            cout << "分配失败，进程等待" << endl;
            return -1;
        }
        else{
            ret = spb.freeStack[0];//得到组长块的地址
            leaderBlock leaderblock= ReadABlock(ret);
            spb.freeStackPointer = leaderblock.S[0];
            for(int i=0;i<leaderblock.S[0];i++){
                spb.freeStack[i] = leaderblock.S[i+2];
            }
            return ret;
        }
    }
    else{
        // cout<<"spb.freeStack[0] = "<<spb.freeStack[0]<<endl;
        // cout << spb.freeStackPointer << endl;
        // for (int i = 0; i < sizeof(spb.freeStack); i++) {
        //     cout << spb.freeStack[i] << endl;
        // }
        
        ret = spb.freeStack[spb.freeStackPointer - 1]; 
        cout << "allocated block: " << ret << endl;
        spb.freeStackPointer--;
        return ret;
    }
}

void FreeABlock(int BlockNo){
    if(spb.freeStackPointer==BLOCK_GROUP_SIZE){
        WriteABlock(spb,BlockNo);
        spb.freeStackPointer = 1;
        spb.freeStack[0] = BlockNo;
    }
    else{
        spb.freeStack[spb.freeStackPointer] = BlockNo;
        spb.freeStackPointer++;
    }
}

// int main(){
//     ifstream ofs("./blocks/0");//仅供测试
//     ofs>>spb.freeInodeCnt>>spb.freeBlockCnt>>spb.freeStackPointer;//读入超级块
//     for(int i=0;i<=spb.freeStackPointer;i++){
//         ofs>>spb.freeStack[i];
//     }
//     int alloc = allocate_block();
//     cout<<spb.freeStackPointer<<endl;
//     cout<<"alloc = "<<alloc<<endl;
//     FreeABlock(alloc);
//     //  cout<<spb.freeStackPointer<<endl;



//     return 0;
// }