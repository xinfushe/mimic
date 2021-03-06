#pragma once

#include <stdio.h>

#include <memory>
#include <thread>

#include <libusb.h>

#define _log(fmt, prefix, ...) fprintf(stderr, prefix fmt "\n", ##__VA_ARGS__)
#define log(fmt, ...) _log(fmt, "", ##__VA_ARGS__)
#define debug(fmt, ...) _log(fmt, "debug: ", ##__VA_ARGS__)
#define info(fmt, ...) _log(fmt, "info: ", ##__VA_ARGS__)
#define warn(fmt, ...) _log(fmt, "warning: ", ##__VA_ARGS__)
#define error(fmt, ...) _log(fmt, "error: ", ##__VA_ARGS__)
#define fatal(...) \
  do {                  \
    error(__VA_ARGS__); \
    exit(1);            \
  } while (false)

enum class AOAMode {
  accessory = 1 << 0,
  audio = 1 << 1,
};

AOAMode operator|(const AOAMode& lhs, const AOAMode& rhs);
AOAMode operator&(const AOAMode& lhs, const AOAMode& rhs);

class AOADevice {
 private:
  libusb_device_handle* handle = nullptr;
  AOAMode mode = AOAMode(0);
  std::thread accessory_read_thread;
  std::thread accessory_write_thread;
  int accessory_internal_fd = -1;
  int accessory_external_fd = -1;

  std::thread audio_read_thread;
  std::thread audio_write_thread;
  int audio_internal_fd = -1;
  int audio_external_fd = -1;

  AOADevice(libusb_device_handle* handle, AOAMode mode);

 public:
  ~AOADevice();

  bool initialize();
  static std::unique_ptr<AOADevice> open(AOAMode mode);

  int get_accessory_fd() {
    return accessory_external_fd;
  }

  int get_audio_fd() {
    return audio_external_fd;
  }

 private:
  bool spawn_accessory_threads();
  bool spawn_audio_threads();
};
