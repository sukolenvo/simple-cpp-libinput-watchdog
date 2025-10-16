#include <catch2/catch_test_macros.hpp>

#include "simple_cpp/in_dog/app.hpp"

TEST_CASE("app cleanup", "[app]") {
  SECTION("not started") {
    App();
  }

  SECTION("started") {
    std::unique_ptr<App> app = std::make_unique<App>();
    std::thread thread([&app]() {
      const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
      );
      std::string path{"/tmp/app-test."};
      path.append(std::to_string(epoch.count()));
      app->run(path);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    app.reset();
    thread.join();
  }
}
