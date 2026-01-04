#pragma once

#include <atomic>

#include "../utils.hpp"

/* Custom barrier impl by https://https://github.com/Sp00ph, adapted by me */

class Barrier {
    std::atomic<u32> waiting;
    std::atomic<u32> gen;
    const u32 n;

public:
    explicit Barrier(u32 n) : waiting(0), gen(0), n(n) {}
    Barrier(const Barrier&) = delete;
    Barrier& operator=(const Barrier&) = delete;

    void wait() {
        uint32_t gen = this->gen.load(std::memory_order_relaxed);
        if (this->waiting.fetch_add(1, std::memory_order_acq_rel) == n - 1) {
            // if num waiting == n
            this->waiting.store(0, std::memory_order_relaxed);
            this->gen.fetch_add(1, std::memory_order_relaxed);
            this->gen.notify_all();
            // reset waiting count, increment gen and wake all
        } else {
            while (this->gen.load(std::memory_order_relaxed) == gen) {
                this->gen.wait(gen, std::memory_order_relaxed);
                // else wait and loop to handle spurious wakeups
            }
        }
    }
};