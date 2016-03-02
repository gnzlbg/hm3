#pragma once
/// \file
///
/// System detection macros/types/tags

/// Is the OS unix-like?
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) \
                         || (defined(__APPLE__) && defined(__MACH__)))
#include <unistd.h>  // This defines the POSIX macros
#endif

namespace hm3 {

/// Operating system utilities
namespace system {

struct posix {};
struct apple : posix {};
struct linux : posix {};

#if defined(__APPLE__) && defined(__MACH__)
#define HM3_SYS_APPLE
#define HM3_SYS_POSIX
using tag_t = apple;
#elif defined(_POSIX_VERSION)
#define HM3_SYS_POSIX
using tag_t = posix;
#else
#error Unknown operating system
#endif

static constexpr tag_t tag{};

}  // namespace system
}  // namespace hm3
