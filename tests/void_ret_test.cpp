#include <coro.hpp>

// This is strictly a "will this compile" test
static void task(coro::Task<void> *task)
{
    (void) task;
}

int main(void)
{
    coro::Loop loop;

    coro::Task<void> voidtask(loop, task);

    loop.run();

    return 0;
}
