#ifndef SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP
#define SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP

#include <memory>
#include <string>
#include <string_view>

#include "fd.hpp"

class UnixServerSocket {
  public:
    explicit UnixServerSocket(std::string_view address);
    Fd accept();

    UnixServerSocket(const UnixServerSocket& ) = delete;
    UnixServerSocket(UnixServerSocket&& other) noexcept = default;
    UnixServerSocket& operator=(const UnixServerSocket& other) = delete;
    UnixServerSocket& operator=(UnixServerSocket&& other) noexcept = default;
    ~UnixServerSocket();

  private:
    std::string address_;
    std::unique_ptr<Fd> unix_socket_;
};

class UnixClientSocket {

};

#endif //SIMPLE_CPP_IN_DOG_UNIX_SOCKET_HPP