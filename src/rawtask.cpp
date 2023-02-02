#include <coro/loop.hpp>
#include <coro/rawtask.hpp>

namespace coro {

RawTask::RawTask(coro_task_entry_f func, void *arg)
{
    _inner = coro_create_task(nullptr, func, arg);
    mapping().insert(std::make_pair(_inner, this));
}

RawTask::RawTask(Loop &loop, coro_task_entry_f func, void *arg)
{
    _inner = coro_create_task(loop.inner(), func, arg);
    mapping().insert(std::make_pair(_inner, this));
}

RawTask::~RawTask()
{
    if (_inner) {
        coro_cancel_task(_inner);
        mapping().erase(_inner);
    }
}

RawTask *RawTask::current()
{
    return from_raw(coro_current_task());
}

RawTask *RawTask::from_raw(coro_task *r)
{
    return mapping().at(r);
}

Loop *RawTask::parent()
{
    return Loop::from_raw(coro_task_parent(_inner));
}

bool RawTask::running()
{
    return coro_task_running(_inner);
}

int RawTask::cancel()
{
    int rc = coro_cancel_task(_inner);

    if (rc == 0) {
        mapping().erase(_inner);
        _inner = nullptr;
    }
    return rc;
}

std::string_view RawTask::name()
{
    return coro_task_name(_inner);
}

int RawTask::set_name(std::string_view name)
{
    return coro_task_set_name(_inner, name.data());
}

void *RawTask::join()
{
    void *res = coro_task_join(_inner);

    mapping().erase(_inner);
    _inner = nullptr;
    return res;
}

void *RawTask::await()
{
    void *val;

    if (coro_await_val(_inner, &val)) {
        mapping().erase(_inner);
        _inner = nullptr;
    }

    return val;
}

bool RawTask::await_val(void **val)
{
    bool rc = coro_await_val(_inner, val);

    if (rc) {
        mapping().erase(_inner);
        _inner = nullptr;
    }

    return rc;
}

void RawTask::yeild()
{
    return coro_yeild();
}

void RawTask::yeild_val(void *val)
{
    return coro_yeild_val(val);
}

void RawTask::sleep(uintmax_t amnt)
{
    return coro_sleep(amnt);
}

void RawTask::sleepms(uintmax_t amnt)
{
    return coro_sleepms(amnt);
}

void RawTask::wait_custom(coro_wait_custom_f poll, void *cb_data)
{
    return coro_wait_custom(poll, cb_data);
}

void RawTask::wait_fd(int fd, int how)
{
    return coro_wait_fd(fd, how);
}

long RawTask::read(int fd, void *buf, unsigned long len)
{
    return coro_read(fd, buf, len);
}

long RawTask::write(int fd, const void *buf, unsigned long len)
{
    return coro_write(fd, buf, len);
}

int RawTask::accept(int sock, struct sockaddr *addr, socklen_t *addrlen)
{
    return coro_accept(sock, addr, addrlen);
}

long RawTask::recv(int sock, void *buf, unsigned long len, int flags)
{
    return coro_recv(sock, buf, len, flags);
}

long RawTask::recvfrom(int sock, void * buf, unsigned long len,
                       int flags, struct sockaddr * src_addr,
                       socklen_t * addrlen)
{
    return coro_recvfrom(sock, buf, len, flags, src_addr, addrlen);
}

long RawTask::recvmsg(int sock, struct msghdr *msg, int flags)
{
    return coro_recvmsg(sock, msg, flags);
}

long RawTask::send(int sock, const void *buf, unsigned long len, int flags)
{
    return coro_send(sock, buf, len, flags);
}

long RawTask::sendto(int sock, const void *buf, unsigned long len, int flags,
                     const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return coro_sendto(sock, buf, len, flags, dest_addr, addrlen);
}

long RawTask::sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    return coro_sendmsg(sockfd, msg, flags);
}

coro_task *RawTask::inner()
{
    return _inner;
}

std::unordered_map<coro_task *, RawTask *> &RawTask::mapping()
{
    static std::unordered_map<coro_task *, RawTask*> _mapping;

    return _mapping;
}

}
