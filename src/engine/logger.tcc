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

	std::string date = Logger<T>::get_date();

	std::string log_file = log_dir.string() + date + ".log";

	ofs_.open(log_file, std::ios::app);
	if(!ofs_.is_open())
		throw FileIOException{"Unable to open " + log_dir.string() +  date + ".log for writing"};

	Logger<T>::template format<Label::Debug>(ofs_, ofs_entry_++, ofs_mutex_);
	last_ = Label::Debug;

	std::lock_guard<std::mutex> lock{ofs_mutex_};
	ofs_ << "Initiating program\n";
}

template <typename T>
logging::FileLogger<T, enable_on_match_t<T, logging::FileLoggingTag>>::~FileLogger() {
	Logger<T>::template format<Label::Debug>(ofs_, ofs_entry_++, ofs_mutex_);

	std::lock_guard<std::mutex> lock{ofs_mutex_};
	ofs_ << "Terminating program with last known status <" << to_string(last_) << ">\n";

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
	std::lock_guard<std::mutex> lock{mtx};
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
void logging::Logger<LoggingTag>::format(std::ostream& os, std::size_t entry_num, std::mutex& mtx) {
	std::lock_guard<std::mutex> lock{mtx};
	os << std::setfill('0') << std::setw(7) << entry_num
	   << " <" << get_time() << ">" << " - " << "<" << to_string(S) << "> : ";
}

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cout_mutex_{};

template <typename LoggingTag>
std::mutex logging::Logger<LoggingTag>::cerr_mutex_{};
