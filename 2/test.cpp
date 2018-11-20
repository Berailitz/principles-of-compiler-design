#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

const string PRODUCTION_MARK = "->";

int main()
{
    std::string grammar_text = "Let -> me split this into words";
    istringstream iss(grammar_text);
    vector<string> words((istream_iterator<string>(iss)),
                         istream_iterator<string>());
    if (words[1] == PRODUCTION_MARK)
    {
        cout << "ok";
    }
    else
    {
        cout << "Invalid grammar.";
    }
    for (auto const &e : words)
    {
        cout << e << endl;
    }
    cin.get();
}