#pragma once
/// \file
///
/// VTK data fields
#ifdef HM3_ENABLE_VTK
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/vector.hpp>
#include <hm3/vis/field.hpp>

namespace hm3::vis::vtk {

/// Manages loading/unloading of data fields (cell data, point data, field data)
///
/// Requires 3 functions from the particular data field type:
/// - bool loaded(field): is a data field loaded,
/// - unload(field): unloads a loaded data field,
/// - load(field_name, field_component_value, no_components, component_name):
///   loads a particular data field,
///
/// and a member variable called `log` for logging.
template <typename DataFieldType>
struct data_fields {
  using field  = vis::field;
  using fields = vector<field>;

  /// Data fields in currently loaded grid
  fields current_;

  /// Data fields modified since last update
  fields modified_;

  /// Gets the particular data field type
  DataFieldType* base() noexcept { return static_cast<DataFieldType*>(this); }

  /// Gets the particular data field type
  DataFieldType const* base() const noexcept {
    return static_cast<DataFieldType const*>(this);
  }

  /// Did any cell data field changed since the last update?
  bool changed() const noexcept {
    return ranges::any_of(modified_, [&](auto&& i) {
      return get_field(current_, i.name).value != i.value;
    });
  }

  /// Are all the fields unique?
  bool assert_unique() const noexcept {
    return ranges::all_of(current_, [&](auto&& i) {
      return ranges::count_if(current_,
                              [&](auto&& j) { return i.name == j.name; })
             == 1;
    });
  }

  /// Set status of field \p name to \p value, returns true if any field
  /// changed
  bool operator()(string const& name, const int value) noexcept {
    if (has_field(modified_, name)) {
      get_field(modified_, name).value = static_cast<field::value_t>(value);
    } else {
      modified_.emplace_back(field(name, std::function<void(void)>(), value));
    }
    return changed();
  }

  /// Gets the status of field \p name
  int operator()(string const& name) const noexcept {
    if (has_field(modified_, name)) { return get_field(modified_, name).value; }
    return get_field(current_, name).value;
  }

  /// Gets the name of the field number \p index
  const char* operator()(const int index) const noexcept {
    HM3_ASSERT(index < static_cast<int>(current_.size()),
               "index out-of-bounds");
    return current_[index].name.c_str();
  }

  /// #of fields
  int size() const noexcept { return current_.size(); }

  /// Register a field
  template <typename ComputeComponent>
  auto push(string const& name, ComputeComponent&& g,
            const int default_value = 0) noexcept {
    auto f = field(name,
                   [&, name, g]() {
                     base()->load(name, [&](auto&& i, auto&&) { return g(i); });
                   },
                   default_value);
    auto i = find_field(current_, name);
    HM3_ASSERT(i == ::hm3::end(current_), "field \"{}\" is already registered!",
               name);
    current_.emplace_back(std::move(f));
  }

  /// Register a field
  template <typename ComputeComponent, typename ComponentName>
  auto push(string const& name, ComputeComponent&& g,
            const sint_t no_components, ComponentName&& cn,
            const int default_value = 0) noexcept {
    auto f = field(name,
                   [&, name, g, no_components, cn]() {
                     base()->load(name, g, no_components, cn);
                   },
                   default_value);
    auto i = find_field(current_, name);
    HM3_ASSERT(i == ::hm3::end(current_), "field \"{}\" is already registered!",
               name);
    current_.emplace_back(std::move(f));
  }

  /// Updates all the data fields
  ///
  /// - inactive fields to be loaded are loaded
  /// - active fields to be unloaded are unloaded
  ///
  /// Otherwise, nothing happens.
  void update() {
    base()->log("Updating fields...");

    // Move fields to update into past fields:
    for (auto&& f : modified_) {
      auto& past_f = *find_field(current_, f.name);
      past_f.value = f.value;
    }
    modified_.clear();

    // Update all fields fields:
    //  - inactive and loaded fields get unloaded
    //  - active and unloaded fields get loaded
    for (auto&& f : current_) {
      base()->log("  {} (loaded?: {}, active?: {})", f.name,
                  base()->loaded(f) ? "true" : "false",
                  f.value ? "true" : "false");
      if (f.value == field::inactive && base()->loaded(f)) {
        base()->unload(f);
        HM3_ASSERT(!base()->loaded(f), "not unloaded: {} ??", f.name);
      }

      if (f.value == field::active && !base()->loaded(f)) {
        f.load();
        HM3_ASSERT(base()->loaded(f), "not loaded: {} ??", f.name);
      }
    }
    base()->log("... fields updated!");
  }

  /// Unloads all field
  void unload_all() noexcept {
    base()->log("Unloading all fields...");
    for (auto&& f : current_) {
      if (base()->loaded(f)) {
        base()->log("  {} is loaded and will be unloaded", f.name);
        base()->unload(f);
      } else {
        base()->log("  {} is not loaded", f.name);
      }
    }
    base()->log("...all fields unloaded!");
  }

  auto begin() const noexcept { return ::hm3::begin(current_); }
  auto begin() noexcept { return ::hm3::begin(current_); }
  auto end() const noexcept { return ::hm3::end(current_); }
  auto end() noexcept { return ::hm3::end(current_); }
};

}  // namespace hm3::vis::vtk
#endif  // HM3_ENABLE_VTK
