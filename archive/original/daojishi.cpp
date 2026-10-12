#include <Windows.h>
#include <iostream>
#include <conio.h>

using namespace std;

int main() {
    cout << "欢迎来到UildWork倒计时！" << endl << endl;

    int hours = 0, minutes = 0, seconds = 0;
    char menuChoice;

    cout << "目录" << endl;
    cout << "【1】1小时" << endl;
    cout << "【2】30分钟" << endl;
    cout << "【3】10分钟" << endl;
    cout << "【4】1分钟" << endl;
    cout << "【5】自定义" << endl;
    cout << endl << "请输入选项：";

    menuChoice = getch();
    cout << menuChoice << endl;

    if(menuChoice == '5') {
        system("cls");
        cout << "输入小时：";
        cin >> hours;
        cout << "输入分钟：";
        cin >> minutes;
        cout << "输入秒：";
        cin >> seconds;
    } else if(menuChoice == '1') {
        hours = 1;
    } else if(menuChoice == '2') {
        minutes = 30;
    } else if(menuChoice == '3') {
        minutes = 10;
    } else if(menuChoice == '4') {
        minutes = 1;
    }

    system("cls");
    cout << "UildWork铃声合集：" << endl << endl;
    cout << "【1】经典铃声" << endl;
    cout << "【2】Walking the wire" << endl;
    cout << "【3】Battle symphony" << endl << endl;
    cout << "请选择铃声：";

    char ringtoneChoice = getch();
    cout << ringtoneChoice << endl << endl;
    cout << "单击s键开始计时";

    char startKey = 'n';
    while(startKey != 's') {
        startKey = getch();
    }

    system("cls");

    while(true) {
        cout << endl;
        cout << hours << "时" << minutes << "分" << seconds << "秒" << endl << endl;

        Sleep(1000);
        seconds--;

        if(seconds == -1) {
            seconds = 59;
            minutes--;
        }

        if(minutes == -1) {
            minutes = 59;
            hours--;
        }

        if(hours == -1) {
            system("cls");
            system("color 47");
            cout << endl << "计时已结束！";

            if(ringtoneChoice == '1') {
                while(true) {
                    Beep(880, 500);
                }
            } else if(ringtoneChoice == '2') {
                system("start music1.vbs");
                cout << endl;
                system("echo 按任意键结束铃声！&pause>nul");
                system("taskkill /f /t /im wscript.exe");
                return 0;
            } else if(ringtoneChoice == '3') {
                system("start music2.vbs");
                cout << endl;
                system("echo 按任意键结束铃声！&pause>nul");
                system("taskkill /f /t /im wscript.exe");
                return 0;
            }
        }

        system("cls");
    }

    return 0;
}
