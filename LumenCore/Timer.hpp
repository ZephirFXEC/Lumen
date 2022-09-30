//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_TIMER_HPP
#define LUMEN_TIMER_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <utility>

namespace Lumen {

    class Timer {
    public:
        Timer() {
            Reset();
        }

        void Timer::Reset() {
            m_Start = std::chrono::high_resolution_clock::now();
        }

        float Timer::Elapsed() {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
        }

        float Timer::ElapsedMillis() {
            return Elapsed() * 1000.0f;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };

    class ScopedTimer {
    public:
        explicit ScopedTimer(std::string name)
                : m_Name(std::move(name)) {}

        ~ScopedTimer() {
            float time = m_Timer.ElapsedMillis();
            std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
        }

    private:
        std::string m_Name;
        Timer m_Timer;
    };


}

#endif //LUMEN_TIMER_HPP