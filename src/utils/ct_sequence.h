#ifndef CT_SEQUENCE_H
#define CT_SEQUENCE_H

#pragma once
#include "traits.h"
#include <cstddef>
#include <type_traits>

template <std::size_t...>
struct odd_index_sequence { };

template <std::size_t N, std::size_t... Seq>
struct odd_seq_gen : std::conditional_t<N % 2 != 0,
									odd_seq_gen<N-1, Seq...>,
									odd_seq_gen<N-1, N-1, Seq...>> { };

template <std::size_t... Seq>
struct odd_seq_gen<0u, Seq...> : type_is<odd_index_sequence<Seq...>> { };

template <std::size_t N>
using make_odd_index_sequence = typename odd_seq_gen<N>::type;

template <typename... Args>
using odd_index_sequence_for = make_odd_index_sequence<sizeof...(Args)>;

template <std::size_t...>
struct even_index_sequence { };

template <std::size_t N, std::size_t... Seq>
struct even_seq_gen : std::conditional_t<N % 2 == 0,
									even_seq_gen<N-1, Seq...>,
									even_seq_gen<N-1, N-1, Seq...>> { };

template <std::size_t... Seq>
struct even_seq_gen<0u, Seq...> : type_is<even_index_sequence<Seq...>> { };

template <std::size_t N>
using make_even_index_sequence = typename even_seq_gen<N>::type;

template <typename... Args>
using even_index_sequence_for = make_even_index_sequence<sizeof...(Args)>;
	

#endif
