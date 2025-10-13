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

    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;
    Fd(Fd&&);
    Fd& operator=(Fd&&) noexcept;
    ~Fd();
  private:
    int fd_;
};

#endif //SIMPLE_CPP_IN_DOG_FD_HPP