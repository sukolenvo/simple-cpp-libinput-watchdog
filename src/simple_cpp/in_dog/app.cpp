#include "app.hpp"

#include <print>

#include "event_fd.hpp"
#include "input.hpp"
#include "unix_socket.hpp"

void forwardEvents(Input* input, EventFd* output, EventFd* shutdown) {
  Fd& input_fd = input->getInputFd();
  while (true) {
    int ready = poll(shutdown->getFd(), input_fd);
    if (ready == -1 || ready == 0) { // error or shutdown
      return;
    }
    if (!output->signal()) {
      std::println(stderr, "Failed to signal");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

App::App() : shutdown_(EventFd::open()) {
}

void App::run(std::string_view socket_path) {
  std::println(stderr, "Creating unix socket at {}", socket_path);
  event_fd_ = std::make_unique<EventFd>(EventFd::open());
  unix_server_socket_ = std::make_unique<UnixServerSocket>(socket_path);
  input_ = std::make_unique<Input>();
  input_->start();

  forward_events_thread_ = std::make_unique<std::thread>(forwardEvents, input_.get(), event_fd_.get(), &shutdown_);

  while (true) {
    std::unique_ptr<UnixSocket> socket = unix_server_socket_->accept();
    if (socket == nullptr) {
      return;
    }
    socket->sendFd(event_fd_->getFd());
  }
}

App::~App() {
  shutdown_.signal();
  if (forward_events_thread_ != nullptr) {
    forward_events_thread_->join();
  }
  forward_events_thread_.reset();
  input_.reset();
  event_fd_.reset();
  unix_server_socket_.reset();
}
