## About

**in-dog** is util for detecting linux machine inactivity (no keyboard/mouse events).

Device event are detected using libinput. Signals of activity are send using eventfd (at most 1 per sec).
EventFd exchange is done via unix socket `/run/in-dog.socket`.

## Build

Requires C++23.

Dependencies:
```bash
sudo apt intall libudev-dev libinput-dev
```

```bash
cmake -B build
# or specific compiler:
# cmake -DCMAKE_C_COMPILER=clang-19 -DCMAKE_CXX_COMPILER=clang++-19 -B build
cmake --build build --target simple-cpp-in-dog-main

# tests
cmake --build build -- -j 24
ctest --test-dir build
```

## References

* eventFd: https://man7.org/linux/man-pages/man2/eventfd.2.html
* libinput docs: https://wayland.freedesktop.org/libinput/doc/latest/
* Unix socket docs (with example at the end): https://man7.org/linux/man-pages/man7/unix.7.html