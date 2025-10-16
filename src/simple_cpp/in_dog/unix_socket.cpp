#include <print>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>

#include "unix_socket.hpp"

UnixServerSocket::UnixServerSocket(std::string_view address) : shutdown_(EventFd::open()), address_(address) {
}

std::unique_ptr<UnixSocket> UnixServerSocket::accept() {
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
    auto mask_before = umask(0);
    auto ret = bind(fd.get(), reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
    umask(mask_before);
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

  int ready = poll(shutdown_.getFd(), *unix_socket_);
  if (ready == -1 || ready == 0) {
    return {};
  }
  Fd fd = ::accept(unix_socket_->get(), nullptr, nullptr);

  if (!fd.isValid()) {
    std::println(stderr, "Failed to accept connection: error {}", errno);
    return {};
  }
  return std::make_unique<UnixSocket>(std::move(fd));
}

UnixServerSocket::~UnixServerSocket() {
  shutdown_.signal();
  if (unix_socket_ != nullptr) {
    unlink(address_.c_str());
  }
  unix_socket_.reset();
}

UnixSocket makeClientSocket(std::string_view address) {
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
  return UnixSocket{std::move(fd)};
}

void UnixSocket::sendFd(const Fd& fd) const {
  msghdr msg{};
  char dummy_data = '\n';
  std::array<iovec, 1> payload{&dummy_data, 1};
  msg.msg_iov = payload.data();
  msg.msg_iovlen = payload.size();

  std::vector<char> control_buf;
  control_buf.resize(CMSG_SPACE(sizeof(int)));
  msg.msg_control = control_buf.data();
  msg.msg_controllen = control_buf.size();
  cmsghdr* cmsg_header = CMSG_FIRSTHDR(&msg);
  cmsg_header->cmsg_level = SOL_SOCKET;
  cmsg_header->cmsg_type = SCM_RIGHTS;
  cmsg_header->cmsg_len = CMSG_LEN(sizeof(int));
  int* fds = reinterpret_cast<int*>(CMSG_DATA(cmsg_header));
  *fds = fd.get();

  if (sendmsg(fd_.get(), &msg, 0) == -1) {
    throw std::runtime_error("Failed to send fd: " + std::to_string(errno));
  }
}

Fd UnixSocket::receiveFd() const {
  msghdr msg{};
  char received_data;
  std::array<iovec, 1> payload{&received_data, 1};
  msg.msg_iov = payload.data();
  msg.msg_iovlen = payload.size();
  std::array<char, 100> cbuff{};
  msg.msg_control = cbuff.data();
  msg.msg_controllen = cbuff.size();
  if (recvmsg(fd_.get(), &msg, 0) == -1) {
    throw std::runtime_error("Failed to receive fd: " + std::to_string(errno));
  }
  if (msg.msg_controllen == 0) {
    throw std::runtime_error("No msg controls received");
  }
  for (cmsghdr* cmsg_header = CMSG_FIRSTHDR(&msg); cmsg_header != nullptr; cmsg_header = CMSG_NXTHDR(&msg, cmsg_header)) {
    if (cmsg_header->cmsg_type == SCM_RIGHTS) {
      if (cmsg_header->cmsg_len != CMSG_LEN(sizeof(int))) {
        throw std::runtime_error("Unexpected payload size: " + std::to_string(cmsg_header->cmsg_len));
      }
      int* data = reinterpret_cast<int*>(CMSG_DATA(cmsg_header));
      return *data;
    }
  }
  throw std::runtime_error("Failed to receive valid control message");
}
