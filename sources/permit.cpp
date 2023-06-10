#include <bits/stdc++.h>
#include "filesys_ds.h"
#include "filesys_func.h"
using namespace std;

void editPermit(string params) {
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex != string::npos) { // 若输入参数字符串中有空格，则认为是不合法文件名
        cout << "Invalid arguments!" << endl;
        return;
    } else {
        searchFile(params);
        if (dir_curPath.fileId == -1) {
            cout << "Error: 文件或目录不存在" << endl;
            return;
        }

        inode toEditInode = dir_curPath;
        // 访问权限控制
        if (toEditInode.ownerName == curUser.username) {
            if (!toEditInode.accessTable.owner.canChangePermit) {
                cout << "Error: 当前用户无更改权限" << endl;
                return;
            }
        } else {
            if (!toEditInode.accessTable.nonOwner.canChangePermit) {
                cout << "Error: 当前用户无更改权限" << endl;
                return;
            }
        }
        cout << "请输入所有者的权限：\"canRead canWrite canExecute canDelete canChangePermit\"" << endl;
        cin >> toEditInode.accessTable.owner.canRead
            >> toEditInode.accessTable.owner.canWrite
            >> toEditInode.accessTable.owner.canExecute
            >> toEditInode.accessTable.owner.canDelete
            >> toEditInode.accessTable.owner.canChangePermit;
        cout << "请输入非所有者的权限：\"canRead canWrite canExecute canDelete canChangePermit\"" << endl;
        cin >> toEditInode.accessTable.nonOwner.canRead
            >> toEditInode.accessTable.nonOwner.canWrite
            >> toEditInode.accessTable.nonOwner.canExecute
            >> toEditInode.accessTable.nonOwner.canDelete
            >> toEditInode.accessTable.nonOwner.canChangePermit;
        
        updateInode(toEditInode);
        cout << "权限已修改" << endl;
    }
}

void viewPermit(string params) {
    int firstSpaceIndex = params.find(' ');
    if (firstSpaceIndex != string::npos) { // 若输入参数字符串中有空格，则认为是不合法文件名
        cout << "Invalid arguments!" << endl;
        return;
    } else {
        searchFile(params);
        if (dir_curPath.fileId == -1) {
            cout << "Error: 文件或目录不存在" << endl;
            return;
        }

        inode toViewInode = dir_curPath;
        cout << "   权限   r w e d c" << endl;
        cout << "  所有者："
            << toViewInode.accessTable.owner.canRead << " "
            << toViewInode.accessTable.owner.canWrite << " "
            << toViewInode.accessTable.owner.canExecute << " "
            << toViewInode.accessTable.owner.canDelete << " "
            << toViewInode.accessTable.owner.canChangePermit << endl;
        cout << "非所有者："
            << toViewInode.accessTable.nonOwner.canRead << " "
            << toViewInode.accessTable.nonOwner.canWrite << " "
            << toViewInode.accessTable.nonOwner.canExecute << " "
            << toViewInode.accessTable.nonOwner.canDelete << " "
            << toViewInode.accessTable.nonOwner.canChangePermit << endl;
    }
}