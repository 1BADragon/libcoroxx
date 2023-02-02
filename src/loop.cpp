#include <coro/loop.hpp>

namespace coro {

Loop::Loop()
{
    _inner = coro_new_loop(0);

    if (_inner == nullptr) {
        throw std::bad_alloc();
    }

    mapping()[_inner] = this;
}

Loop::Loop(int flags)
{
    _inner = coro_new_loop(flags);

    if (_inner == nullptr) {
        throw std::bad_alloc();
    }

    mapping()[_inner] = this;
}

Loop::~Loop()
{
    mapping().erase(_inner);
    coro_free_loop(_inner);
}

Loop *Loop::current()
{
    return from_raw(coro_current());
}

Loop *Loop::from_raw(coro_loop *r)
{
    return mapping().at(r);
}

int Loop::run()
{
    return coro_run(_inner);
}

int Loop::poke()
{
    return coro_poke(_inner);
}

int Loop::callsoon(coro_void_f func, void *cb)
{
    return coro_callsoon(_inner, func, cb);
}

int Loop::callsoon_threadsafe(coro_void_f func, void *cb)
{
    return coro_callsoon_threadsafe(_inner, func, cb);
}

coro_loop *Loop::inner()
{
    return _inner;
}

std::unordered_map<coro_loop *, Loop *> &Loop::mapping()
{
    static std::unordered_map<coro_loop *, Loop *> _mapping;

    return _mapping;
}
}
