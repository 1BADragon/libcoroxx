#pragma once

#include<tuple>

namespace coro {

template<typename P, typename F, typename ...Args>
decltype(auto) inner_passthrough(P p, F f, Args ...args)
{

}

}
