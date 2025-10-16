#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>
#include <print>

#include <simple_cpp/in_dog/unix_socket.hpp>

#include "simple_cpp/in_dog/event_fd.hpp"

TEST_CASE("send event fd", "[unix_socket]") {
  const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );
  std::string path{"/tmp/unix_socket-test."};
  path.append(std::to_string(epoch.count()));
  bool server_success = false;
  std::thread server_thread(
    [&]() {
      try {
        UnixServerSocket socket{path};
        std::unique_ptr<UnixSocket> accept = socket.accept();
        EventFd event_fd = EventFd::open();
        REQUIRE(event_fd.signal());
        accept->sendFd(event_fd.getFd());
        server_success = true;
      } catch (std::runtime_error& e) {
        std::print("error in server thread: {}", e.what());
        server_success = false;
      }
    }
  );

  // wait till server is ready to accept connections
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  UnixSocket client = makeClientSocket(path);
  EventFd event_fd{client.receiveFd()};
  event_fd.wait();
  server_thread.join();
  REQUIRE(server_success == true);

  REQUIRE(unlink(path.c_str()) == -1); // expecting server to cleanup
}

TEST_CASE("unix server socket cleanup", "[unix_socket]") {
  SECTION("not started") {
    UnixServerSocket{""};
  }

  SECTION("accept") {
    const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    );
    std::string path{"/tmp/unix_socket-test."};
    path.append(std::to_string(epoch.count()));
    std::unique_ptr<UnixServerSocket> socket = std::make_unique<UnixServerSocket>(path);
    std::thread thread(
      [&]() {
        auto client = socket->accept();
        REQUIRE(client == nullptr);
      }
    );
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    socket.reset();
    thread.join();
  }
}
