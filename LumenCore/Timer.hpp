//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_TIMER_HPP
#define LUMEN_TIMER_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <utility>

namespace Lumen {

    class Timer {
    public:
        Timer() { Reset(); }

        void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

        auto Elapsed() -> float {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - m_Start)
                           .count()
                   * 0.001F * 0.001F * 0.001F;
        }

        auto ElapsedMillis() -> float { return Elapsed() * 1000.0F; }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };

    class [[maybe_unused]] ScopedTimer {
    public:
        [[maybe_unused]] explicit ScopedTimer(std::string name) : m_Name(std::move(name)) {}

        ~ScopedTimer() {
            float const time = m_Timer.ElapsedMillis();
            std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
        }

    private:
        std::string m_Name;
        Timer m_Timer;
    };


}// namespace Lumen

#endif// LUMEN_TIMER_HPP