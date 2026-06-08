/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Clock.hpp - Time unit system
*/
#pragma once

#include <cstdint>
#include <chrono>

namespace Zappy {

    // A point in time (nanoseconds since some fixed epoch)
    using TimePoint = std::chrono::steady_clock::time_point;

    // A duration in nanoseconds
    using Duration  = std::chrono::nanoseconds;

    class Clock {
    public:
        // Returns the current time
        static TimePoint now();

        // Returns a duration corresponding to (actionCost / freq) seconds
        // e.g. actionCost=7, freq=100 -> 70ms
        static Duration actionDuration(int actionCost, int freq);

        // Returns a deadline = now() + actionDuration(actionCost, freq)
        static TimePoint deadline(int actionCost, int freq);

        // Returns true if the given deadline has passed
        static bool hasPassed(const TimePoint &tp);

        // Returns how many milliseconds remain until a deadline (0 if passed)
        static int64_t msUntil(const TimePoint &tp);
    };

} // namespace Zappy