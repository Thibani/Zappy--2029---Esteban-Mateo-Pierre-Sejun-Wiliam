/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Clock.cpp - Time unit system
*/
#include "utils/clock.hpp"

#include <algorithm>

namespace Zappy {

    TimePoint Clock::now()
    {
        return std::chrono::steady_clock::now();
    }

    Duration Clock::actionDuration(int actionCost, int freq)
    {
        // (actionCost / freq) seconds expressed as nanoseconds
        // Use integer arithmetic to avoid floating point imprecision:
        // actionCost * 1_000_000_000 ns / freq
        int64_t ns = (static_cast<int64_t>(actionCost) * 1'000'000'000LL) / freq;
        return std::chrono::nanoseconds(ns);
    }

    TimePoint Clock::deadline(int actionCost, int freq)
    {
        return now() + actionDuration(actionCost, freq);
    }

    bool Clock::hasPassed(const TimePoint &tp)
    {
        return now() >= tp;
    }

    int64_t Clock::msUntil(const TimePoint &tp)
    {
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(tp - now());
        return std::max(remaining.count(), int64_t(0));
    }

} // namespace Zappy