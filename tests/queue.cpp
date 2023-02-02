#include <cassert>
#include <coro.hpp>

static void* producer(void *arg)
{
    auto queue = reinterpret_cast<coro::Queue<int> *>(arg);

    for(int i = 0; i < 10; ++i) {
        queue->push(i);
    }

    return nullptr;
}

static void* consumer(void *arg)
{
    auto queue = reinterpret_cast<coro::Queue<int> *>(arg);

    for (int i = 0; i < 10; ++i) {
        auto val_opt = queue->pop();

        assert(val_opt.has_value());
        assert(i == *val_opt);
    }

    return nullptr;
}

int main()
{
    coro::Loop loop;
    coro::Queue<int> queue(loop);

    coro::RawTask prod(loop, &producer, reinterpret_cast<void *>(&queue));
    coro::RawTask cons(loop, &consumer, reinterpret_cast<void *>(&queue));

    loop.run();
    return 0;
}
