#include "app.hpp"

#include <print>

#include "event_fd.hpp"
#include "input.hpp"
#include "unix_socket.hpp"

void forwardEvents(Input* input, EventFd* event_fd) {
  Fd& input_fd = input->getInputFd();
  while (input_fd.poll()) {
    if (!event_fd->signal()) {
      std::print("Failed to signal");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void App::run(std::string_view socket_path) {
  std::println("Creating unix socket at {}", socket_path);
  event_fd_ = std::make_unique<EventFd>(EventFd::open());
  unix_server_socket_ = std::make_unique<UnixServerSocket>(socket_path);
  input_ = std::make_unique<Input>();
  input_->start();

  forward_events_thread_ = std::make_unique<std::thread>(forwardEvents, input_.get(), event_fd_.get());

  while (true) {
    UnixSocket socket = unix_server_socket_->accept();
    socket.sendFd(event_fd_->getFd());
  }
}

App::~App() {
  input_.reset();
  unix_server_socket_.reset();
  event_fd_.reset();
  if (forward_events_thread_ != nullptr) {
    forward_events_thread_->join();
  }
  forward_events_thread_.reset();
}
