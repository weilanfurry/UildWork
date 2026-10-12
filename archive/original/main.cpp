#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <string>
#include <stdio.h>

using namespace std;

// 设置控制台颜色函数 (ANSI 兼容)
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// 清屏函数
void clearScreen() {
    system("cls");
}

// 延时函数
void delay(int milliseconds) {
    Sleep(milliseconds);
}

// 显示简单标题
void showTitle() {
    setColor(11); // 浅蓝色
    cout << "================================================" << endl;
    cout << "              UILDWORK 生产力工具集" << endl;
    cout << "================================================" << endl;
    setColor(7); // 恢复默认颜色
}

// 显示加载动画
void showLoading() {
    setColor(10); // 绿色
    cout << "加载中";
    for(int i = 0; i < 5; i++) {
        cout << ".";
        delay(200);
    }
    cout << " 完成!" << endl;
    delay(500);
}

// 显示当前时间
void showTime() {
    SYSTEMTIME time;
    GetLocalTime(&time);

    setColor(13); // 粉色
    cout << "当前时间: ";
    setColor(15); // 白色
    printf("%04d/%02d/%02d %02d:%02d:%02d",time.wYear, time.wMonth, time.wDay,time.wHour, time.wMinute, time.wSecond);

    // 根据时间显示不同问候语
    setColor(11);
    cout << " - ";
    if(time.wHour >= 18) {
        cout << "晚上好!";
    } else if(time.wHour >= 12) {
        cout << "下午好!";
    } else if(time.wHour >= 6) {
        cout << "上午好!";
    } else {
        cout << "凌晨好!";
    }
    cout << endl << endl;
}

// 显示菜单选项
void showMenu() {
    setColor(14); // 黄色
    cout << "******************** 功能菜单 ********************" << endl;

    setColor(11);
    cout << "    【1】 图表统计" << endl;
    setColor(12);
    cout << "    【2】 任务清单" << endl;
    setColor(13);
    cout << "    【3】 智能秒表" << endl;
    setColor(14);
    cout << "    【4】 高级选项" << endl;
    setColor(15);
    cout << "    【5】 退出软件" << endl;

    setColor(14);
    cout << "**************************************************" << endl;
}

// 显示高级选项菜单
void showAdvancedMenu() {
    clearScreen();
    setColor(14);
    cout << "================== 高级选项 ==================" << endl;

    setColor(11);
    cout << "    【1】 退回1.3版本" << endl;
    setColor(13);
    cout << "    【2】 关于UildWork" << endl;
    setColor(14);
    cout << "    【3】 卸载UildWork" << endl;
    setColor(15);
    cout << "    【4】 返回主菜单" << endl;

    setColor(14);
    cout << "==============================================" << endl;
}

// 显示关于信息
void showAbout() {
    clearScreen();
    setColor(14);
    cout << "================== 关于 UildWork ==================" << endl;

    setColor(15);
    cout << "  UildWork 是一款多功能生产力工具集，包含:" << endl;
    cout << endl;
    setColor(10);
    cout << "  - 图表统计 - 数据可视化与分析工具" << endl;
    setColor(11);
    cout << "  - 任务清单 - 个人任务管理与进度跟踪" << endl;
    setColor(12);
    cout << "  - 智能秒表 - 多种计时功能集成" << endl;
    setColor(13);
    cout << "  - 闹钟提醒 - 自定义提醒与多种铃声" << endl;
    setColor(14);
    cout << "  - 番茄钟 - 高效工作法时间管理工具" << endl;
    cout << endl;
    setColor(15);
    cout << "  版本: ZERO v4.0" << endl;
    cout << "  官网：https://weilanfurry.github.io/" << endl;
    cout << "  开发者: UildWork & 蔚澜Waverly" << endl;

    setColor(14);
    cout << "===================================================" << endl;

    setColor(7);
    cout << endl << "按任意键返回...";
    getch();
}

// 显示退出信息
void showExit() {
    clearScreen();
    setColor(12);
    cout << "**************************************************" << endl;
    cout << "                 感谢使用 UildWork!" << endl;
    cout << "**************************************************" << endl;

    setColor(14);
    cout << "            期待再次相见! 再见!" << endl << endl;

    // 简单闪烁效果
    for(int i = 0; i < 3; i++) {
        setColor(12);
        cout << "                程序即将退出..." << endl;
        delay(500);
        clearScreen();
        delay(200);
        setColor(12);
        cout << "**************************************************" << endl;
        cout << "                 感谢使用 UildWork!" << endl;
        cout << "**************************************************" << endl;
        setColor(14);
        cout << "            期待再次相见! 再见!" << endl << endl;
    }
}

int main() {
    // 设置窗口标题
    system("title UildWork - 多功能生产力工具集");

    // 初始加载
    clearScreen();
    showTitle();
    showLoading();
    delay(1000);

    while(true) {
        clearScreen();

        // 显示标题
        showTitle();

        // 显示当前时间和问候语
        showTime();

        // 显示菜单
        showMenu();

        // 获取用户输入
        setColor(10);
        cout << "请输入选项 (1-5): ";

        char option;
        option = getch();
        cout << option << endl << endl;

        // 处理用户选择
        switch(option) {
            case '1':
                setColor(11);
                cout << "启动图表统计功能..." << endl;
                delay(1000);
                system("start Statistics.exe");
                break;

            case '2':
                setColor(12);
                cout << "启动任务清单功能..." << endl;
                delay(1000);
                system("start Manifest.exe");
                break;

            case '3':
                setColor(13);
                cout << "启动智能秒表功能..." << endl;
                delay(1000);
                system("start Stopwatch.exe");
                break;

            case '4':
                {
                    // 高级选项子菜单
                    char advancedOption;
                    do {
                        showAdvancedMenu();
                        setColor(10);
                        cout << "请输入选项 (1-4): ";

                        advancedOption = getch();
                        cout << advancedOption << endl << endl;

                        switch(advancedOption) {
                            case '1':
                                setColor(11);
                                cout << "正在退回1.3版本..." << endl;
                                delay(1000);
                                system("start UW13.exe");
                                break;

                            case '2':
                                showAbout();
                                break;

                            case '3':
                                setColor(12);
                                cout << "准备卸载UildWork..." << endl;
                                delay(1000);
                                system("start unins000.exe");
                                break;

                            case '4':
                                setColor(14);
                                cout << "返回主菜单..." << endl;
                                delay(500);
                                break;

                            default:
                                setColor(12);
                                cout << "无效选项，请重新选择!" << endl;
                                delay(1000);
                        }
                    } while(advancedOption != '4');
                }
                break;

            case '5':
                setColor(12);
                cout << "确定要退出吗? (Y/N): ";
                char confirm;
                confirm = getch();
                cout << confirm << endl;

                if(confirm == 'Y' || confirm == 'y') {
                    showExit();
                    delay(2000);
                    return 0;
                }
                break;

            default:
                setColor(12);
                cout << "无效选项，请重新选择!" << endl;
                delay(1000);
        }

    }

    return 0;
}
