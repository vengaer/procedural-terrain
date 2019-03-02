template <typename T, typename U>
ColorType<T, U>::ColorType() noexcept : x{{}}, y{{}}, z{{}}, w{{}} { }

template <typename T, typename U>
ColorType<T, U>::ColorType(T all) noexcept : x{all}, y{all}, z{all}, w{all} { }

template <typename T, typename U>
ColorType<T, U>::ColorType(T p, T q, T r, T s) noexcept : x{p}, y{q}, z{r}, w{s} { }

template <typename T, typename U>
ColorType<T, U>::ColorType(std::array<T, 4> const& arr) noexcept : x{arr[0]}, y{arr[1]}, z{arr[2]}, w{arr[3]} { }
