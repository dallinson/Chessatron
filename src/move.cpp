#include "move.hpp"

#include <string>

bool operator==(const Move& lhs, const Move& rhs) { return lhs.value() == rhs.value(); }
