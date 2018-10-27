#include <iostream>
#include <string>

/*
    合法源文件。
*/

int main(int argc, char const *argv[])
{
    const int i = ~0;
    int j = 011, k = 0xa1;
    double d = 0.3e2 + i;
    char a = '\'';
    std::string s = "string value\n";
    std::cout << s;
    return 0;
}
