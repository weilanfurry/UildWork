#include <Windows.h>
#include <iostream>
#include <conio.h>

using namespace std;

int main() {
    cout << "欢迎来到UildWork番茄钟！" << endl << endl;

    int hours = 0, minutes = 25, seconds = 0;
    cout << "按s键开始番茄钟";

    char startKey = 'n';
    while(startKey != 's') {
        startKey = getch();
    }

    bool isWorkMode = true;
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

            if(isWorkMode) {
                cout << endl << "工作时间结束！开始休息！";
                isWorkMode = false;
                hours = 0;
                minutes = 5;
                seconds = 0;
            } else {
                cout << endl << "休息时间结束！开始工作！";
                isWorkMode = true;
                hours = 0;
                minutes = 25;
                seconds = 0;
            }

            for(int i = 0; i < 10; i++) {
                Beep(880, 500);
                Sleep(100);
            }
        }

        system("color 07");
        system("cls");
    }

    return 0;
}
