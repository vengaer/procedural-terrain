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
#include "traits.h"
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


namespace logging {
	struct FileLoggingTag { };
	struct OSdLoggingTag { };

	enum class Severity { Debug, Warning, Critical };
	std::string to_string(Severity S);
	enum class Ostream { Ofstream, Stdout, Stderr };


	class LoggerBase {
			using Severity = logging::Severity;
		protected:
			Severity last_{Severity::Debug};
	};

	template <typename, typename = void>
	class FileLogger : public LoggerBase { };

	template <typename T>
	class FileLogger<T, enable_on_match_t<T, FileLoggingTag>> : public LoggerBase{
		protected:
			std::ofstream ofs_;
			std::mutex ofs_mutex_;
			std::size_t ofs_entry_{};

			FileLogger();
			~FileLogger();
		private:
			void init();
	};



	template <typename LoggingPolicy>
	class Logger : public FileLogger<LoggingPolicy> {
			using Severity = logging::Severity;
		public:
			Logger();

			template <logging::Ostream OS, logging::Severity S, typename... Args>
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


	#if defined(LOG_FULL_VERBOSE) || defined(LOG_FULL_DEFAULT)
	static Logger<FileLoggingTag> logger_instance;
	#else
	static Logger<OSdLoggingTag> logger_instance;
	#endif
}
#define CALL_TRACE "<Invoked in function \'", __FUNCTION__, "\' on line ", __LINE__, " in file \'", __FILE__ "\'> : "

#ifdef LOG_FULL_VERBOSE
#define LOG(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Debug>(CALL_TRACE, __VA_ARGS__)
#define LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Warning>(CALL_TRACE, __VA_ARGS__)
#define LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Critical>(CALL_TRACE, __VA_ARGS__)
#elif defined(LOG_FULL_DEFAULT)
#define LOG(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Debug>(__VA_ARGS__)
#define LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Warning>(__VA_ARGS__)
#define LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::Ofstream, logging::Severity::Critical>(__VA_ARGS__)
#else
#define LOG(...)
#define LOG_WARN(...)
#define LOG_CRIT(...)
#endif

#if defined(LOG_FULL_VERBOSE) || defined(LOG_ERROUT_VERBOSE)
#define O_LOG(...) logging::logger_instance.template print<logging::Ostream::Stdout, logging::Severity::Debug>(CALL_TRACE, __VA_ARGS__)
#define O_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Stdout, logging::Severity::Warning>(CALL_TRACE, __VA_ARGS__)
#define O_LOG_CRIT(...) logging::logger_instance.print<logging::Ostream::Stdout, logging::Severity::Critical>(CALL_TRACE, __VA_ARGS__)

#define E_LOG(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Debug>(CALL_TRACE, __VA_ARGS__)
#define E_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Warning>(CALL_TRACE, __VA_ARGS__)
#define E_LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Critical>(CALL_TRACE, __VA_ARGS__)
#else
#define O_LOG(...) logging::logger_instance.template print<logging::Ostream::Stdout, logging::Severity::Debug>(__VA_ARGS__)
#define O_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Stdout, logging::Severity::Warning>(__VA_ARGS__)
#define O_LOG_CRIT(...) logging::logger_instance.print<logging::Ostream::Stdout, logging::Severity::Critical>(__VA_ARGS__)

#define E_LOG(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Debug>(__VA_ARGS__)
#define E_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Warning>(__VA_ARGS__)
#define E_LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::Stderr, logging::Severity::Critical>(__VA_ARGS__)
#endif

#include "logger.tcc"
#endif
#endif
