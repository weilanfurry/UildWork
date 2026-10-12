# UildWork（ZERO v4.0）

Windows 控制台生产力工具集。一份可执行文件承载全部工具，通过子命令分发；
同时保留旧版独立 exe 的编译目标，安装包与 `.udf` 文件关联不需要改动。

## 目录结构

```
源码整合/
├─ build.ps1              一键构建脚本（自动查找 MinGW，无需 CMake）
├─ resources/app.rc       exe 图标资源（引用 assets/favicon.ico）
├─ assets/                运行时资源
│  ├─ Walking The Wire.mp3
│  ├─ Battle Symphony.mp3
│  └─ favicon.ico
├─ src/
│  ├─ main.cpp            UildWork.exe 入口：解析子命令 / .udf 文件参数
│  ├─ common/             基础库（原来 8 个 cpp 各写一遍的东西）
│  │  ├─ console.*        颜色、清屏、光标、UTF-8 输出、对话框
│  │  ├─ text.*           UTF-8 ↔ UTF-16、显示宽度、对齐、时间格式化
│  │  ├─ input.*          按键/整行输入、范围校验、无控制台时的降级
│  │  ├─ ui.*             横幅、方向键菜单、进度条、大号数字、配色主题
│  │  ├─ audio.*          铃声选择与播放（MCI + 蜂鸣）
│  │  ├─ paths.*          程序目录、资源定位、启动子进程
│  │  ├─ timer.*          单调时钟计时器、周期触发器
│  │  └─ appinfo.*        名称/版本/问候语/本地时间
│  ├─ apps/               各工具实现（一个工具一个文件）
│  │  ├─ launcher.cpp     主菜单
│  │  ├─ manifest.cpp     任务清单（.udf 读写）
│  │  ├─ statistics.cpp   图表统计
│  │  ├─ stopwatch.cpp    智能秒表（模式选择）
│  │  ├─ clock.cpp        实时时钟
│  │  ├─ countup.cpp      正计时
│  │  ├─ countdown.cpp    倒计时
│  │  ├─ alarm.cpp        小闹钟
│  │  ├─ pomodoro.cpp     番茄钟
│  │  └─ info.cpp         --help / --version
│  └─ legacy/             兼容旧 exe 名的极薄入口（每个 4~8 行）
├─ archive/original/      迁移前的原始源码（仅归档，不参与构建）
└─ build/                 构建产物（bin/ 可执行文件，obj/ 中间文件）
```

## 构建

需要 MinGW-w64 / TDM-GCC（`g++` + `windres`），脚本会先找 Code::Blocks 自带的
MinGW，再找 PATH：

```powershell
.\build.ps1                                  # 输出到 build\bin
.\build.ps1 -Clean                           # 先清理中间文件
.\build.ps1 -DeployDir "D:\Uild\UildWork\ZERO\app"
                                             # 构建后直接复制到打包目录（含 assets）
```

手动编译等价命令（以主程序为例）：

```bat
g++ -std=gnu++11 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 -Isrc ^
    src\main.cpp src\common\*.cpp src\apps\*.cpp ^
    -static -static-libgcc -static-libstdc++ -lwinmm -lshell32 -lole32 -o build\bin\UildWork.exe
```

### 两个容易踩的工具链坑（脚本已处理）

