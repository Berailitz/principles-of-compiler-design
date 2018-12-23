#include <fstream>
#include <windows.h>

#pragma comment(lib, "user32")

#include "calculator.cpp"

int main(int argc, char const *argv[])
{
    ifstream stream;
    Calculator calculator;
    ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); // 最大化窗口
    if (argc == 2)
    {
        // 使用文件输入
        stream.open(argv[1]);
        calculator.receive_text(stream); // 分析句子
    }
    else
    {
        // 使用交互式输入
        calculator.receive_text(cin);
    }
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
