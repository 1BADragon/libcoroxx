#pragma once

#include <coro/rawqueue.hpp>

#include <optional>
#include <vector>

namespace coro {

template<typename T>
class Queue
{
public:
    using value_type = T;
    Queue()
        : _queue() {}
    Queue(Loop &l)
        : _queue(l) {}

    void push(const value_type& val)
    {
        // convert this to a heap value
        value_type *_heap_alloced = new value_type(val);
        _queue.push(reinterpret_cast<void *>(_heap_alloced),
                    [] (void *_val) {
            delete reinterpret_cast<value_type *>(_val);
        });
    }

    void push(value_type&& val)
    {
        // convert this to a heap value
        value_type *_heap_alloced = new value_type(std::forward<value_type>(val));
        _queue.push(reinterpret_cast<void *>(_heap_alloced),
                    [] (void *_val) {
            delete reinterpret_cast<value_type *>(_val);
        });
    }

    std::optional<value_type>
    pop()
    {
        void *_raw_val = _queue.pop();

        if (nullptr == _raw_val) {
            return std::nullopt;
        }
        value_type *_ptr = reinterpret_cast<value_type *>(_raw_val);

        value_type _tmp = std::move(*_ptr);
        delete _ptr;
        return _tmp;
    }

    std::optional<value_type>
    pop_nowait()
    {
        void *_raw_val = _queue.pop_nowait();

        if (nullptr == _raw_val) {
            return std::nullopt;
        }
        value_type *_ptr = reinterpret_cast<value_type *>(_raw_val);

        value_type _tmp = std::move(*_ptr);
        delete _ptr;
        return _tmp;
    }

    void closewrite()
    {
        _queue.closewrite();
    }

    class iterator
    {
    public:

    private:
        iterator (Queue *q)
            : _q(q), _copied(false) {};

        Queue *_q;
        value_type *_ptr;
        bool _copied;
    };

private:
    RawQueue _queue;
};

}
