#include "badalloc.h"

bool compare(const std::pair<size_t, Pool>& lhs, const std::pair<size_t, Pool>& rhs) {
    return lhs.first < rhs.first;
};
