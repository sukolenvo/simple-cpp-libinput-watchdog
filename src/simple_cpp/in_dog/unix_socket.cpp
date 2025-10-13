#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>

#include "unix_socket.hpp"

UnixServerSocket::UnixServerSocket(std::string_view address) : address_(address) {
}

Fd UnixServerSocket::accept() {
  if (unix_socket_ == nullptr) {
    Fd fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (!fd.isValid()) {
      throw std::runtime_error("Failed to create unix socket");
    }
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;

    if (address_.size() + 1 > sizeof(addr.sun_path)) {
      throw std::runtime_error("Cannot bind: address too long");
    }
    std::ranges::copy(address_, addr.sun_path);
    unlink(address_.c_str());
    auto ret = bind(fd.get(), reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
    if (ret != 0) {
      throw std::runtime_error("Cannot bind: error " + std::to_string(errno));
    }
    ret = listen(fd.get(), 5);
    if (ret == -1) {
      unlink(address_.c_str());
      throw std::runtime_error("Cannot listen: error " + std::to_string(errno));
    }
    unix_socket_ = std::make_unique<Fd>(std::move(fd));
  }

  Fd fd = ::accept(unix_socket_->get(), nullptr, nullptr);

  if (!fd.isValid()) {
    throw std::runtime_error("Failed to accept: error " + std::to_string(errno));
  }
  return fd;
}

UnixServerSocket::~UnixServerSocket() {
  if (unix_socket_ != nullptr) {
    unlink(address_.c_str());
  }
  unix_socket_.reset();
}