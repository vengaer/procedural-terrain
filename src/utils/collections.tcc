template <typename Container, typename = std::void_t<decltype(std::begin(std::declval<Container>()))>>
auto constexpr opt_c_begin(Container&& c) {
	if constexpr(std::is_const_v<std::remove_reference_t<Container>>)
		return std::cbegin(c);
	else
		return std::begin(c);
}

template <typename Container, typename = std::void_t<decltype(std::end(std::declval<Container>()))>>
auto constexpr opt_c_end(Container&& c) {
	if constexpr(std::is_const_v<std::remove_reference_t<Container>>)
		return std::cend(c);
	else
		return std::end(c);
}

/* zip_iterator */
template <typename... Iters>
zip_iterator<Iters...>::zip_iterator(iter_tuple_t&& iters) : iters_{std::move(iters)} { }

template <typename... Iters>
zip_iterator<Iters...>& zip_iterator<Iters...>::operator++() {
	increment(std::index_sequence_for<Iters...>{});
	return *this;
}

template <typename... Iters>
zip_iterator<Iters...> zip_iterator<Iters...>::operator++(int) {
	auto old = *this;
	++*this;
	return old;
}

template <typename... Iters>
typename zip_iterator<Iters...>::reference zip_iterator<Iters...>::operator*() {
	return dereference(std::index_sequence_for<Iters...>{});
}

template <typename... Iters>
bool zip_iterator<Iters...>::operator==(zip_iterator const& rhs) const {
	return compare(iters_, rhs.iters_, std::index_sequence_for<Iters...>{});
}

template <typename... Iters>
bool zip_iterator<Iters...>::operator!=(zip_iterator const& rhs) const {
	return !(*this == rhs);
}

template <typename... Iters>
template <std::size_t... Is>
void zip_iterator<Iters...>::increment(std::index_sequence<Is...>) {
	(++std::get<Is>(iters_), ...);
}

template <typename... Iters>
template <std::size_t... Is>
typename zip_iterator<Iters...>::ref_tuple_t zip_iterator<Iters...>::dereference(std::index_sequence<Is...>) {
	return ref_tuple_t{*std::get<Is>(iters_)...};
}

template <typename... Iters>
template <typename Tuple, std::size_t... Is>
bool zip_iterator<Iters...>::compare(Tuple const& l, Tuple const& r, std::index_sequence<Is...>) {
	return ((std::get<Is>(l) == std::get<Is>(r)) || ...);
}

/* zip_collection */
template <typename... Iters>
zip_collection<Iters...>::zip_collection(iter_tuple_t&& begins, iter_tuple_t&& ends) : begins_{std::move(begins)}, ends_{std::move(ends)} { }

template <typename... Iters>
typename zip_collection<Iters...>::iterator zip_collection<Iters...>::begin() {
	return begins_;
}

template <typename... Iters>
typename zip_collection<Iters...>::iterator zip_collection<Iters...>::end() {
	return ends_;
}

/* zip */
template <typename... Args>
zip_collection<get_iterator_t<Args>...> zip(Args&&... args) {
	using collection_t = zip_collection<get_iterator_t<Args>...>;
	using iter_tuple_t = typename collection_t::iter_tuple_t;
	return collection_t{iter_tuple_t{opt_c_begin(args)...}, iter_tuple_t{opt_c_end(args)...}};
}

/* fold_iterator */
template <typename Iter, typename Op>
fold_iterator<Iter, Op>::fold_iterator(Iter it, value_type&& init, Op op) : it_{it}, current_{std::move(init)}, op_{op} { }

template <typename Iter, typename Op>
fold_iterator<Iter, Op>& fold_iterator<Iter, Op>::operator++() {
	current_ = op_(current_, *it_++);
	return *this;
}

template <typename Iter, typename Op>
fold_iterator<Iter, Op> fold_iterator<Iter, Op>::operator++(int) {
	auto old = *this;
	++*this;
	return old;
}

template <typename Iter, typename Op>
typename fold_iterator<Iter, Op>::reference fold_iterator<Iter, Op>::operator*() {
	return op_(current_, *it_);
}

template <typename Iter, typename Op>
bool fold_iterator<Iter, Op>::operator==(fold_iterator const& rhs) const {
	return it_ == rhs.it_;
}

template <typename Iter, typename Op>
bool fold_iterator<Iter, Op>::operator!=(fold_iterator const& rhs) const {
	return !(*this == rhs);
}

/* fold_collection */
template <typename Container, typename Op>
fold_collection<Container, Op>::fold_collection(Container const& c, value_type&& init, Op op) : begin_{std::cbegin(c), std::move(init), op}, end_{std::cend(c), value_type{}, op} { }

template <typename Container, typename Op>
typename fold_collection<Container, Op>::iterator fold_collection<Container, Op>::begin() {
	return begin_;
}

template <typename Container, typename Op>
typename fold_collection<Container, Op>::iterator fold_collection<Container, Op>::end() {
	return end_;
}

/* fold */

template <typename Container, typename Op = std::plus<value_type_t<Container>>>
fold_collection<Container, Op> fold(Container const& c, value_type_t<Container> init, Op op) {
	return fold_collection{c, std::move(init), op};
} 

/* enumerate_iterator */
template <typename Iter>
enumerate_iterator<Iter>::enumerate_iterator(Iter it) : it_{it}, idx_{0u}  { }

template <typename Iter>
enumerate_iterator<Iter>& enumerate_iterator<Iter>::operator++() {
	++idx_;
	++it_;
	return *this;
}

template <typename Iter>
enumerate_iterator<Iter> enumerate_iterator<Iter>::operator++(int) {
	auto old = *this;
	++*this;
	return old;
}

template <typename Iter>
typename enumerate_iterator<Iter>::reference enumerate_iterator<Iter>::operator*() {
	if constexpr(is_const_iterator_v<Iter>)
		return std::make_pair(idx_, std::cref(*it_));

	return std::make_pair(idx_, std::ref(*it_));
}

template <typename Iter>
bool enumerate_iterator<Iter>::operator==(enumerate_iterator const& rhs) const {
	return it_ == rhs.it_;
}

template <typename Iter>
bool enumerate_iterator<Iter>::operator!=(enumerate_iterator const& rhs) const {
	return !(*this == rhs);
}

/* enumerate_collection */
template <typename Iter>
enumerate_collection<Iter>::enumerate_collection(Iter begin, Iter end) : begin_{begin}, end_{end} { }

template <typename Iter>
typename enumerate_collection<Iter>::iterator enumerate_collection<Iter>::begin() {
	return begin_;
}

template <typename Iter>
typename enumerate_collection<Iter>::iterator enumerate_collection<Iter>::end() {
	return end_;
}

/* enumerate */
template <typename Container>
enumerate_collection<get_iterator_t<Container>> enumerate(Container&& c) {
	using collection_t = enumerate_collection<get_iterator_t<Container>>;
	return collection_t{opt_c_begin(c), opt_c_end(c)};
}
