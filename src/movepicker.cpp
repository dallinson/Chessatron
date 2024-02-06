#include "movepicker.hpp"

#include <algorithm>
#include <array>

#include "evaluation.hpp"
#include "move_generator.hpp"
#include "search.hpp"

constexpr std::array<uint8_t, 6> ordering_scores = { 1, 2, 3, 4, 5, 6 };

MovePicker::MovePicker(MoveList&& moves, const ChessBoard& board, const Move pv_move, std::array<int32_t, 8192>& history_table, Move killer_move, bool& found_pv_move) {
    this->move_list = moves;
    this->sorted_idx = 0;
    found_pv_move = false;
    for (size_t i = 0; i < this->move_list.len(); i++) {
        this->move_list[i].score = 0;
        if (this->move_list[i].move == pv_move) {
            found_pv_move = true;
            this->move_list[i].score = std::numeric_limits<int32_t>::max();
            continue;
        } else if (this->move_list[i].move.is_promotion()) {
            switch (this->move_list[i].move.get_promotion_piece_type()) {
            case PieceTypes::QUEEN:
                this->move_list[i].score = 2000000001;
                break;
            case PieceTypes::KNIGHT:
                this->move_list[i].score = 2000000000;
                break;
            default:
                this->move_list[i].score = -2000000001;
                break;
            }
        } else if (this->move_list[i].move.is_capture()) {
            this->move_list[i].score = 900000000;
            this->move_list[i].see_ordering_result = Search::static_exchange_evaluation(board, this->move_list[i].move, -20);
            if (!this->move_list[i].see_ordering_result) {
                this->move_list[i].score = -1000000;
            }
            const auto src_score = ordering_scores[static_cast<uint8_t>(board.get_piece(this->move_list[i].move.get_src_square()).get_type()) - 1];
            const auto dest_type = this->move_list[i].move.get_move_flags() == MoveFlags::EN_PASSANT_CAPTURE ? PieceTypes::PAWN : board.get_piece(this->move_list[i].move.get_dest_square()).get_type();
            const auto dest_score = ordering_scores[static_cast<uint8_t>(dest_type) - 1];
            this->move_list[i].score += ((100000 * dest_score) + (6 - src_score));
        } else if (this->move_list[i].move == killer_move) {
            this->move_list[i].score = 800000000;
        } else {
            this->move_list[i].score += history_table[this->move_list[i].move.get_history_idx(board.get_side_to_move())];
        }
    }
}

ScoredMove MovePicker::next_move() {
    if (this->sorted_idx >= this->move_list.len()) {
        ScoredMove to_return;
        to_return.move = Move::NULL_MOVE;
        return to_return;
    }
    size_t best_move_idx = this->sorted_idx;
    int32_t best_score = this->move_list[sorted_idx].score;
    for (size_t i = this->sorted_idx+1; i < this->move_list.len(); i++) {
        if (move_list[i].score > best_score) {
            best_score = move_list[i].score;
            best_move_idx = i;
        }
    }
    std::swap(this->move_list[this->sorted_idx], this->move_list[best_move_idx]);
    this->sorted_idx++;
    return this->move_list[sorted_idx - 1];
}

size_t MovePicker::size() {
    return this->move_list.len();
}