1. **汇编器找不到 libintl-8.dll。** TDM-GCC / Code::Blocks 的布局是
   `…\MinGW\mingw32\bin\as.exe`，而 `libintl-8.dll`、`libiconv-2.dll` 在
   `…\MinGW\bin\`，而 Windows 只按 PATH 找 DLL。所以直接在命令行调 g++ 时
   `as.exe` 会报"找不到 libintl-8.dll"，编译器退出码 1 且**不打印任何信息**
   （Code::Blocks 会自动把 `…\MinGW\bin` 加进 PATH，所以在 IDE 里是正常的）。
   `build.ps1` 已自动把工具链目录加入 PATH。
2. **`windres` 不处理带空格的编译器路径。** 它自行拼接预处理命令行且不加引号，
   于是 `C:\Program Files (x86)\…` 被断成 `C:\Program`。脚本改用 8.3 短路径绕开。

编译标志用 `-std=gnu++11`（与 Code::Blocks 默认一致）。用 `-std=c++11` 时
`__STRICT_ANSI__` 会让 MinGW 头文件隐藏 `_fileno`、`SHGFP_TYPE_CURRENT` 等扩展。

## 运行

```
UildWork.exe                 主菜单
UildWork.exe statistics      图表统计
UildWork.exe manifest [文件] 任务清单（可带 .udf 路径）
UildWork.exe stopwatch       智能秒表（时钟/计时/闹钟/番茄钟）
UildWork.exe clock           实时时钟
UildWork.exe countup         正计时
UildWork.exe countdown       倒计时
UildWork.exe alarm           小闹钟
UildWork.exe pomodoro        番茄钟
UildWork.exe help|version    帮助 / 版本
```

菜单统一使用方向键（也支持数字键直选、W/S 移动），`Esc` 返回上一层，
所有计时类工具都支持 `空格` 暂停、`Q`/`Esc` 退出。

## 部署与打包

安装脚本 `1.iss` 从 `D:\Uild\UildWork\ZERO\app\*` 递归打包，所以：

1. `.\build.ps1 -DeployDir "D:\Uild\UildWork\ZERO\app"`；
2. 保留 `UW13.exe`、`unins000.exe`（主菜单的"退回 1.3 版本""卸载"会调用它们）；
3. 用 Inno Setup 编译 `1.iss`。

旧版需要的 `music1.vbs` / `music2.vbs` 已经不再需要，MP3 改为程序自己播放，
放在 `assets\` 子目录（文件关联里的 `.udf` 路径、注册表项都不受影响）。

## 结构与代码优化

| 原来 | 现在 |
| --- | --- |
| 9 个平铺在根目录的 cpp，颜色表/菜单/铃声各写一遍 | `src/common` 公共库 + `src/apps` 每工具一个文件 |
| `system("title ...")`、`system("cls")`、`system("color XX")` | 控制台 API（`SetConsoleTitleW` / `FillConsoleOutput*` / `SetConsoleTextAttribute`） |
| 输出走 `cout` + `printf` 混用，依赖控制台代码页 | 统一 `console::write`，UTF-8 → `WriteConsoleW`，重定向到文件也不乱码 |
| `system("start X.exe")` 依赖当前工作目录 | `CreateProcess` + `CREATE_NEW_CONSOLE`，按程序目录解析，缺文件会提示 |
| 变长数组 `string items[n]`、`float data[n][m]` | `std::vector` |
| `Sleep(1000)` 累加/累减，误差累积，无法暂停 | `std::chrono::steady_clock` 截止时间计算，支持暂停/跳过 |
| 每秒钟 `system("cls")` / `system("color")`（每秒创建进程） | 只重绘变化区域 |
| 无输入校验，`cin >> int` 非法输入即死循环 | `input::readInt/readDouble/readLine` 带范围校验与重试 |
| 无控制台时 `getch()` 返回 -1 导致忙等 | `input` 层自动降级为按行读取 |
| 源码 GBK 编码 | 源码 UTF-8，脚本 `-finput-charset=UTF-8` |

## 修复的缺陷

| 文件 | 问题 | 现状 |
| --- | --- | --- |
| `main.cpp` | 安装包注册了 `UildWork.exe "%1"`，但 `main()` 从不读参数，双击 `.udf` 只打开主菜单 | 识别 `.udf` 参数并直接打开对应清单 |
| `Manifest.cpp` | 变长数组（数量大即爆栈，0 或负数直接崩溃） | `std::vector` |
| `Manifest.cpp` | `cin >> item` 使任务名遇空格截断 | 整行读取 |
| `Manifest.cpp` | `completionStatus[itemIndex-1] = 1` 无范围校验，越界写内存 | 索引校验 + 上下键选择 |
| `Manifest.cpp` | 清单只存在内存中，退出即丢失 | `.udf` 保存/打开/另存为，默认写入"文档\UildWork" |
| `Statistics.cpp` | `data[row-1][col-1]` 无校验，输错即破坏内存 | 行列范围校验 |
| `Statistics.cpp` | `long long total` 累加 `float` 丢精度；项目数为 0 时除零 | `double` 累加 + 除零保护 |
| `Statistics.cpp` | 导出写到当前工作目录（Program Files 下通常没权限） | 写入"文档\UildWork"，并显示完整路径 |
| `Statistics.cpp` | 名为"图表统计"却没有任何图 | 按行/按列条形图 + 合计/平均/最大/最小 |
| `naozhong.cpp` | 设定时刻早于当前时间时永远不响 | 自动顺延次日并提示 |
| `naozhong.cpp` | 响铃后只能 Ctrl+C；`taskkill /f /im wscript.exe` 会杀掉系统上所有脚本宿主 | MCI 播放 + 按任意键停止（蜂鸣用可中断线程） |
| `naozhong.cpp` / `daojishi.cpp` | 铃声菜单与播放逻辑重复两份 | 合并到 `common/audio` |
| `daojishi.cpp` | 可输入负数/0，进入后行为异常 | 范围校验，支持"时:分:秒"自定义 |
| `zhengjishi.cpp` | 无法暂停、无法退出 | 空格暂停、R 归零、Q 退出 |
| `fanqiezhong.cpp` | 每秒 `system("color 07")` + `system("cls")` | 直接重绘，无进程创建 |
| `fanqiezhong.cpp` | 工作/休息时长硬编码，不能暂停/退出 | 可配置 25/5/15 与轮次，支持暂停/跳过/退出 |
| `Stopwatch.cpp` | `while(true)` 里 `start`，Esc 无效 | 菜单驱动，Esc 返回 |
| `shishishizhong.cpp` | 每秒创建进程清屏，只能 Ctrl+C 退出 | 按需重绘 + 大号数字 + 日期开关 |

下面几条是运行测试时才暴露出来的（前两条是 Windows 中文路径的经典坑）：

| 位置 | 问题 | 现状 |
| --- | --- | --- |
| 全部工具（命令行） | `main(char** argv)` 的参数经过 ANSI 代码页转换，中文路径的 `.udf` 变成 GBK 字节，文件关联打开必然失败 | 改用 `CommandLineToArgvW` 取宽字符参数再转 UTF-8 |
| 任务清单 / 图表统计 | MinGW 的 `std::ofstream` 按 ANSI 代码页打开路径，中文目录（如 `D:\…\源码整合\…`）写入直接失败 | 新增 `common/fileio`，用 `_wfopen` + UTF-8 字节读写 |
| 菜单 | 数字键只是移动高亮：按"1"时若高亮本来就在第 1 项，屏幕毫无变化，像是卡死 | 数字键直接选中并确认（与旧版单键操作一致），方向键用于浏览 |
| 主菜单 | 输入结束（脚本/管道）时 `readKey` 返回 Esc，而主菜单把 Esc 当作"什么都不做" → 空转死循环 | 新增 `input::atEndOfInput()`，输入结束即退出 |
| 智能秒表 | 启动即弹模态问候框，被脚本调用时会一直阻塞在那里 | 仅在有真实控制台时弹出 |
| 降级输入 | 兼容模式的单键别名（h/j/k/l/w/a/s/d）抢走了 a 新增 / d 删除 / s 保存 / l 另存为 | 只识别 up/down/left/right/enter/esc 等完整单词 |

## 已验证

本机用 TDM-GCC 5.1 完整构建（`.\build.ps1 -Clean` 从零重建，9 个可执行文件全部生成），
并用管道输入做了冒烟测试：

| 场景 | 结果 |
| --- | --- |
| `UildWork.exe version` / `help` | 中文正常输出，退出码 0 |
| `clock` / `countup` / `countdown` / `alarm` / `pomodoro` | 大号数字时钟、倒计时渲染正常，退出码 0 |
| `stopwatch` | 菜单显示与返回正常，退出码 0 |
| 主菜单：5 → y | 退出动画正常，退出码 0 |
| `manifest`：建清单 → 另存为 `D:\…\源码整合\build\test-manifest.udf` → 退出 | 生成正确的 UTF-8 `.udf`（无 BOM） |
| `UildWork.exe "D:\…\源码整合\…\test-manifest.udf"`（模拟双击关联） | 直接打开清单并回写，退出码 0 |
| `statistics`：录入 → 编辑 → 统计计算 → 导出 | 表格、条形图、合计/平均/最大/最小均正确 |

尚未覆盖：方向键高亮、暂停/继续、铃声播放、`MessageBox` 问候框这些依赖真实按键的交互，
管道环境下无法真实模拟；"导出到 文档\UildWork" 的写入路径因当前环境无该目录写权限未实测成功分支。

## 兼容性

* 仍会构建 `Statistics.exe`、`Manifest.exe`、`Stopwatch.exe`、`shishishizhong.exe`、
  `zhengjishi.exe`、`daojishi.exe`、`naozhong.exe`、`fanqiezhong.exe`，主菜单优先启动
  同目录的独立 exe，找不到时才用"自身 + 子命令"启动，因此新旧部署都可用。
* 对话框统一使用 `MessageBoxW`，中文不会因 ANSI 代码页出错。
* 铃声资源既支持 `assets\` 也兼容放在程序目录根部，便于从旧版本平滑升级。

## 后续可以做的事

1. 用 `1.iss` 的 `SetupIconFile` 指向 `assets\favicon.ico`，让安装程序也有图标。
2. 任务清单可增加拖拽排序、子任务、导入导出 CSV。
3. 统计模块可增加平均值柱状图、百分比饼图（控制台可用方块字符近似）。
4. 若要在 CI 里编译，可用 `-DeployDir` 之外的 `-OutDir` 参数把产物放进 artifacts 目录。
