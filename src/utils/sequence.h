#ifndef SEQUENCE_H
#define SEQUENCE_H

#pragma once
#include "traits.h"
#include <cstddef>
#include <type_traits>

template <typename Numeric, Numeric...>
struct odd_integer_sequence { };

namespace impl {
    template <typename Numeric, std::size_t N, Numeric... Seq>
    struct odd_seq_gen : std::conditional_t<N % 2 != 0,
                                            odd_seq_gen<Numeric, N-1, Seq...>,
                                            odd_seq_gen<Numeric, N-1, N-1, Seq...>> { };

    template <typename Numeric, Numeric... Seq>
    struct odd_seq_gen<Numeric, 0u, Seq...> : type_is<odd_integer_sequence<Numeric, Seq...>> { };
}

template <typename Numeric, std::size_t N>
using make_odd_integer_sequence = typename impl::odd_seq_gen<Numeric, N>::type;

template <typename Numeric, typename... Args>
using odd_integer_sequence_for = make_odd_integer_sequence<Numeric, sizeof...(Args)>;

template <std::size_t... Is>
using odd_index_sequence = odd_integer_sequence<std::size_t, Is...>;

template <std::size_t N>
using make_odd_index_sequence = make_odd_integer_sequence<std::size_t, N>;

template <typename... Args>
using odd_index_sequence_for = odd_integer_sequence_for<std::size_t, Args...>;

template <typename Numeric, Numeric...>
struct even_integer_sequence { };

namespace impl {
    template <typename Numeric, std::size_t N, Numeric... Seq>
    struct even_seq_gen : std::conditional_t<N % 2 == 0,
                                             even_seq_gen<Numeric, N-1, Seq...>,
                                             even_seq_gen<Numeric, N-1, N-1, Seq...>> { };

    template <typename Numeric, Numeric... Seq>
    struct even_seq_gen<Numeric, 0u, Seq...> : type_is<even_integer_sequence<Numeric, Seq...>> { };
}

template <typename Numeric, std::size_t N>
using make_even_integer_sequence = typename impl::even_seq_gen<Numeric, N>::type;

template <typename Numeric, typename... Args>
using even_integer_sequence_for = make_even_integer_sequence<Numeric, sizeof...(Args)>;

template <std::size_t... Is>
using even_index_sequence = even_integer_sequence<std::size_t, Is...>;

template <std::size_t N>
using make_even_index_sequence = make_even_integer_sequence<std::size_t, N>;

template <typename... Args>
using even_index_sequence_for = even_integer_sequence_for<std::size_t, Args...>;


#endif
