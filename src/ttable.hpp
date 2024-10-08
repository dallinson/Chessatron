#pragma once

#include <vector>

#include "chessboard.hpp"
#include "magic_numbers.hpp"
#include "move.hpp"
#include "tunable.hpp"
#include "utils.hpp"

class TranspositionTable;
extern TranspositionTable tt;

TUNABLE_SPECIFIER TunableInt tt_depth_offset = TUNABLE_INT("tt_depth_offset", 5, 3, 6);

enum class BoundTypes : uint8_t {
    NONE = 0,
    LOWER_BOUND = 1,
    UPPER_BOUND = 2,
    EXACT_BOUND = 3,
};

class TranspositionTableEntry {
    private:
        ZobristKey _key;
        Score _score;
        Score _static_eval;
        Move pv_move;
        uint8_t _depth;
        BoundTypes _bound;

        friend class TranspositionTable;
        void set_score(Score new_score) { this->_score = new_score; };
        void set_move(Move new_move) { this->pv_move = new_move; };
    public:
        TranspositionTableEntry() : _key(0), pv_move(Move::NULL_MOVE()), _depth(0), _bound(BoundTypes::NONE) {};
        TranspositionTableEntry(Move pv_move, uint8_t depth, BoundTypes bound, Score score, Score static_eval, ZobristKey key) : _key(key), _score(score), _static_eval(static_eval), pv_move(pv_move), _depth(depth), _bound(bound) {};

        Move move() const { return this->pv_move; };
        uint8_t depth() const { return this->_depth; };
        BoundTypes bound_type() const { return this->_bound; };
        Score score() const { return this->_score; };
        Score static_eval() const { return this->_static_eval; };
        ZobristKey key() const { return this->_key; };
};

class TranspositionTable {
    private:
        std::vector<TranspositionTableEntry> table;

    public:
        TranspositionTable() {
            this->resize(16);
        };
        uint64_t tt_index(const ZobristKey key) const { return static_cast<uint64_t>((static_cast<__uint128_t>(key) * static_cast<__uint128_t>(table.size())) >> 64); };
        const TranspositionTableEntry& operator[](const Position& key) const { return table[tt_index(key.zobrist_key())]; };
        void store(TranspositionTableEntry entry, const Position& key) {
            const auto tt_key = tt_index(key.zobrist_key());
            if (table[tt_key].key() != entry.key()
                || entry.bound_type() == BoundTypes::EXACT_BOUND
                || entry.depth() + tt_depth_offset > table[tt_key].depth()) {
                if (entry.score() <= MagicNumbers::NegativeInfinity + MAX_PLY) {
                    // If we're being mated
                    entry.set_score(MagicNumbers::NegativeInfinity);
                } else if (entry.score() >= MagicNumbers::PositiveInfinity - MAX_PLY) {
                    // If we have mate
                    entry.set_score(MagicNumbers::PositiveInfinity);
                }
                if (entry.move().is_null_move()) {
                    entry.set_move(table[tt_key].move());
                }
                table[tt_key] = entry;
            }
        }
        void clear() {
            std::fill(table.begin(), table.end(), TranspositionTableEntry());
        }
        void resize(size_t mb_size) {
            table.resize((mb_size * 1024 * 1024) / sizeof(TranspositionTableEntry));
            clear();
        }
        void prefetch(const ZobristKey key) const {
            __builtin_prefetch(&table[tt_index(key)]);
        }
};
