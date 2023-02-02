#include <coro/loop.hpp>
#include <coro/rawqueue.hpp>

namespace coro {

RawQueue::RawQueue()
{
   _inner = coro_queue_new(nullptr);
}

RawQueue::RawQueue(Loop &l)
{
    _inner = coro_queue_new(l.inner());
}

RawQueue::~RawQueue()
{
    coro_queue_delete(_inner);
}

int RawQueue::push(void *data, coro_void_f free_f)
{
    return coro_queue_push(_inner, data, free_f);
}

void *RawQueue::pop()
{
    return coro_queue_pop(_inner);
}

void *RawQueue::pop_nowait()
{
    return coro_queue_pop_nowait(_inner);
}

void RawQueue::closewrite()
{
    coro_queue_closewrite(_inner);
}

coro_queue *RawQueue::inner()
{
    return _inner;
}

}
