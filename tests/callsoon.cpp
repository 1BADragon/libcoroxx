#include <cassert>
#include <coro.hpp>

static bool exit_time = false;

static int should_run()
{
    if (exit_time) {
        return 0;
    }

    return 1;
}

static void disable()
{
    exit_time = true;
}

void* main_task(void *)
{
    coro::Loop::current()->callsoon(disable);

    coro::RawTask::current()->wait_custom(should_run);

    return nullptr;
}

int main()
{
    coro::Loop loop;

    coro::RawTask task(loop, &main_task, nullptr);

    loop.run();

    assert(exit_time);
    return 0;
}
