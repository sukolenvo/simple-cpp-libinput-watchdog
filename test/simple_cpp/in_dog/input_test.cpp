#include <catch2/catch_test_macros.hpp>

#include "simple_cpp/in_dog/input.hpp"

TEST_CASE("input cleanup", "[input]") {
  SECTION("not started") {
    Input();
  }

  SECTION("started") {
    Input input{};
    input.start();
  }
}
