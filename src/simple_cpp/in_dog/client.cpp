
#include <algorithm>
#include <array>
#include <chrono>
#include <print>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

/**
 * Connect to unix socket /run/in-dog.socket
 *
 * @return fd
 */
int openUnixSocket();

/**
 * Read event_fd from unix socket.
 */
int readEventFd(int unix_socket_fd);

int main() {
  int fd = openUnixSocket();
  int event_fd = readEventFd(fd);

  uint64_t buf;
  while (read(event_fd, &buf, sizeof(buf))) {
    std::println("Received event at {}", std::chrono::system_clock::now());
  }
}

int openUnixSocket() {
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) {
    throw std::runtime_error("Failed to create unix socket");
  }
  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;

  std::string path = "/run/in-dog.socket";
  std::ranges::copy(path, addr.sun_path);

  auto ret = ::connect(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
  if (ret == -1) {
    throw std::runtime_error("Failed to connect: error " + std::to_string(errno));
  }
  return fd;
}

int readEventFd(int unix_socket_fd) {
  msghdr msg{};
  char received_data;
  std::array<iovec, 1> payload{&received_data, 1};
  msg.msg_iov = payload.data();
  msg.msg_iovlen = payload.size();
  std::array<char, 100> cbuff{};
  msg.msg_control = cbuff.data();
  msg.msg_controllen = cbuff.size();
  if (recvmsg(unix_socket_fd, &msg, 0) == -1) {
    throw std::runtime_error("Failed to receive fd: " + std::to_string(errno));
  }
  if (msg.msg_controllen == 0) {
    throw std::runtime_error("No msg controls received");
  }
  for (cmsghdr* cmsg_header = CMSG_FIRSTHDR(&msg); cmsg_header != nullptr; cmsg_header = CMSG_NXTHDR(&msg, cmsg_header)) {
    if (cmsg_header->cmsg_type == SCM_RIGHTS) {
      if (cmsg_header->cmsg_len != CMSG_LEN(sizeof(int))) {
        throw std::runtime_error("Unexpected payload size: " + std::to_string(cmsg_header->cmsg_len));
      }
      int* data = reinterpret_cast<int*>(CMSG_DATA(cmsg_header));
      return *data;
    }
  }
  throw std::runtime_error("Failed to receive valid control message");
}