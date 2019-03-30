#ifndef CT_MATH_H
#define CT_MATH_H

#pragma once
#include "traits.h"
#include <cstddef>
#include <numeric>
#include <type_traits>

namespace math {
    namespace ct{
        namespace impl {
            template <long long Prod, long long Base, long long Exp>
            struct pow_impl : std::conditional_t<Exp % 2 != 0,
                                                 pow_impl<Prod*Base, Base*Base, (Exp >> 1)>,
                                                 pow_impl<Prod, Base*Base, (Exp >> 1)>> { };


            template <long long Prod, long long Base>
            struct pow_impl<Prod, Base, 0> : std::integral_constant<long long, Prod> { };

        }
        
        template <long long N>
        struct abs : std::integral_constant<long long, N < 0 ? -N : N> {
            static_assert(N != std::numeric_limits<long long>::lowest());
        };

        /* O(log n) complexity */
        template <long long Base, long long Exp>
        struct pow : impl::pow_impl<1, Base, Exp> { 
            static_assert(Exp >= 0);
        };

        namespace impl {
            template <std::size_t Low, std::size_t High>
            struct mid : std::integral_constant<std::size_t, (High + Low) / 2> { };

            template <std::size_t N, std::size_t Low, std::size_t High>
            struct sqrt_impl : std::conditional_t<(pow<mid<Low,High>::value, 2>::value < N),
                                                  sqrt_impl<N, mid<Low, High>::value + 1u, High>,
                                                  sqrt_impl<N, Low, mid<Low, High>::value>> { };

            template <std::size_t N, std::size_t I>
            struct sqrt_impl<N, I, I> : std::integral_constant<std::size_t, I> { };

            template <std::size_t N, std::size_t Low, std::size_t High>
            struct cbrt_impl : std::conditional_t<(pow<mid<Low,High>::value, 3>::value < N),
                                                  cbrt_impl<N, mid<Low, High>::value + 1u, High>,
                                                  cbrt_impl<N, Low, mid<Low, High>::value>> { };

            template <std::size_t N, std::size_t I>
            struct cbrt_impl<N, I, I> : std::integral_constant<std::size_t, I> { };

            template <std::size_t P0, std::size_t P1, std::size_t N>
            struct fibonacci_impl : fibonacci_impl<P1, P0 + P1, N-1> { };

            template <std::size_t P0, std::size_t P1>
            struct fibonacci_impl<P0, P1, 0> : std::integral_constant<unsigned long long, P1> { };
        }


        /* O(log n) complexity */
        template <std::size_t N>
        struct sqrt : impl::sqrt_impl<N, 1, N>{};

        template <std::size_t N>
        struct cbrt : impl::cbrt_impl<N, 1, N>{};

        template <long long M, long long N>
        struct hypot : std::integral_constant<std::size_t, sqrt<pow<M, 2>::value + pow<N, 2>::value>::value> { };

        template <long long M, long long N>
        struct dim : std::integral_constant<std::size_t, abs<M-N>::value> { };

        template <long long P0, long long... P1toN>
        struct max : std::integral_constant<long long, (P0 > max<P1toN...>::value) ? P0 : max<P1toN...>::value> { };

        template <long long P0>
        struct max<P0> : std::integral_constant<long long, P0> { };

        template <long long P0, long long... P1toN>
        struct min : std::integral_constant<long long, (P0 < min<P1toN...>::value) ? P0 : max<P1toN...>::value> { };

        template <long long P0>
        struct min<P0> : std::integral_constant<long long, P0> { };

        template <long long N>
        struct factorial : std::integral_constant<long long, N * factorial<N-1>::value> { };

        template <>
        struct factorial<0> : std::integral_constant<long long, 1> { };

        template <long long M ,long long N>
        struct gcd : gcd<N, M % N> { };

        template <long long M>
        struct gcd<M, 0> : std::integral_constant<long long, M> {
            static_assert(M != 0);
        };

        /* N-th fibonacci sequence, 0-indexed */
        template <std::size_t N>
        struct fibonacci : impl::fibonacci_impl<0, 1, N-1> { };

        template <>
        struct fibonacci<0> : std::integral_constant<unsigned long long, 0> { };
        
        template <>
        struct fibonacci<1> : std::integral_constant<unsigned long long, 1> { };


        template <long long N>
        inline long long constexpr abs_v = abs<N>::value;

        template <long long Base, long long Exp>
        inline long long constexpr pow_v = pow<Base, Exp>::value;

        template <std::size_t N>
        inline std::size_t constexpr sqrt_v = sqrt<N>::value;

        template <std::size_t N>
        inline std::size_t constexpr cbrt_v = cbrt<N>::value;

        template <long long M, long long N>
        inline std::size_t constexpr hypot_v = hypot<M, N>::value; 

        template <long long M, long long N>
        inline std::size_t constexpr dim_v = dim<M, N>::value;

        template <long long... P0toN>
        inline long long constexpr max_v = max<P0toN...>::value;

        template <long long... P0toN>
        inline long long constexpr min_v = min<P0toN...>::value;

        template <long long N>
        inline long long factorial_v = factorial<N>::value;

        template <long long M, long long N>
        inline long long gcd_v = gcd<M, N>::value;

        template <std::size_t N>
        inline unsigned long long constexpr fibonacci_v = fibonacci<N>::value;
    }
}

#endif
