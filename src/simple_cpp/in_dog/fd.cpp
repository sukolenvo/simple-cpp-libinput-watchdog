#include "fd.hpp"

#include <stdexcept>
#include <unistd.h>

bool Fd::isValid() const {
  return fd_ != -1;
}

int Fd::get() const {
  return fd_;
}

size_t Fd::read(std::vector<char>& buff) {
  size_t ready = 0;
  while (ready != buff.size()) {
    auto result = ::read(fd_, buff.data() + ready, buff.size() - ready);
    if (result == -1) {
      throw std::runtime_error("Failed to read from unix socket: " + std::to_string(errno));
    }
    if (result == 0) {
      break;
    }
    ready += result;
  }
  return ready;
}

bool Fd::writeBuf(std::span<const char> buffer) const {
  size_t pos = 0;
  while (pos < buffer.size()) {
    const auto done = write(fd_, buffer.data() + pos, buffer.size() - pos);
    if (done == -1) {
      return false;
    }
    pos += done;
  }
  return true;
}

Fd::Fd(Fd&& other) : fd_(other.fd_) {
  other.fd_ = -1;
}

Fd& Fd::operator=(Fd&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  fd_ = other.fd_;
  other.fd_ = -1;
  return *this;
}

Fd::~Fd() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}
