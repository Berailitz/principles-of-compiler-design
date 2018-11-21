#include "analyser.cpp"

int main(int argc, char const *argv[])
{
    ifstream stream;
    Analyser analyser;
    if (argc == 2)
    {
        // 使用文件输入
        stream.open(argv[1]);
        analyser.create_grammar(stream); // 构建文法
        analyser.receive_text(stream); // 分析句子
    }
    else
    {
        // 使用交互式输入
        analyser.create_grammar(cin);
        analyser.receive_text(cin);
    }
    cout << "Press Enter to exit.";
    cin.get();
    return 0;
}
