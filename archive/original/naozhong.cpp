#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>

using namespace std;

int main() {
    cout << "欢迎来到UildWork小闹钟！" << endl << endl;

    int alarmHour, alarmMinute, alarmSecond;
    cout << "输入小时：";
    cin >> alarmHour;
    cout << "输入分钟：";
    cin >> alarmMinute;
    cout << "输入秒：";
    cin >> alarmSecond;

    system("cls");
    cout << "UildWork铃声合集：" << endl << endl;
    cout << "【1】经典铃声" << endl;
    cout << "【2】Walking the wire" << endl;
    cout << "【3】Battle symphony" << endl << endl;
    cout << "请选择铃声：";

    char ringtoneChoice = getch();
    cout << ringtoneChoice << endl << endl;
    cout << "单击s键开启闹钟";

    char startKey = 'n';
    while(startKey != 's') {
        startKey = getch();
    }

    system("cls");

    while(true) {
        SYSTEMTIME currentTime;
        GetLocalTime(&currentTime);

        printf("时钟时间：%02d时%02d分%02d秒\n\n",currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
        cout << "您的闹钟：" << alarmHour << "时" << alarmMinute << "分" << alarmSecond << "秒";

        Sleep(1000);
        system("cls");

        if(currentTime.wHour == alarmHour &&
           currentTime.wMinute == alarmMinute &&
           currentTime.wSecond == alarmSecond) {
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
    }

    return 0;
}
