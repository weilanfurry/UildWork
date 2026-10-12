#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>

using namespace std;

int main() {
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);

    if(currentTime.wHour >= 18) {
        MessageBox(NULL, "晚上好，今天也要早睡哦，祝你身体健康！", "UildWork", MB_OK);
    } else if(currentTime.wHour >= 12) {
        MessageBox(NULL, "下午好，快享受美妙的阳光吧，祝你开开心心！", "UildWork", MB_OK);
    } else if(currentTime.wHour >= 6) {
        MessageBox(NULL, "上午好，今天又会有什么样的好运呢？祝你一帆风顺！", "UildWork", MB_OK);
    } else {
        MessageBox(NULL, "凌晨好，美好的一天又开始了，祝你事业有成！", "UildWork", MB_OK);
    }

    while(true) {
        cout << "欢迎来到UildWork智能秒表！" << endl << endl;
        printf("您的时间：%04d/%02d/%02d %02d:%02d\n",currentTime.wYear, currentTime.wMonth, currentTime.wDay,currentTime.wHour, currentTime.wMinute);

        cout << endl << "【1】实时时钟" << endl;
        cout << "【2】正计时" << endl;
        cout << "【3】倒计时" << endl;
        cout << "【4】小闹钟" << endl;
        cout << "【5】番茄钟" << endl;
        cout << endl << "请选择模式：";

        char modeChoice = getch();
        cout << modeChoice << endl;

        if(modeChoice == '1') {
            system("start shishishizhong.exe");
        } else if(modeChoice == '2') {
            system("start zhengjishi.exe");
        } else if(modeChoice == '3') {
            system("start daojishi.exe");
        } else if(modeChoice == '4') {
            system("start naozhong.exe");
        } else if(modeChoice == '5') {
            system("start fanqiezhong.exe");
        }

        system("cls");
    }

    return 0;
}
