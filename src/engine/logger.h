#ifndef LOGGER_H
#define LOGGER_H
#ifdef LOG_DISABLE_ALL
#define LOG(...)
#define LOG_WARN(...)
#define LOG_CRIT(...)
#define O_LOG(...)
#define O_LOG_WARN(...)
#define O_LOG_CRIT(...)
#define E_LOG(...)
#define E_LOG_WARN(...)
#define E_LOG_CRIT(...)
#else

#pragma once
#include "exception.h"
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>


struct FileLoggingTag { };
struct StdLoggingTag { };
namespace logging {
	enum class Severity { Debug, Warning, Critical };
	std::string to_string(Severity S);
	enum class Stream { Ofstream, Stdout, Stderr };
	enum class Detail { Default, Verbose };
}

template <typename T, typename U>
using enable_on_match_t = std::enable_if_t<std::is_same_v<T, U>>;

class LoggerBase {
		using Severity = logging::Severity;
	protected:
		Severity last_{Severity::Debug};
};

template <typename, typename = void>
class FileLogger : public LoggerBase { };

template <typename T>
class FileLogger<T, enable_on_match_t<T, FileLoggingTag>> : public LoggerBase{
	public:
		FileLogger();
		~FileLogger();
	protected:
		std::ofstream ofs_;
		std::mutex ofs_mutex_;
		std::size_t ofs_entry_{};

	private:
		void init();
};



template <typename LoggingPolicy, logging::Detail D = logging::Detail::Default>
class Logger : public FileLogger<LoggingPolicy> {
		using Severity = logging::Severity;
	public:
		Logger();

		template <logging::Stream St, logging::Severity S, typename... Args>
		void print(Args&&... args);

		static std::string get_time();

		template <Severity S>
		static void format(std::ostream& os, std::size_t entry_num, std::mutex& mtx);
	private:
		static std::mutex cout_mutex_, cerr_mutex_;
		std::size_t std_entry_{};

		template <typename Tuple, std::size_t... Is>
		static void print(Tuple const& t, std::index_sequence<Is...>, std::ostream& os, std::mutex& mtx);
};


#ifdef LOG_FULL_VERBOSE
static Logger<FileLoggingTag, logging::Detail::Verbose> logger_instance;
#elif defined(LOG_FULL_DEFAULT)
static Logger<FileLoggingTag, logging::Detail::Default> logger_instance;
#elif defined(LOG_STD_ERROUT_VERBOSE)
static Logger<StdLoggingTag, logging::Detail::Verbose> logger_instance;
#else
static Logger<StdLoggingTag, logging::Detail::Default> logger_instance;
#endif
#if defined(LOG_FULL_VERBOSE) || defined(LOG_FULL_DEFAULT)
#define LOG logger_instance.print<logging::Stream::Ofstream, logging::Severity::Debug>
#define LOG_WARN logger_instance.print<logging::Stream::Ofstream, logging::Severity::Warning>
#define LOG_CRIT logger_instance.print<logging::Stream::Ofstream, logging::Severity::Critical>
#endif

#define O_LOG logger_instance.print<logging::Stream::Stdout, logging::Severity::Debug>
#define O_LOG_WARN logger_instance.print<logging::Stream::Stdout, logging::Severity::Warning>
#define O_LOG_CRIT logger_instance.print<logging::Stream::Stdout, logging::Severity::Critical>

#define E_LOG logger_instance.print<logging::Stream::Stderr, logging::Severity::Debug>
#define E_LOG_WARN logger_instance.print<logging::Stream::Stderr, logging::Severity::Warning>
#define E_LOG_CRIT logger_instance.print<logging::Stream::Stderr, logging::Severity::Critical>

#include "logger.tcc"
#endif
#endif
