#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

int main()
{
    ifstream source("legal.cpp");
    char out[1000];
    source.read(out, 1000);
    cout << out;
    cin.get();
    return 0;
}
