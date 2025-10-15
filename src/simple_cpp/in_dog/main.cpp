#include <print>

#include "app.hpp"

int main(int argc, char** argv) {
  App app{};
  try {
    std::string_view path = argc > 1 ? argv[1] : "/run/in-dog.socket";
    app.run(path);
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    std::println(stderr, "Received error {}", e.what());
    return EXIT_FAILURE;
  }
}
