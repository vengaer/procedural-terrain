template <typename P0, typename... P1toN>
std::common_type_t<P0, P1toN...> math::max(P0 p0, P1toN... p1ton) {
    static_assert(std::is_arithmetic_v<P0> && (std::is_arithmetic_v<P1toN> && ...),
                  "Arguments must be numeric");
    if constexpr(sizeof...(P1toN) == 0u)
        return p0;
    else {
        auto rest = max(p1ton...);
        return p0 > rest ? p0 : rest;
    }
}

template <typename P0, typename... P1toN>
std::common_type_t<P0, P1toN...> math::min(P0 p0, P1toN... p1ton) {
    static_assert(std::is_arithmetic_v<P0> && (std::is_arithmetic_v<P1toN> && ...),
                  "Arguments must be numeric");
    if constexpr(sizeof...(P1toN) == 0u)
        return p0;
    else {
        auto rest = min(p1ton...);
        return p0 < rest ? p0 : rest;
    }
}

template <typename T>
std::decay_t<T> gcd(T num, T den) {

    T swp;
    while(den != static_cast<T>(0)) {
        swp = den;
        den = num % den;
        num = swp;
    }

    if(num == static_cast<T>(0))
        throw UndefinedOperationException{"GCD of 0/0 is undefined"};

    return num;
}
