#include <iostream>
#include <unordered_map>
using namespace std;

int main()
{
    unordered_multimap<int, int> map = {{1, 2}, {1, 3}, {2, 4}};
    for (unordered_multimap<int, int>::iterator it = map.begin(), end = map.end(); it != end; it = map.upper_bound(it->first))
    {
        auto range = map.equal_range(it->first);
        cout << it->first << endl;
        for (auto i = range.first; i != range.second; ++i)
        {
            cout << i->first << ' ' << i->second << endl;
        }
    }
    cin.get();
    return 0;
}