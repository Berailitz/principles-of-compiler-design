#include <iostream>
#include <string>
using namespace std;

int main()
{
    std::string a = "aa", b = "ab'";
    if (a < b)
    {
        cout << 1;
    }
    else
    {
        cout << 2;
    }
    cin.get();
    return 0;
}