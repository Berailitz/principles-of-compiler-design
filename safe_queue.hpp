#include <queue>
#include <mutex>
#include <condition_variable>
#include <initializer_list>

using namespace std;

template <typename T>
class safe_queue
{
  private:
    mutable mutex mut; // 不影响外部状态
    mutable condition_variable condition;
    queue<T> data_queue;

  public:
    void push(const T &new_value);
    T pop();
    bool empty() const;
    int size() const;
};
