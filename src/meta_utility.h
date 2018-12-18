#ifndef TMP_UTILITY_H
#define TMP_UTILITY_H

#pragma once
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
struct is_contiguously_stored : std::false_type { };

/* std::vector */
template <typename T>
struct is_contiguously_stored<std::vector<T>> : std::true_type { };

/* std::array */
template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N>> : std::true_type { };

/* Bounded array */
template <typename T, std::size_t N>
struct is_contiguously_stored<T[N]> : std::true_type { };


/* Check if container is c array, default no */
template <typename T>
struct is_c_array : std::false_type { };

/* C array true */
template <typename T, std::size_t N>
struct is_c_array<T[N]> : std::true_type { };

/* Unbounded c array true */
template <typename T>
struct is_c_array<T[]> : std::true_type { };


/* Alias templates */
template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

template <typename T>
inline bool constexpr is_c_array_v = is_c_array<T>::value;

#endif
