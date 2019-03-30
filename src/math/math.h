#ifndef MATH_H
#define MATH_H

#pragma once
#include "constants.h"
#include "traits.h"
#include <type_traits>

namespace math {
    template <typename P0, typename... P1toN>
    std::common_type_t<P0, P1toN...> max(P0 p0, P1toN... p1ton);

    template <typename P0, typename... P1toN>
    std::common_type_t<P0, P1toN...> min(P0 p0, P1toN... p1ton);

    namespace angle {
        double to_radians(double degrees);
        double to_degrees(double radians);
    }
}

#include "math.tcc"
#endif
