#include <iostream>
#include <string>
using namespace std;

int main()
{
    std::string name;

    std::getline(std::cin, name);
    std::cout << "Hello, " << name << "!\n";

    std::getline(std::cin, name);
    std::cout << "Hello, " << name << "!\n";
    cin.get();
    return 0;
}