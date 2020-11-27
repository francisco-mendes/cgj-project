#pragma once

#include <chrono>
#include <iostream>

template <class T>
using OptPtr = T*;

template <class T>
using Ptr = T*;

template <bool Pred, class F>
using ReturnsVoid = std::enable_if_t<std::is_void_v<std::invoke_result_t<F>> == Pred>;

template <class Run, typename = ReturnsVoid<false, Run>>
auto logTimeTaken(std::string_view const name, Run run)
{
    using namespace std::chrono;
    auto const start = steady_clock::now();
    auto       temp  = run();
    auto const end   = steady_clock::now();
    auto const diff  = end - start;
    std::cerr << name << " took " << duration_cast<milliseconds>(diff).count() << " ms." << std::endl;
    return temp;
}

template <class Run, typename = ReturnsVoid<true, Run>>
void logTimeTaken(std::string_view const name, Run run)
{
    using namespace std::chrono;
    auto const start = steady_clock::now();
    run();
    auto const end  = steady_clock::now();
    auto const diff = end - start;
    std::cerr << name << " took " << duration_cast<milliseconds>(diff).count() << " ms." << std::endl;
}

