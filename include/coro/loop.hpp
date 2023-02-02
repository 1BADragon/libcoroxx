#pragma once

#include <unordered_map>
#include <tuple>

#include <coro.h>

namespace coro {

class Loop {
public:
    Loop();
    Loop(int flags);
    virtual ~Loop();

    static Loop *current();
    static Loop *from_raw(coro_loop *r);

    int run();
    int poke();

    int callsoon(coro_void_f func, void *cb);
    int callsoon_threadsafe(coro_void_f func, void *cb);

    template<typename F, typename ...Args>
    void callsoon(F &&f, Args&& ...args)
    {
        struct _ctx {
            F f;
            std::tuple<Args...> args;
        };

        auto ctx =  new _ctx{std::forward<F>(f), std::make_tuple(std::forward<Args>(args)...)};

        coro_callsoon(inner(), [] (void *arg) {
            auto i_ctx = reinterpret_cast<_ctx *>(arg);
            std::apply(std::move(i_ctx->f), std::move(i_ctx->args));
            delete i_ctx;
        }, reinterpret_cast<void *>(ctx));
    }

    template<typename F, typename ...Args>
    void callsoon_threadsafe(F f, Args ...args)
    {
        struct _ctx {
            F f;
            std::tuple<Args...> args;
        };

        auto ctx =  new _ctx{f, std::make_tuple(std::move(args)...)};

        coro_callsoon_threadsafe(inner(), [] (void *arg) {
            auto i_ctx = reinterpret_cast<_ctx *>(arg);
            std::apply(i_ctx->f, std::move(i_ctx->args));
            delete i_ctx;
        }, reinterpret_cast<void *>(ctx));
    }

    coro_loop *inner();

private:
    static std::unordered_map<coro_loop *, Loop *> &mapping();

    coro_loop *_inner;
};
}
