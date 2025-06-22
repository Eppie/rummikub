#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip> // For std::fixed and std::setprecision

// To enable tracing, define ENABLE_PERFORMANCE_TRACING before including this header,
// or pass it as a compiler flag (e.g., -DENABLE_PERFORMANCE_TRACING)

#ifdef ENABLE_PERFORMANCE_TRACING

namespace PerformanceTracer {

struct FunctionProfile {
    long long total_nanoseconds;
    long long call_count;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time; // Temporary for nested calls

    FunctionProfile() : total_nanoseconds(0), call_count(0) {}
};

// Global map to store function profiles
inline std::map<std::string, FunctionProfile>& get_profiles() {
    static std::map<std::string, FunctionProfile> profiles;
    return profiles;
}

class TimeGuard {
public:
    TimeGuard(const std::string& function_name)
        : name(function_name), start_time(std::chrono::high_resolution_clock::now()) {
        get_profiles()[name].call_count++;
    }

    ~TimeGuard() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        get_profiles()[name].total_nanoseconds += duration.count();
    }

private:
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

inline void print_performance_report() {
    std::cout << "\n--- Performance Report ---" << std::endl;
    std::cout << std::left << std::setw(50) << "Function"
              << std::right << std::setw(15) << "Call Count"
              << std::setw(25) << "Total Time (ns)"
              << std::setw(25) << "Avg Time/Call (ns)" << std::endl;
    std::cout << std::string(115, '-') << std::endl;

    for (const auto& pair : get_profiles()) {
        const std::string& name = pair.first;
        const FunctionProfile& profile = pair.second;
        long double avg_time_per_call = 0;
        if (profile.call_count > 0) {
            avg_time_per_call = static_cast<long double>(profile.total_nanoseconds) / profile.call_count;
        }

        std::cout << std::left << std::setw(50) << name
                  << std::right << std::setw(15) << profile.call_count
                  << std::setw(25) << profile.total_nanoseconds
                  << std::setw(25) << std::fixed << std::setprecision(2) << avg_time_per_call
                  << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

// Macro to create a TimeGuard object easily
#define TRACE_FUNCTION() PerformanceTracer::TimeGuard time_guard_obj(__func__) // __func__ gives the current function name

} // namespace PerformanceTracer

#else // ENABLE_PERFORMANCE_TRACING is not defined

// Define macros as no-ops if tracing is disabled
#define TRACE_FUNCTION()
namespace PerformanceTracer {
    inline void print_performance_report() {
        // Do nothing if tracing is disabled
        // std::cout << "\nPerformance tracing was disabled at compile time." << std::endl;
    }
} // namespace PerformanceTracer

#endif // ENABLE_PERFORMANCE_TRACING
