#ifndef SIMPLE_CPP_IN_DOG_APP_HPP
#define SIMPLE_CPP_IN_DOG_APP_HPP

#include <memory>
#include <string_view>

#include "event_fd.hpp"
#include "input.hpp"
#include "unix_socket.hpp"

class App {
  public:
    App();
    void run(std::string_view socket_path);

    ~App();
  private:
    EventFd shutdown_;
    std::unique_ptr<EventFd> event_fd_;
    std::unique_ptr<UnixServerSocket> unix_server_socket_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<std::thread> forward_events_thread_;
};

#endif //SIMPLE_CPP_IN_DOG_APP_HPP
