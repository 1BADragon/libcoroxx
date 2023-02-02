#include <cassert>
#include <coro.hpp>

static int add1(coro::Task<int> *task, int x)
{
    task->write(1, "Here\n", 5);
    return x + 1;
}

int main()
{
    coro::Loop loop;

    coro::Task<int> t(loop, add1, 4);

    loop.run();
    int res = t.join();

    assert(res == 5);

    return 0;
}
