#include <Windows.h>
#include <iostream>
#include <conio.h>


using namespace std;

int main() {
    while(true) {
        SYSTEMTIME currentTime;
        GetLocalTime(&currentTime);

        printf("Time:%04d/%02d/%02d %02d:%02d:%02d\n",currentTime.wYear, currentTime.wMonth, currentTime.wDay,currentTime.wHour, currentTime.wMinute, currentTime.wSecond);

        Sleep(1000);
        system("cls");
    }

    return 0;
}
