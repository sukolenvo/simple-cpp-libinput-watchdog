#include <sys/eventfd.h>

#include "event_fd.hpp"

#include <stdexcept>

EventFd EventFd::open() {
  Fd fd = eventfd(0, 0);
  if (!fd.isValid()) {
    throw std::runtime_error("Failed to create eventfd");
  }
  return EventFd{std::move(fd)};
}

bool EventFd::signal() {
  return fd_.writeVal<uint64_t>(1);
}

bool EventFd::wait() {
  std::vector<char> buf;
  buf.resize(sizeof(uint64_t));
  if (fd_.read(buf) != buf.size()) {
    return false;
  }
  return true;
}

Fd& EventFd::getFd() {
  return fd_;
}

EventFd::EventFd(Fd&& fd) : fd_(std::move(fd)) {
}
