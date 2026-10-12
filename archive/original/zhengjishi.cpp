#include <Windows.h>
#include <iostream>
#include <conio.h>

using namespace std;

int main() {
    cout << "欢迎来到UildWork正计时器！" << endl << endl;
    cout << "按s键开始计时";

    char startKey = 'n';
    while(startKey != 's') {
        startKey = getch();
    }

    system("cls");

    int hours = 0, minutes = 0, seconds = 0;

    while(true) {
        cout << endl;
        cout << hours << "时" << minutes << "分" << seconds << "秒" << endl << endl;

        Sleep(1000);
        seconds++;

        if(seconds == 60) {
            seconds = 0;
            minutes++;
        }

        if(minutes == 60) {
            minutes = 0;
            hours++;
        }

        system("cls");
    }

    return 0;
}
