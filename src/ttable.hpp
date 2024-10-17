#pragma once

#include <optional>
#include <vector>

#include "chessboard.hpp"
#include "magic_numbers.hpp"
#include "move.hpp"
#include "tunable.hpp"
#include "utils.hpp"

class TranspositionTable;
extern TranspositionTable tt;

constexpr int TT_CLUSTER_SIZE = 3;
constexpr int AGE_BITS = 6;
constexpr int AGE_MOD = 1 << AGE_BITS;
constexpr int AGE_MASK = powi(2, AGE_BITS) - 1;

TUNABLE_SPECIFIER TunableInt tt_depth_offset = TUNABLE_INT("tt_depth_offset", 5, 3, 6);
TUNABLE_SPECIFIER TunableInt tt_cluster_relative_age = TUNABLE_INT("tt_cluster_relative_age", 2, 1, 4);

enum class BoundTypes : uint8_t {
    NONE = 0,
    LOWER_BOUND = 1,
    UPPER_BOUND = 2,
    EXACT_BOUND = 3,
};

class TranspositionTableEntry {
    private:
        uint16_t _key;
        Score _score;
        Score _static_eval;
        Move pv_move;
        uint8_t _depth;
        uint8_t _age : AGE_BITS;
        BoundTypes _bound : 2;

        friend class TranspositionTable;
        void set_score(Score new_score) { this->_score = new_score; };
        void set_move(Move new_move) { this->pv_move = new_move; };
        void set_age(uint8_t new_age) { assert(new_age < AGE_MOD); _age = new_age; };
    public:
        TranspositionTableEntry() : _key(0), pv_move(Move::NULL_MOVE()), _depth(0), _age(0), _bound(BoundTypes::NONE) {};
        TranspositionTableEntry(Move pv_move, uint8_t depth, BoundTypes bound, Score score, Score static_eval, ZobristKey key) : _key(key), _score(score), _static_eval(static_eval), pv_move(pv_move), _depth(depth), _age(0), _bound(bound) {};

        Move move() const { return this->pv_move; };
        uint8_t depth() const { return this->_depth; };
        uint8_t age() const { return this->_age; };
        BoundTypes bound_type() const { return this->_bound; };
        Score score() const { return this->_score; };
        Score static_eval() const { return this->_static_eval; };
        uint16_t key() const { return this->_key; };
};

struct Cluster {
    std::array<TranspositionTableEntry, TT_CLUSTER_SIZE> entries;
    uint16_t _padding;
};

class TranspositionTable {
    private:
        std::vector<Cluster> table;
        uint8_t current_age = 0;
    public:
        TranspositionTable() {
            this->resize(16);
        };

        uint64_t tt_index(const ZobristKey key) const { return static_cast<uint64_t>((static_cast<__uint128_t>(key) * static_cast<__uint128_t>(table.size())) >> 64); };

        void store(TranspositionTableEntry new_entry, const Position& pos) {

            const auto key = pos.zobrist_key();
            auto& cluster = table[tt_index(key)];

            std::optional<std::reference_wrapper<TranspositionTableEntry>> entry = std::nullopt;
            auto min_val = std::numeric_limits<int32_t>::max();

            for (auto& candidate : cluster.entries) {
                if (candidate.key() == key || candidate.bound_type() == BoundTypes::NONE) {
                    entry = std::optional(std::ref(candidate));
                    break;
                }

                const auto relative_age = (AGE_MOD + current_age - candidate.age()) & AGE_MASK;
                const auto entry_value = candidate.depth() - relative_age * 2;

                if (entry_value < min_val) {
                    min_val = entry_value;
                    entry = std::optional(std::ref(candidate));
                }
            }

            assert(entry.has_value());

            if (!(
                   new_entry.bound_type() == BoundTypes::EXACT_BOUND // Replace if the new one is an exact bound
                || entry->get().key() != new_entry.key() // Or doesn't match the existing key
                || entry->get().age() != current_age // Or the entry wasn't inserted this search
                || new_entry.depth() + tt_depth_offset > entry->get().depth()
            )) {
                return;
            }

            if (new_entry.move().is_null_move()) {
                new_entry.set_move(entry->get().move());
            }
            
            entry->get() = new_entry;
        }

        std::optional<std::reference_wrapper<const TranspositionTableEntry>> probe(const Position& pos) const {
            const auto tt_key = pos.zobrist_key();
            const auto tt_idx = tt_index(tt_key);
            const auto& cluster = table[tt_idx];
            for (const auto& elem : cluster.entries) {
                if (elem.key() == tt_key) {
                    return std::optional(std::ref(elem));
                }
            }
            return std::nullopt;
        }

        void clear() {
            std::fill(table.begin(), table.end(), Cluster());
        }

        void resize(size_t mb_size) {
            table.resize((mb_size * 1024 * 1024) / sizeof(Cluster));
            clear();
        }

        void prefetch(const ZobristKey key) const {
            __builtin_prefetch(&table[tt_index(key)]);
        }

        void age() {
            current_age = (current_age + 1) % AGE_MOD;
        }
};

static_assert(sizeof(TranspositionTableEntry) == 10);
static_assert(sizeof(Cluster) == 32);