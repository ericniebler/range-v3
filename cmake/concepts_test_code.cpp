template<class>
concept bool True = true;

template<class T>
constexpr bool test(T)
{
    return false;
}

template<True T>
constexpr bool test(T)
{
    return true;
}

int main()
{
    static_assert(::test(42), "");
}
