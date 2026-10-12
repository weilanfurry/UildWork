#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <conio.h>

using namespace std;

void setBackgroundColor(char colorCode) {
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

void setTextColor(char colorCode) {
    string colorCommand = "color ";

    switch(colorCode) {
        case '0': colorCommand += "F0"; break;
        case '1': colorCommand += "F1"; break;
        case '2': colorCommand += "F2"; break;
        case '3': colorCommand += "F3"; break;
        case '4': colorCommand += "F4"; break;
        case '5': colorCommand += "F5"; break;
        case '6': colorCommand += "F6"; break;
        case '7': colorCommand += "07"; break;
        case '8': colorCommand += "F8"; break;
        case '9': colorCommand += "F9"; break;
        case 'A': colorCommand += "1A"; break;
        case 'B': colorCommand += "1B"; break;
        case 'C': colorCommand += "1C"; break;
        case 'D': colorCommand += "1D"; break;
        case 'E': colorCommand += "1E"; break;
        case 'F': colorCommand += "1F"; break;
        default: colorCommand += "07"; break;
    }

    system(colorCommand.c_str());
}

int main() {
    cout << "欢迎来到UildWork图表统计！" << endl << endl;
    system("title UildWork Statistics");

    int objectCount, categoryCount;
    char templateChoice;

    cout << "表格模板" << endl;
    cout << "【1】绚丽背景" << endl;
    cout << "【2】多彩字体" << endl;
    cout << "【3】空白文档" << endl;
    cout << "【4】蓝底白字" << endl;
    cout << "【5】黑底绿字" << endl;
    cout << endl << "请选择你的模板: ";

    templateChoice = getch();
    cout << templateChoice << endl;

    if(templateChoice == '1') {
        char colorChoice;
        cout << endl;
        cout << "0=黑色, 1=蓝色, 2=绿色, 3=湖蓝, 4=红色, 5=紫色, 6=黄色, 7=白色, 8=灰色, 9=浅蓝" << endl;
        cout << "A=浅绿, B=青色, C=浅红, D=浅紫, E=浅黄, F=亮白" << endl << endl;
        cout << "请选择背景颜色: ";

        colorChoice = getch();
        cout << colorChoice;
        setBackgroundColor(colorChoice);
    } else if(templateChoice == '2') {
        char colorChoice;
        cout << endl;
        cout << "0=黑色, 1=蓝色, 2=绿色, 3=湖蓝, 4=红色, 5=紫色, 6=黄色, 7=白色, 8=灰色, 9=浅蓝" << endl;
        cout << "A=浅绿, B=青色, C=浅红, D=浅紫, E=浅黄, F=亮白" << endl << endl;
        cout << "请选择字体颜色: ";

        colorChoice = getch();
        cout << colorChoice;
        setTextColor(colorChoice);
    } else if(templateChoice == '4') {
        system("color 1F");
    } else if(templateChoice == '5') {
        system("color 02");
    }

    system("cls");
    cout << "请输入要统计的物体数量：";
    cin >> objectCount;

    string objects[objectCount];
    for(int i = 0; i < objectCount; i++) {
        cout << "请输入第" << i+1 << "个物体名称: ";
        cin >> objects[i];
    }

    cout << endl << "请输入统计的项目数量：";
    cin >> categoryCount;

    string categories[categoryCount];
    for(int i = 0; i < categoryCount; i++) {
        cout << "请输入第" << i+1 << "个项目名称: ";
        cin >> categories[i];
    }

    system("cls");
    float data[objectCount][categoryCount] = {0};

    while(true) {
        for(int i = 0; i < objectCount; i++) {
            cout << objects[i] << " ";
            for(int j = 0; j < categoryCount; j++) {
                cout << categories[j] << ": " << data[i][j] << " ";
            }
            cout << endl;
        }

        cout << endl << "目录" << endl;
        cout << "【1】编辑" << endl;
        cout << "【2】清空" << endl;
        cout << "【3】计算" << endl;
        cout << "【4】导出" << endl;
        cout << "【5】退出" << endl;

        char option;
        cout << endl << "请输入选项：";
        option = getch();
        cout << option;

        if(option == '1') {
            int row, column;
            float value;
            cout << endl << "请输入编辑数据的行数：";
            cin >> row;
            cout << "请输入编辑数据的列数：";
            cin >> column;
            cout << "请输入数据：";
            cin >> value;
            data[row-1][column-1] = value;
        } else if(option == '2') {
            for(int i = 0; i < objectCount; i++) {
                for(int j = 0; j < categoryCount; j++) {
                    data[i][j] = 0;
                }
            }
        } else if(option == '4') {
            ofstream outFile;
            outFile.open("Statistics.txt");

            for(int i = 0; i < objectCount; i++) {
                outFile << objects[i] << " ";
                for(int j = 0; j < categoryCount; j++) {
                    outFile << categories[j] << ": " << data[i][j] << " ";
                }
                outFile << endl;
            }

            outFile.close();
            cout << endl << "导出完成！请至软件安装目录查看Statistics.txt" << endl << endl;
            system("pause");
        } else if(option == '3') {
            cout << endl << endl;
            cout << "计算行请按1，计算列请按2" << endl;

            char calcType = getch();
            int index = 0;
            long long total = 0;
            double average = 0;

            if(calcType == '1') {
                cout << "请输入要计算的行数：";
                cin >> index;
                index--;

                for(int i = 0; i < categoryCount; i++) {
                    total += data[index][i];
                }
                average = static_cast<double>(total) / categoryCount;
            } else if(calcType == '2') {
                cout << "请输入要计算的列数：";
                cin >> index;
                index--;

                for(int i = 0; i < objectCount; i++) {
                    total += data[i][index];
                }
                average = static_cast<double>(total) / objectCount;
            }

            system("cls");
            cout << "计算结果：" << endl;
            cout << "总数：" << total << endl;
            cout << "平均：" << average << endl;
            cout << endl;
            system("pause");
        } else if(option == '5') {
            return 0;
        }

        system("cls");
    }

    return 0;
}
