#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include "meta_utility.h"
#include <utility>


template <typename T, typename = std::enable_if_t<is_contiguously_stored_v<decltype(std::declval<T>().get())>>>
class Renderer {
	public:
	
	private:
};

#include "renderer.tcc"
#endif
