#pragma once

#include <coro.h>

namespace coro {

class Loop;

class RawQueue {
public:
    RawQueue();
    RawQueue(Loop &l);
    virtual ~RawQueue();

    int push(void *data, coro_void_f free_f = nullptr);

    void *pop();
    void *pop_nowait();

    void closewrite();

    coro_queue *inner();

private:
    coro_queue *_inner;
};
}
