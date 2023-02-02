#pragma once

#include <string>
#include <unordered_map>

#include <coro.h>

namespace coro {

class Loop;

class RawTask {
public:
    RawTask(coro_task_entry_f func, void *arg);
    RawTask(Loop &loop, coro_task_entry_f func, void *cb);
    virtual ~RawTask();

    static RawTask *current();
    static RawTask *from_raw(coro_task *r);

    Loop* parent();

    bool running();
    int cancel();

    std::string_view name();
    int set_name(std::string_view name);

    void* join();

    void *await();
    bool await_val(void **val);

    void yeild();
    void yeild_val(void *val);

    void sleep(uintmax_t amnt);
    void sleepms(uintmax_t amnt);

    void wait_custom(coro_wait_custom_f poll, void *cb_data);

    template<typename F, typename ...Args>
    void wait_custom(F f, Args ...args)
    {
        struct _ctx {
            F f;
            std::tuple<Args...> args;
        };

        auto ctx = new _ctx{f, std::make_tuple(std::move(args)...)};
        coro_wait_custom([] (void *arg) -> int {
            auto i_ctx = reinterpret_cast<_ctx *>(arg);

            bool rc = std::apply(i_ctx->f, i_ctx->args);
            delete i_ctx;
            return rc;
        }, reinterpret_cast<void *>(ctx));
    }

    void wait_fd(int fd, int how);

    long read(int fd, void *buf, unsigned long len);
    long write(int fd, const void *buf, unsigned long len);

    int accept(int sock, struct sockaddr *addr, socklen_t *addr_len);
    long recv(int sock, void *buf, unsigned long len, int flags);
    long recvfrom(int sock, void * buf, unsigned long len,
                  int flags, struct sockaddr * src_addr,
                  socklen_t * addrlen);
    long recvmsg(int sock, struct msghdr *msg, int flags);
    long send(int sock, const void *buf, unsigned long len, int flags);
    long sendto(int sock, const void *buf, unsigned long len, int flags,
                const struct sockaddr *dest_addr, socklen_t addrlen);
    long sendmsg(int sockfd, const struct msghdr *msg, int flags);

    coro_task *inner();

private:
    static std::unordered_map<coro_task *, RawTask *> &mapping();

    coro_task * _inner;
};
}
