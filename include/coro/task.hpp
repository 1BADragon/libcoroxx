#pragma once

#include <optional>
#include <chrono>
#include <stdexcept>
#include <coro/rawtask.hpp>

namespace coro {

class Loop;

template<typename T>
class Task : public RawTask {
public:
    using return_type = T;

    template<typename F, typename ...Args>
    Task(F f, Args... args) :
        RawTask([] (void *fargs) -> void * {
            auto funcargs = reinterpret_cast<std::pair<F, std::tuple<Task<T> *, Args...>> *>(fargs);

            auto ret = std::apply(std::move(std::get<0>(*funcargs)),
                                  std::move(std::get<1>(*funcargs)));
            delete funcargs;

            return reinterpret_cast<void *>(new return_type(std::move(ret)));
        },
        reinterpret_cast<void *>(new std::pair<F, std::tuple<Task<T> *, Args...>>(f, {this, std::move(args)...})))
    {

    }

    template<typename F, typename ...Args>
    Task(Loop &loop, F f, Args... args) :
        RawTask(loop, [] (void *fargs) -> void * {
            auto funcargs = reinterpret_cast<std::pair<F, std::tuple<Task<T> *, Args...>> *>(fargs);

            auto ret = std::apply(std::move(std::get<0>(*funcargs)),
                                  std::move(std::get<1>(*funcargs)));
            delete funcargs;

            return reinterpret_cast<void *>(new return_type(std::move(ret)));
        },
        reinterpret_cast<void *>(new std::pair<F, std::tuple<Task<T> *, Args...>>(f, {this, std::move(args)...})))
    {

    }

    return_type join()
    {
        void *rawval = RawTask::join();

        auto val = reinterpret_cast<return_type *>(rawval);

        return_type ret = std::move(*val);
        delete val;
        return ret;
    }

    std::optional<return_type> await()
    {
        void *raw = RawTask::await();

        if (nullptr == raw) {
            return std::nullopt;
        }

        return_type *valp = reinterpret_cast<return_type *>(raw);

        return_type val = std::move(*valp);
        delete valp;

        return val;
    }

    bool await_val(return_type &val)
    {
        void *raw;

        if (RawTask::await_val(&raw)) {
            return true;
        }

        return_type *valp = reinterpret_cast<return_type *>(raw);

        val = *valp;
        delete valp;
        return false;
    }

    void yeild_val(const return_type &val)
    {
        auto *valp = new return_type(val);

        RawTask::yeild_val(reinterpret_cast<void *>(valp));
    }

    void yeild_val(return_type &&val)
    {
        auto *valp = new return_type(std::forward<return_type>(val));

        RawTask::yeild_val(valp);
    }

    void sleep(const std::chrono::seconds &d)
    {
        sleep(d.count());
    }

    void sleepms(const std::chrono::milliseconds &d)
    {
        sleepms(d.count());
    }

    class iterator {
    public:
        using value_type = return_type;

        iterator(const iterator &rhs) = delete;
        iterator(iterator &&rhs)
        {
            this->_curr_val = rhs._curr_val;
            this->_finished = rhs._finished;
            this->_parent = rhs._parent;

            rhs._finished = true;
            rhs._parent = nullptr;
        }

        iterator &operator=(const iterator &rhs) = delete;
        iterator &operator=(iterator &&rhs)
        {
            this->_curr_val = rhs._curr_val;
            this->_finished = rhs._finished;
            this->_parent = rhs._parent;

            rhs._finished = true;
            rhs._parent = nullptr;
        }

        bool operator==(const iterator &rhs)
        {
            return _parent == rhs._parent &&
                    _finished == rhs._finished;
        }

        bool operator!=(const iterator &rhs)
        {
            return _parent != rhs._parent ||
                    _finished != rhs._finished;
        }

        iterator &operator++()
        {
            if (_parent->await_val(_curr_val)) {
                _finished = true;
            }

            return *this;
        }

        value_type &operator*()
        {
            if (_finished) {
                throw std::runtime_error("dereference of eneded task");
            }
            return _curr_val;
        }

    private:
        friend Task<T>;

        iterator(Task<T> *parent, bool isend)
            : _parent(parent), _finished(isend)
        {
            if (!_finished) {
                ++(*this);
            }
        }

        Task<T> *_parent;
        return_type _curr_val;
        bool _finished;
    };

    iterator begin()
    {
        return iterator(this, false);
    }

    iterator end()
    {
        return iterator(this, true);
    }
};

}
