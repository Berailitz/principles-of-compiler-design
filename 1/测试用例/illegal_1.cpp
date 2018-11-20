/*
    错误处理测试样例-常数中的错误
*/

int main(int argc, char const *argv[])
{
    const int _a = ~0., B_ = 11;
    double d2 = 0.3e2 + _a;
    int j = 0xz1, k = 011;
    return 0;
}
