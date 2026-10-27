# 图书借阅系统

面向对象程序设计（C++）课程设计子集，演示封装、继承、多态、组合和依赖关系。

## 功能

- 图书登记、查询和副本状态管理
- 学生/教师读者登记与差异化借阅规则
- 借书、归还和借阅记录查询
- 使用 CSV 文件保存数据

## 编译运行

在项目根目录执行：

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude `
  src/Book.cpp src/BorrowRecord.cpp src/BorrowRule.cpp `
  src/CsvUtils.cpp src/DateUtils.cpp src/Library.cpp src/Reader.cpp `
  src/main.cpp -o library_system.exe
.\library_system.exe
```

程序默认读取 `data/books.csv`、`data/readers.csv` 和
`data/records.csv`，选择菜单中的“保存”或退出时会写回文件。

## 运行测试

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude `
  src/Book.cpp src/BorrowRecord.cpp src/BorrowRule.cpp `
  src/CsvUtils.cpp src/DateUtils.cpp src/Library.cpp src/Reader.cpp `
  tests/test_library.cpp -o tests/test_library.exe
.\tests\test_library.exe
```

测试退出码为 `0` 表示全部通过。

## VS Code

默认生成任务已经配置为一次编译全部 `src/*.cpp`，不会只编译当前打开的单个文件。

- `Ctrl+Shift+B`：构建 `library_system.exe`
- `Tasks: Run Task` → `test library system`：编译测试程序
- `Tasks: Run Task` → `run library system`：构建后从项目根目录启动程序
- `F5`：使用 `Run Library System` 调试/运行配置
- 双击 `run.cmd`：不通过 VS Code，直接从程序目录启动

程序会按自身 `.exe` 所在目录定位 `data/`，不依赖 VS Code 的工作目录。

## 目录

```text
include/      头文件
src/          实现与命令行入口
tests/        无第三方依赖的断言测试
data/         初始数据及运行期持久化文件
diagrams/     PlantUML 源文件
report/      实验报告与报告图片
tools/        图片素材生成脚本
```
