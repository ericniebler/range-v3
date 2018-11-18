#include <new>

int main() {
    struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__ * 4) S {};
    (void) ::operator new(sizeof(S), std::align_val_t{alignof(S)});
}
