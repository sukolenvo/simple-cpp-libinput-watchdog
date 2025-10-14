#ifndef SIMPLE_CPP_IN_DOG_EVENT_FD_HPP
#define SIMPLE_CPP_IN_DOG_EVENT_FD_HPP

#include "fd.hpp"

class EventFd {
  public:
    static EventFd open();
    explicit EventFd(Fd&& fd);
    bool signal();
    bool wait();

    Fd& getFd();
  private:
    Fd fd_;
};

#endif //SIMPLE_CPP_IN_DOG_EVENT_FD_HPP
