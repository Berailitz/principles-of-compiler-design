/*
    延伸测试样例-多进制整数
*/

int main(int argc, char const *argv[])
{
    int a = 012, b = 0x12;
    const int c = 012e2, d = 0x12E3;
    a += 0x12E-3;
    a -= 012E-4;
    return 0;
}
