#include <cassert>
#include <coro.hpp>

static void* add1(void* x) {
    auto r = reinterpret_cast<int *>(x);
    return new int(*r + 1);
}

int main() {
    coro::Loop loop;

    int *val = new int(5);

    coro::RawTask task(loop, add1, reinterpret_cast<void *>(val));

    loop.run();
    int *res = reinterpret_cast<int *>(task.join());

    assert(*res == 6);

    delete val;
    delete res;

    return 0;
}
