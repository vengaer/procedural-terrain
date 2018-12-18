#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
struct is_contiguously_stored : std::false_type { };

template <typename T>
struct is_contiguously_stored<std::vector<T>> : std::true_type { };

template <typename T, std::size_t N>
struct is_contiguously_stored<std::array<T,N>> : std::true_type { };

template <typename T>
inline bool constexpr is_contiguously_stored_v = is_contiguously_stored<T>::value;

template <typename T, typename = std::enable_if_t<is_contiguously_stored_v<decltype(std::declval<T>().get())>>>
class Renderer {

};

#include "renderer.tcc"
#endif
