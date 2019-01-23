template <typename T>
logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::FileLogger() : LoggerBase{}, ofs_{}, ofs_mutex_{}, ofs_entry_{1u} {
	init();
}

template <typename T>
void logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::init() {
	namespace fs = std::filesystem;
	fs::path log_dir = fs::current_path();
	log_dir += "/logs/";

	if(!fs::is_directory(log_dir))
		if(std::error_code err{}; !fs::create_directory(log_dir, err))
			throw FileSystemException{err.message()};

	std::string time = Logger<T>::get_time();

	ofs_.open(log_dir.string() + time + ".log");
	if(!ofs_.is_open())
		throw FileIOException{"Unable to open " + log_dir.string() +  time + ".log for writing"};

	Logger<T>::template format<Label::Debug>(ofs_, ofs_entry_++, ofs_mutex_);
	last_ = Label::Debug;

	ofs_mutex_.lock();
	ofs_ << "Initiating program\n";
	ofs_mutex_.unlock();
}

template <typename T>
logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::~FileLogger() {
	Logger<T>::template format<Label::Debug>(ofs_, ofs_entry_++, ofs_mutex_);

	ofs_mutex_.lock();
	ofs_ << "Terminating program with last known status <" << to_string(last_) << ">\n";
	ofs_mutex_.unlock();

	ofs_.close();
}

template <typename LoggingTag>
logging::Logger<LoggingTag>::Logger() : FileLogger<LoggingTag>{}, std_entry_{1u} { }

template <typename LoggingTag>
template <logging::Ostream OS, logging::Label S, typename... Args>
void logging::Logger<LoggingTag>::print(Args&&... args) {
	if constexpr(OS == Ostream::StdOut) {
		format<S>(std::cout, std_entry_++, cout_mutex_);
		print(std::forward_as_tuple(args...), std::index_sequence_for<Args...>{}, std::cout,  cout_mutex_);
	}
	else if constexpr(OS == Ostream::StdErr){
		format<S>(std::cerr, std_entry_++, cerr_mutex_);
		print(std::forward_as_tuple(args...), std::index_sequence_for<Args...>{}, std::cerr,  cerr_mutex_);
	}
	else if constexpr(OS == Ostream::OfStream) {
		format<S>(this->ofs_, this->ofs_entry_++, this->ofs_mutex_);
		print(std::forward_as_tuple(args...), std::index_sequence_for<Args...>{}, this->ofs_,  this->ofs_mutex_);
	}
	this->last_ = S;
}

template <typename LoggingTag>
template <typename Tuple, std::size_t... Is>
void logging::Logger<LoggingTag>::print(Tuple const& t, std::index_sequence<Is...>, std::ostream& os, std::mutex& mtx) {
	mtx.lock();
	((os << std::get<Is>(t)), ...);
	os << "\n";
	mtx.unlock();
}

template <typename LoggingTag>
std::string logging::Logger<LoggingTag>::get_time() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::ostringstream os;
	os << std::put_time(std::localtime(&now_c), "%T %F");
	return os.str();
}

template <typename LoggingTag>
template <logging::Label S>
void logging::Logger<LoggingTag>::format(std::ostream& os, std::size_t entry_num, std::mutex& mtx) {
	mtx.lock();
	os << std::setfill('0') << std::setw(7) << entry_num
	   << " <" << get_time() << ">" << " - " << "<" << to_string(S) << "> : ";
	mtx.unlock();
}

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cout_mutex_{};

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cerr_mutex_{};
