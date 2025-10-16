#ifndef SIMPLE_CPP_IN_DOG_INPUT_HPP
#define SIMPLE_CPP_IN_DOG_INPUT_HPP

#include <libinput.h>
#include <memory>
#include <thread>

#include "event_fd.hpp"
#include "fd.hpp"
#include "input.hpp"

void delete_udev(udev* udev);

void delete_libinput(libinput* libinput);

class Input {
  public:
    Input();

    Fd& start();

    Fd& getInputFd();

    ~Input();
  private:
    EventFd shutdown_;
    libinput_interface libinput_interface_;
    std::unique_ptr<udev, decltype(&delete_udev)> udev_;
    std::unique_ptr<libinput, decltype(&delete_libinput)> context_;
    std::unique_ptr<Fd> fd_;
    std::unique_ptr<std::thread> dispatch_thread_;
};

#endif //SIMPLE_CPP_IN_DOG_INPUT_HPP
