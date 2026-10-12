#include <iostream>
#include <string>
#include <stdlib.h>
#include <conio.h>

using namespace std;

void setConsoleColor(char colorCode) {
    string colorCommand = "color ";

    switch(colorCode) {
        case '0': colorCommand += "0F"; break;
        case '1': colorCommand += "1F"; break;
        case '2': colorCommand += "2F"; break;
        case '3': colorCommand += "3F"; break;
        case '4': colorCommand += "4F"; break;
        case '5': colorCommand += "5F"; break;
        case '6': colorCommand += "6F"; break;
        case '7': colorCommand += "70"; break;
        case '8': colorCommand += "8F"; break;
        case '9': colorCommand += "9F"; break;
        case 'A': colorCommand += "A1"; break;
        case 'B': colorCommand += "B1"; break;
        case 'C': colorCommand += "C1"; break;
        case 'D': colorCommand += "D1"; break;
        case 'E': colorCommand += "E1"; break;
        case 'F': colorCommand += "F1"; break;
        default: colorCommand += "07"; break;
    }

    system(colorCommand.c_str());
}

int main() {
    int itemCount;
    string listName;

    system("title UildWork Task List");
    cout << "欢迎来到UildWork任务清单！" << endl << endl;

    // 颜色选择
    cout << "0=黑色, 1=蓝色, 2=绿色, 3=湖蓝, 4=红色, 5=紫色, 6=黄色, 7=白色, 8=灰色, 9=浅蓝" << endl;
    cout << "A=浅绿, B=青色, C=浅红, D=浅紫, E=浅黄, F=亮白" << endl << endl;
    cout << "请选择背景颜色: ";

    char colorChoice = getch();
    cout << colorChoice;
    setConsoleColor(colorChoice);

    system("cls");
    cout << "您有多少个事项？";
    cin >> itemCount;
    cout << endl;

    string items[itemCount];
    int completionStatus[itemCount] = {0};

    cout << "命名你的清单：";
    cin >> listName;
    cout << endl;

    for(int i = 0; i < itemCount; i++) {
        cout << "第" << i+1 << "项: ";
        cin >> items[i];
    }

    while(true) {
        system("cls");
        cout << listName << endl << endl;

        for(int i = 0; i < itemCount; i++) {
            cout << "第" << i+1 << "项: " << items[i];
            cout << (completionStatus[i] == 0 ? " 未完成" : " 已完成") << endl;
        }

        cout << endl;
        cout << "【1】完成" << endl;
        cout << "【2】编辑" << endl;
        cout << "【3】退出" << endl;
        cout << endl << "请输入选项：";

        char option = getch();
        cout << option << endl;

        if(option == '1') {
            int itemIndex;
            cout << "您完成了第几项：";
            cin >> itemIndex;
            completionStatus[itemIndex-1] = 1;
        } else if(option == '2') {
            int itemIndex;
            string newContent;
            cout << "您要编辑第几项：";
            cin >> itemIndex;
            cout << endl << "请输入您要编辑的内容：";
            cin >> newContent;
            items[itemIndex-1] = newContent;
        } else if(option == '3') {
            return 0;
        }
    }

    return 0;
}
