#ifndef LOGGER_H
#define LOGGER_H
#ifdef LOG_DISABLE_ALL
/* Log file */
#define LOG(...)
#define LOG_WARN(...)
#define LOG_CRIT(...)
/* std::cout */
#define O_LOG(...)
#define O_LOG_WARN(...)
#define O_LOG_CRIT(...)
/* std::cerr */
#define E_LOG(...)
#define E_LOG_WARN(...)
#define E_LOG_CRIT(...)
/* Log file and std::cerr */
#define ERR_LOG(...)
#define ERR_LOG_WARN(...)
#define ERR_LOG_CRIT(...)
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
	using ErrOutLoggingTag = void;

	enum class Label { Debug, Warning, Critical };
	std::string to_string(Label S);
	enum class Ostream { OfStream, StdOut, StdErr };


	class LoggerBase {
		protected:
			Label last_{Label::Debug};
	};

	template <typename, typename = ErrOutLoggingTag>
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



	template <typename LoggingTag>
	class Logger : public FileLogger<LoggingTag> {
		public:
			Logger();

			template <Ostream OS, Label S, typename... Args>
			void print(Args&&... args);

			static std::string get_time();
			static std::string get_date();

			template <Label S>
			static void format(std::ostream& os, std::size_t entry_num, std::mutex& mtx);
		private:
			static std::mutex cout_mutex_, cerr_mutex_;
			std::size_t std_entry_{};

			static std::time_t raw_time();

			template <typename Tuple, std::size_t... Is>
			static void print(Tuple const& t, std::index_sequence<Is...>, std::ostream& os, std::mutex& mtx);

	};


	#if defined(LOG_FULL_VERBOSE) || defined(LOG_FULL_DEFAULT)
	inline Logger<FileLoggingTag> logger_instance;		/* inline non-static => same address in all TUs */
	#else
	inline Logger<ErrOutLoggingTag> logger_instance;
	#endif
}
#define CALL_TRACE "<Invoked in function \'", __FUNCTION__, "\' on line ", __LINE__, " in file \'", __FILE__ "\'> : "

#ifdef LOG_FULL_VERBOSE
#define LOG(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Debug>(CALL_TRACE, __VA_ARGS__)
#define LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Warning>(CALL_TRACE, __VA_ARGS__)
#define LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Critical>(CALL_TRACE, __VA_ARGS__)

#define ERR_LOG(...) LOG(__VA_ARGS__); E_LOG(__VA_ARGS__)
#define ERR_LOG_WARN(...) LOG_WARN(__VA_ARGS__); E_LOG_WARN(__VA_ARGS__)
#define ERR_LOG_CRIT(...) LOG_CRIT(__VA_ARGS__); E_LOG_CRIT(__VA_ARGS__)
#elif defined(LOG_FULL_DEFAULT)
#define LOG(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Debug>(__VA_ARGS__)
#define LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Warning>(__VA_ARGS__)
#define LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::OfStream, logging::Label::Critical>(__VA_ARGS__)

#define ERR_LOG(...) LOG(__VA_ARGS__); E_LOG(__VA_ARGS__)
#define ERR_LOG_WARN(...) LOG_WARN(__VA_ARGS__); E_LOG_WARN(__VA_ARGS__);
#define ERR_LOG_CRIT(...) LOG_CRIT(__VA_ARGS__); E_LOG_CRIT(__VA_ARGS__);
#else
#define LOG(...)
#define LOG_WARN(...)
#define LOG_CRIT(...)
#define ERR_LOG(...) E_LOG(__VA_ARGS__)
#define ERR_LOG_WARN(...) E_LOG_WARN(__VA_ARGS__)
#define ERR_LOG_CRIT(...) E_LOG_CRIT(__VA_ARGS__)
#endif

#if defined(LOG_FULL_VERBOSE) || defined(LOG_ERROUT_VERBOSE)
#define O_LOG(...) logging::logger_instance.template print<logging::Ostream::StdOut, logging::Label::Debug>(CALL_TRACE, __VA_ARGS__)
#define O_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::StdOut, logging::Label::Warning>(CALL_TRACE, __VA_ARGS__)
#define O_LOG_CRIT(...) logging::logger_instance.print<logging::Ostream::StdOut, logging::Label::Critical>(CALL_TRACE, __VA_ARGS__)

#define E_LOG(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Debug>(CALL_TRACE, __VA_ARGS__)
#define E_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Warning>(CALL_TRACE, __VA_ARGS__)
#define E_LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Critical>(CALL_TRACE, __VA_ARGS__)
#else
#define O_LOG(...) logging::logger_instance.template print<logging::Ostream::StdOut, logging::Label::Debug>(__VA_ARGS__)
#define O_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::StdOut, logging::Label::Warning>(__VA_ARGS__)
#define O_LOG_CRIT(...) logging::logger_instance.print<logging::Ostream::StdOut, logging::Label::Critical>(__VA_ARGS__)

#define E_LOG(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Debug>(__VA_ARGS__)
#define E_LOG_WARN(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Warning>(__VA_ARGS__)
#define E_LOG_CRIT(...) logging::logger_instance.template print<logging::Ostream::StdErr, logging::Label::Critical>(__VA_ARGS__)
#endif

#include "logger.tcc"
#endif
#endif
