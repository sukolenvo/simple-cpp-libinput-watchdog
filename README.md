## About

**in-dog** is util for detecting linux machine inactivity (no keyboard/mouse events).

Device event are detected using libinput. Signals of activity are send using eventfd (at most 1 per sec).
EventFd exchange is done via unix socket `/run/in-dog.socket`.

## Example usage

```c++
int main() {
  int fd = openUnixSocket();
  int event_fd = readEventFd(fd);

  uint64_t buf;
  while (read(event_fd, &buf, sizeof(buf))) {
    std::println("Received event at {}", std::chrono::system_clock::now());
  }
}
// Received event at 2025-10-15 13:44:14.725692097
// Received event at 2025-10-15 13:44:15.351273864
// Received event at 2025-10-15 13:44:18.393661309
// Received event at 2025-10-15 13:44:22.710662992
```

See full example: [client.cpp](./src/simple_cpp/in_dog/client.cpp)

## Build

Github action workflow include latest setup steps.

Requires C++23.

Dependencies:
```bash
sudo apt install libudev-dev libinput-dev
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

## Install

To add it to autostart:

Create `/etc/systemd/system-in-dog.service` (update ExecStart to full path if necessary):
```
[Unit]
Description=in dog service
Documentation=https://github.com/sukolenvo/simple-cpp-libinput-watchdog
Wants=modprobe@uinput.service
After=modprobe@uinput.service

[Install]
WantedBy=multi-user.target

[Service]
Type=simple
ExecStart=simple-cpp-in-dog-main
```

Then execute:
```bash
sudo systemctl start in-dog
sudo systemctl status in-dog
sudo systemctl enable in-dog
```

## References

* eventFd: https://man7.org/linux/man-pages/man2/eventfd.2.html
* libinput docs: https://wayland.freedesktop.org/libinput/doc/latest/
* Unix socket docs (with example at the end): https://man7.org/linux/man-pages/man7/unix.7.html