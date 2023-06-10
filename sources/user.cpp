#include "filesys_ds.h"
#include "filesys_func.h"

#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

bool login()
{
    string name, passwd;
    cout << "请输入用户名：";
    cin >> name;
    cout << endl;
    cout << "请输入密码:";
    cin >> passwd;
    cout << endl;
    bool existflag = false; // 标记输入的用户名是否存在
    for (int i = 0; i < userList.size(); i++)
    {
        if (name == userList[i].username)
        {
            existflag = true;
            if (passwd == userList[i].password)
            {
                isLoggedIn = true;     // 标记登录Flag
                curUser = userList[i]; // 给当前登录的用户赋值
                cout << "登录成功" << endl;
                return true;
            }
            else
            {
                cout << "密码错误";
                return false;
            }
        }
    } // 匹配用户名及密码

    if (!existflag)
    {
        cout << "用户不存在" << endl;
        return false;
    }
    return true;
} // 用户登入

void logout()
{
    curUser = anonymous; // 将当前登录用户恢复默认
    isLoggedIn = false;  // 登录Flag恢复默认
    cout << "Logged out" << endl;
} // 用户登出

bool useradd()
{
    user newuser;
    string name, passwd;
    bool existname = true; // 判断用户名是否存在

    cout << "请输入用户名：";
    cin >> name;
    while (existname) {
        if (name == "") {
            cout << "输入为空，终止创建" << endl;
            return false;
        }
        for (int i = 0; i < userList.size(); i++) {
            if (name == userList[i].username) {
                bool existname = true;
                cout << "用户名已存在，请重新输入：";
                cin >> name;
                break;
            } else
                existname = false;
        }
    }

    cout << endl;
    cout << "请输入密码:";
    cin >> passwd;
    newuser.username = name;
    newuser.password = passwd;
    userList.push_back(newuser);

    //创建用户打开文件表
    singleUserTable newUserTable;
    newUserTable.username = name;
    newUserTable.singleTableCnt = 0;
    userOpenTable.push_back(newUserTable);

    cout << endl << "创建成功！" << endl;
    return true;
} // 创建用户

bool userdel()
{
    string name, passwd;
    cout << "请输入要删除的用户名：";
    cin >> name;
    cout << endl;
    cout << "请输入密码:";
    cin >> passwd;
    cout << endl;
    bool existflag = false; // 标记输入的用户名是否存在
    vector<user>::iterator itor;
    for (itor = userList.begin(); itor != userList.end(); itor++)
    {
        if (name == itor->username)
        {
            existflag = true;
            if (passwd == itor->password)
            {
                // 删除该用户的打开文件表
                for (auto i = userOpenTable.begin(); i < userOpenTable.end(); i++) {
                    if (i->username == itor->username) {
                        userOpenTable.erase(i);
                        break;
                    }
                }
                singleUserTable newUserTable;
                newUserTable.username = name;
                newUserTable.singleTableCnt = 0;
                userOpenTable.push_back(newUserTable);

                userList.erase(itor); // 删除
                cout << "删除成功" << endl;
                return true;
            }
            else
            {
                cout << "密码错误,不可执行删除" << endl;
                return false;
            }
        }
    } // 匹配用户名及密码

    if (!existflag)
    {
        cout << "所要删除的用户不存在" << endl;
        return false;
    }
    return true;
} // 删除用户