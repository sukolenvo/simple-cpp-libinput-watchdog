#ifndef SIMPLE_CPP_IN_DOG_FD_HPP
#define SIMPLE_CPP_IN_DOG_FD_HPP

#include <span>
#include <vector>

class Fd {
  public:
    Fd(int fd) : fd_(fd) {
    }

    bool isValid() const;
    int get() const;
    size_t read(std::vector<char>& buff);
    bool writeBuf(std::span<const char> buffer) const;
    template <typename  T>
    bool writeVal(const T& val) const {
      const char* begin = reinterpret_cast<const char*>(&val);
      const char* end = begin + sizeof(val);
      return writeBuf({begin, end});
    }
    bool sendFd(const Fd& fd) const;
    bool poll() const;

    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;
    Fd(Fd&&);
    Fd& operator=(Fd&&) noexcept;
    ~Fd();
  private:
    int fd_;
};

#endif //SIMPLE_CPP_IN_DOG_FD_HPP