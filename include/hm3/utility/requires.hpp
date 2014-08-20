#pragma once
/// \file requires.hpp Implements a concept-like requires clause using a macro
/// for constraining templates (via SFINAE) and concept checking.
#include <type_traits>
#include <hm3/utility/range-v3.hpp>

#define HM3_REQUIRES(...) CONCEPT_REQUIRES(__VA_ARGS__)
#define HM3_REQUIRES_(...) CONCEPT_REQUIRES_(__VA_ARGS__)
