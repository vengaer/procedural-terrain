template <typename T>
logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::FileLogger() : LoggerBase{}, ofs_entry_{1u} {
	init();
}


template <typename T>
logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::~FileLogger() {
	#ifndef RESTRICT_THREAD_USAGE
	should_compress_ = false;
	LOG("Detaching compressor thread with id ", compressor_thread_.get_id());
	{
		std::lock_guard<std::mutex> lock{ofs_mutex_};
		compressor_thread_.detach();
	}
	#endif

	LOG("Terminating program with last known status <", to_string(last_),">");
	std::lock_guard<std::mutex> lock{ofs_mutex_};
	ofs_.close();
	
}

template <typename T>
void logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::init() {
	namespace fs = std::filesystem;
	fs::path log_dir = fs::current_path();
	log_dir += "/logs/";

	if(!fs::is_directory(log_dir))
		if(std::error_code err{}; !fs::create_directory(log_dir, err))
			throw FileSystemException{err.message()};

	std::string date = Logger<T>::get_date();

	log_file_ = log_dir.string() + date + ".log";

	ofs_.open(log_file_, std::ios::app);
	if(!ofs_.is_open())
		throw FileIOException{"Unable to open " + log_file_ + ".log for writing"};


	LOG("Initiating program");

	{
		std::lock_guard<std::mutex> lock{ofs_mutex_};
		last_ = Label::Debug;
	}
	#ifndef RESTRICT_THREAD_USAGE
	LOG("Creating separate thread for log file compression");
	std::lock_guard<std::mutex> lock{ofs_mutex_};
	compressor_thread_ = std::thread{std::bind(&FileLogger::monitor_log_file, this)};
	#endif
}

template <typename T>
void logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::monitor_log_file() {
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(60s);

	while(should_compress_) {
		{
			is_compressing_ = true;
			std::lock_guard<std::mutex> lock{ofs_mutex_};
			compress_content();
			is_compressing_ = false;
			std::lock_guard<std::mutex> lock_buf{buffer_mutex_};	
			ofs_ << buffer_.rdbuf();
			buffer_.str(std::string{});
		}

		std::this_thread::sleep_for(60s);
	}
}

template <typename T>
void logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::compress_content() {
	LOG("Compressing log file");
	ofs_.close();
	std::ifstream ifs{log_file_};
	if(!ifs.is_open()) {
		LOG("Failed to open log file for reading, aborting...");
		return;
	}

	std::vector<std::string> data;
	{
		std::string line;
		while(std::getline(ifs, line))
			data.push_back(line);
		
	}
	
	ifs.close();

	data.erase(
		std::unique(std::begin(data),
					std::end(data),
					[](auto const& str1, auto const str2) {

			std::string const str1_label = extract_label(str1);
			std::string const str2_label = extract_label(str2);
			if(str1_label == "Critical" || str2_label == "Critical")
				return false;

			return str1_label == str2_label && extract_content(str1) == extract_content(str2);
		}),
		std::end(data));

	std::size_t start_idx = 0u;
	if(std::size_t size = data.size(); size > LOG_SIZE)
		start_idx = size - LOG_SIZE;
	
	ofs_.open(log_file_);
	if(!ofs_.is_open()) {
		ERR_LOG_WARN("Unable to reopen ",  log_file_, " after compression.");
		return;
	}

	auto start = std::begin(data);
	std::advance(start, start_idx);

	std::copy(start, std::end(data), std::ostream_iterator<std::string>(ofs_, "\n"));
} 
template <typename T>
std::size_t logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::LOG_SIZE = 500u;

template <typename LoggingTag>
logging::Logger<LoggingTag>::Logger() : FileLogger<LoggingTag>{}, std_entry_{1u} { }

template <typename LoggingTag>
template <logging::Ostream OS, logging::Label S, typename... Args>
void logging::Logger<LoggingTag>::print(Args&&... args) {
	if constexpr(OS == Ostream::StdOut) {
		std::lock_guard<std::mutex> lock{cout_mutex_};
		format<S>(std::cout, std_entry_++);
		print(std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{}, std::cout);
	}
	else if constexpr(OS == Ostream::StdErr){
		std::lock_guard<std::mutex> lock{cerr_mutex_};
		format<S>(std::cerr, std_entry_++);
		print(std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{}, std::cerr);
	}
	else if constexpr(OS == Ostream::OfStream) {
		if(!this->is_compressing_) {
			std::lock_guard<std::mutex> lock{this->ofs_mutex_};
			format<S>(this->ofs_, this->ofs_entry_++);
			print(std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{}, this->ofs_);
		}
		else { /* Currently compressing file, write to buffer and carry on */
			std::lock_guard<std::mutex> lock{this->buffer_mutex_};
			format<S>(this->buffer_, this->ofs_entry_++);
			print(std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{}, this->buffer_);
		}
	}
	this->last_ = S;
}

template <typename LoggingTag>
template <typename Tuple, std::size_t... Is>
void logging::Logger<LoggingTag>::print(Tuple const& t, std::index_sequence<Is...>, std::ostream& os) {
	((os << std::get<Is>(t)), ...);
	os << "\n";
}

template <typename LoggingTag>
std::string logging::Logger<LoggingTag>::get_time() {
	std::time_t now = raw_time();
	std::ostringstream os;
	os << std::put_time(std::localtime(&now), "%T %F");
	return os.str();
}

template <typename LoggingTag>
std::string logging::Logger<LoggingTag>::get_date() {
	std::time_t now = raw_time();
	std::ostringstream os;
	os << std::put_time(std::localtime(&now), "%F");
	return os.str();
}

template <typename LoggingTag>
std::time_t logging::Logger<LoggingTag>::raw_time() {
	auto now = std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(now);
}


template <typename LoggingTag>
template <logging::Label S>
void logging::Logger<LoggingTag>::format(std::ostream& os, std::size_t entry_num) {
	os << std::setfill('0') << std::setw(7) << entry_num
	   << " <" << get_time() << "> - <" << to_string(S) << "> : ";
}

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cout_mutex_{};

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cerr_mutex_{};
