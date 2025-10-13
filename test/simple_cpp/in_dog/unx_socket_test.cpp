#include <catch2/catch_test_macros.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <chrono>
#include <thread>
#include <print>

#include <simple_cpp/in_dog/unix_socket.hpp>

Fd unixSocketClientConnect(std::string_view address) {
  Fd fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (!fd.isValid()) {
    throw std::runtime_error("Failed to create unix socket");
  }
  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;

  if (address.size() + 1 > sizeof(addr.sun_path)) {
    throw std::runtime_error("Cannot bind: address too long");
  }
  std::ranges::copy(address, addr.sun_path);

  auto ret = ::connect(fd.get(), reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
  if (ret == -1) {
    throw std::runtime_error("Failed to connect: error " + std::to_string(errno));
  }
  return fd;
}

TEST_CASE("client", "[unix_socket]") {
  const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );
  std::string path{"/tmp/in-dog-test."};
  path.append(std::to_string(epoch.count()));
  bool server_success = false;
  std::thread server_thread(
    [&]() {
      try {
        UnixServerSocket socket{path};
        Fd accept = socket.accept();
        server_success = accept.writeBuf(std::string("hello"));
      } catch (std::runtime_error& e) {
        std::print("error in server thread: {}", e.what());
        server_success = false;
      }
    }
  );

  // wait till server is ready to accept connections
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  Fd client = unixSocketClientConnect(path);
  std::vector<char> buff;
  buff.resize(100);
  const size_t received = client.read(buff);
  buff.resize(received);
  REQUIRE( std::string(buff.begin(), buff.end()) == "hello" );
  server_thread.join();
  REQUIRE( server_success == true);

  REQUIRE( unlink(path.c_str()) == -1 ); // expecting server to cleanup
}
