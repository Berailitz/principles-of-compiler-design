#include <iostream>
#include <string>

/*
    延伸测试样例-高级运算符及符号表
*/

class A
{
public:
    A();
    int s;
};

int main(int argc, char const *argv[])
{
    A aa;
    char a = '1', b = '\'';
    std::string s1 = "string value\n", s2 = "67gi8gb_+@#$%s^&*(oimk";
    std::cout << s1;
    std::cin >> s2;
    aa.s = (~2 | 4);
    a %= (&aa)->s;
    return 0;
}
