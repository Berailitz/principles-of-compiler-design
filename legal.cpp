#include <iostream>
#include <string>

/*
    合法源文件。
*/

int main(int argc, char const *argv[])
{
    const int _a = ~0, B_=11;
    double d2 = 0.3e2 + _a;
    int j = 0xa1, k = 011;
    char c = '\'';
    std::string s = "string value\n";
    std::cout << s;
    return 0;
}
