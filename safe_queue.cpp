#include "safe_queue.hpp"

template <typename T>
void safe_queue<T>::push(const T &new_value)
{
    lock_guard<mutex> lk(mut);
    data_queue.push(move(new_value));
    condition.notify_one();
}

template <typename T>
T safe_queue<T>::pop()
{
    unique_lock<mutex> lk(mut);
    condition.wait(lk, [this] { return !this->data_queue.empty(); });
    auto value = move(data_queue.front());
    data_queue.pop();
    return value;
}

template <typename T>
bool safe_queue<T>::empty() const
{
    lock_guard<mutex> lk(mut);
    return data_queue.empty();
}

template <typename T>
int safe_queue<T>::size() const
{
    lock_guard<mutex> lk(mut);
    return data_queue.size();
}
