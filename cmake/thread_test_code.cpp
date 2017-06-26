#include <thread>

int main() {
    return (void)std::this_thread::get_id(), 0;
}
