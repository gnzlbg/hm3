#pragma once
/// \file system.hpp

namespace hm3 {

/// Operating system utilities
namespace system {

struct posix {};
struct apple : posix {};
struct linux : posix {};

#ifdef __APPLE__
using tag_t = apple;
#elif __posix
using tag_t = posix;
#else
using tag_t = posix;
#endif

static constexpr tag_t tag{};

}  // namespace system
}  // namespace hm3
