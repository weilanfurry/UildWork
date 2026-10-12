#pragma once
// UildWork 输入层
//
// 统一封装键盘输入，并解决原实现的两个问题：
//   1. getch() 在无控制台（管道/重定向）环境下返回 -1，原来的 while(getch()!='s')
//      会变成死循环；这里退化为按行读取，程序仍然可用（也方便自动化测试）。
//   2. cin >> int 遇到非法输入会让流进入错误状态并无限循环；这里统一做校验与重试。

#include <string>

namespace uw {
namespace input {

enum class Key {
    None,
    Character,  // 见 KeyEvent::character
    Enter,
    Escape,
    Space,
    Tab,
    Backspace,
    Up,
    Down,
    Left,
    Right,
    Home,
    End,
    Delete,
    Insert
};

struct KeyEvent {
    Key key;
    char character;

    KeyEvent() : key(Key::None), character(0) {}
    explicit KeyEvent(Key value) : key(value), character(0) {}
    KeyEvent(Key value, char ch) : key(value), character(ch) {}

    bool isCharacter(char expected) const {
        return key == Key::Character &&
               (character == expected ||
                (expected >= 'a' && expected <= 'z' && character == static_cast<char>(expected - 'a' + 'A')) ||
                (expected >= 'A' && expected <= 'Z' && character == static_cast<char>(expected - 'A' + 'a')));
    }
};

// 是否连着真实控制台（可逐键读取）
bool isInteractive();

// 清空输入缓冲，防止上一次的残留按键影响下一次选择
void clearBuffer();

// 是否有按键等待读取（非控制台环境恒为 false）
bool keyPressed();

// 阻塞读取一个按键
KeyEvent readKey();

// 上一次 readKey() 是否因为输入结束而返回（非交互模式下读到 EOF）。
// 用于避免"按 Esc 什么都不做"的循环在无输入时空转。
bool atEndOfInput();

// 阻塞读取一个按键，只接受 allowed 中的字符（大小写不敏感），返回实际按下的字符
char readChar(const std::string& allowed, bool echo = true);

// 读取整数，超出 [lowest, highest] 或非法输入时重新询问
int readInt(const std::string& utf8Prompt, int lowest, int highest);

// 读取浮点数，超出 [lowest, highest] 或非法输入时重新询问
double readDouble(const std::string& utf8Prompt, double lowest, double highest);

// 读取一整行（可含空格）。
// allowEmpty 为 false 时会一直问到非空为止。
std::string readLine(const std::string& utf8Prompt, bool allowEmpty = false);

// 读取 HH:MM:SS 形式的时间，返回总秒数；输入 q 返回 false（表示放弃）
bool readTimeOfDay(const std::string& utf8Prompt, int& hour, int& minute, int& second);

void pause(const std::string& utf8Prompt = "按任意键继续...");

}  // namespace input
}  // namespace uw
