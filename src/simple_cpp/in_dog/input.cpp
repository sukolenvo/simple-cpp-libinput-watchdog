#include "input.hpp"

#include <fcntl.h>

int open_restricted(const char* path, int flags, void* user_data) {
  int fd = open(path, flags);
  return fd > 0 ? fd : -errno;
}

void close_restricted(int fd, void* user_data) {
  close(fd);
}

void delete_udev(udev* udev) {
  udev_unref(udev);
}

void delete_libinput(libinput* libinput) {
  libinput_unref(libinput);
}

Input::Input(): libinput_interface_{open_restricted, close_restricted},
                udev_(nullptr, delete_udev),
                context_(nullptr, delete_libinput) {
}

Fd& Input::start() {
  if (udev_ == nullptr) {
    udev_ = std::unique_ptr<udev, decltype(&delete_udev)>(udev_new(), delete_udev);
    context_ = std::unique_ptr<libinput, decltype(&delete_libinput)>(
      libinput_udev_create_context(&libinput_interface_, nullptr, udev_.get()),
      delete_libinput
    );
    if (libinput_udev_assign_seat(context_.get(), "seat0") == -1) {
      throw std::runtime_error("Failed to assign seat " + std::to_string(errno));
    }
    fd_ = std::make_unique<Fd>(libinput_get_fd(context_.get()));

    dispatch_thread_ = std::make_unique<std::thread>(
      [this]() {
        while (fd_ != nullptr && fd_->poll()) {
          libinput_dispatch(context_.get());
          libinput_event* event;
          while ((event = libinput_get_event(context_.get())) != nullptr) {
            libinput_event_destroy(event);
          }
        }
      }
    );
  }

  return *fd_;
}

Fd& Input::getInputFd() {
  return *fd_;
}

Input::~Input() {
  fd_.reset();
  context_.reset();
  udev_.reset();
  if (dispatch_thread_ != nullptr) {
    // dispatch_thread_->join();
  }
  dispatch_thread_.reset();
}