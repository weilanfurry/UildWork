#pragma once
// UildWork 铃声 / 声音层
//
// 原实现的三个问题：
//   1. 铃声菜单与播放代码在 naozhong.cpp 和 daojishi.cpp 里各写了一遍，这里合并成一份。
//   2. 播放 mp3 依赖外部的 music1.vbs / music2.vbs（脚本里还是硬编码相对路径），
//      安装目录变化就会失效；现在直接用 MCI 播放 assets 里的 mp3，
//      MCI 打不开时退化为交给系统默认播放器。
//   3. 原来响铃是 while(true) Beep(...)，或者 taskkill /f /im wscript.exe
//      （会顺手杀掉系统里所有脚本宿主），现在改为按任意键停止，且只操作自己的播放设备。

#include <string>
#include <vector>

namespace uw {
namespace audio {

// 与原菜单编号保持一致：1=蜂鸣 2=Walking The Wire 3=Battle Symphony
enum RingtoneId {
    RingtoneBeep = 1,
    RingtoneWalkingTheWire = 2,
    RingtoneBattleSymphony = 3
};

struct Ringtone {
    int id;
    std::string label;
    std::string fileName;  // 空表示使用系统蜂鸣
    std::string path;      // 解析出的绝对路径，未找到时为空
};

// 列出全部铃声（蜂鸣永远可用，音乐文件缺失时 path 为空）
std::vector<Ringtone> availableRingtones();

// 交互式选择铃声，返回铃声编号；用户取消时返回蜂鸣
int chooseRingtone(const std::string& utf8Title = "选择铃声");

// 按编号取铃声描述
Ringtone ringtoneById(int id);

// 开始循环播放（蜂鸣类铃声会启动一个后台线程）
void startLoop(int ringtoneId);

// 停止播放并释放设备
void stopLoop();

// 短促提示音（阶段切换等场景），durationMs 后自动停止
void chime(int ringtoneId, int durationMs = 1200);

// 循环响铃直到用户按下任意键
void ringUntilDismissed(int ringtoneId, const std::string& utf8Message);

}  // namespace audio
}  // namespace uw
