#include <iostream>
#include <cassert>
#include <coro/task.hpp>
#include <coro/loop.hpp>

int counter(coro::Task<int> *t)
{
    int i = 0;

    for (; i < 10; ++i) {
        t->yeild_val(i);
    }

    return i;
}

static int main_task(coro::Task<int> *task)
{
    int expected = 0;
    coro::Task<int> counting_task(counter);

    for (auto it = counting_task.begin(); it != counting_task.end(); ++it) {
        assert(expected == *it);

        expected++;
    }

    expected = 0;
    for (int val : coro::Task<int>(counter)) {
        assert(expected == val);
        expected++;
    }

    return 0;
}

int main()
{
    coro::Loop loop;

    coro::Task<int> task(loop, main_task);

    loop.run();

    return task.join();
}
