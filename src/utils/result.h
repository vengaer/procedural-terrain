#ifndef RESULT_H
#define RESULT_H

#pragma once
#include <optional>
#include <tuple>
#include <variant>

enum class Outcome { Failure, Success, End_ };

template <typename... Args>	
struct Result{
	Outcome outcome{};
	std::tuple<Args...> data{};
};

template <typename... Args>
struct Result<std::variant<Args...>> {
	Outcome outcome{};
	std::variant<Args...> data{};
};

template <typename T>
struct Result<std::optional<T>> {
	Outcome outcome{};
	std::optional<T> data{};
};

#endif
