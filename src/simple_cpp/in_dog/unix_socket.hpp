#ifndef SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP
#define SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP

#include <memory>
#include <string>
#include <string_view>

#include "fd.hpp"

class UnixSocket {
  public:
    explicit UnixSocket(Fd&& fd) : fd_(std::move(fd)) {}
    void sendFd(const Fd& fd) const;

    Fd receiveFd() const;

  private:
    Fd fd_;
};

class UnixServerSocket {
  public:
    explicit UnixServerSocket(std::string_view address);
    UnixSocket accept();

    UnixServerSocket(const UnixServerSocket&) = delete;
    UnixServerSocket(UnixServerSocket&& other) noexcept = default;
    UnixServerSocket& operator=(const UnixServerSocket& other) = delete;
    UnixServerSocket& operator=(UnixServerSocket&& other) noexcept = default;
    ~UnixServerSocket();

  private:
    std::string address_;
    std::unique_ptr<Fd> unix_socket_;
};

UnixSocket makeClientSocket(std::string_view address);

#endif //SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP
