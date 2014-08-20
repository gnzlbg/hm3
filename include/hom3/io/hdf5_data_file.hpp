#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Data file in HDF5 format
////////////////////////////////////////////////////////////////////////////////
/// Include:
#include <type_traits>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <hom3/core/filesystem.hpp>
#include <hom3/core/mpi.hpp>
#include <hom3/core/heap_buffer.hpp>
#include <hom3/core/terminate.hpp>
#include <hom3/core/types.hpp>
#include <hom3/io/access_modes.hpp>
#include <hom3/io/error.hpp>
#include <hom3/io/layout.hpp>
#include <hom3/core/ranges.hpp>
////////////////////////////////////////////////////////////////////////////////
/// File local macros (to be undefined at the end of the file):
#define herror(status) handle_error((status), AT_)
#define assert_read_mode()                                          \
  static_assert(std::is_same<AccessMode, access_mode::read>::value, \
                "This function can only be used in read-only mode!")
#define assert_write_mode()                                          \
  static_assert(std::is_same<AccessMode, access_mode::write>::value, \
                "This function can only be used in write mode!")

////////////////////////////////////////////////////////////////////////////////
namespace hom3 {
namespace io {
namespace data {

/// \brief HDF5 file handler
template <class AccessMode> struct hdf5_file {
  hdf5_file(const String fileName,
            const boost::mpi::communicator& comm = boost::mpi::communicator())
   : fileName_(fileName), comm_(comm), fileId_(open(AccessMode())) {}

  ~hdf5_file() { herror(H5Fclose(fileId_)); };
  hdf5_file() = delete;
  hdf5_file(hdf5_file const&) = delete;
  hdf5_file& operator=(hdf5_file const&) = delete;
  hdf5_file(hdf5_file&&) = default;
  hdf5_file& operator=(hdf5_file&&) = default;

  static inline String extension() noexcept { return ".hdf5"; }

  /// \name Read dataset
  ///@{

  /// \brief Reads \p dataset directly to the memory at \p target .
  template <class T>
  inline void array(const String dataset, T* const target) const {
    assert_read_mode();
    read_array_to_memory_(dataset, target);
  }

  /// \brief Reads \p dataset into \p functor .
  template <class Functor>
  inline auto array(const String dataset, Functor&& functor)
   -> decltype(
      std::function<decltype(functor(Ind{}, Ind{}))(Ind, Ind)>(functor),
      void()) {
    assert_read_mode();
    read_array_to_f_(dataset, std::forward<Functor>(functor));
  }

  ///@}

  /// \name Write dataset
  ///@{

  /// \brief Writes \p dataset from \p memory using
  /// the \p order with \p no_rows and \p no_cols.
  template <class T>
  inline void array(const String name, const T* const memory, const Ind no_rows,
                    const Ind no_cols = 1,
                    layout order = layout::col_major) const {
    assert_write_mode();
    write_array_from_memory_(name, memory, no_rows, no_cols, order);
  }

  /// \brief Writes \p dataset from \p functor (\p row_range, \p col_range)
  /// using \p order.
  template <class Functor, class RowRange, class ColRange>
  inline void array(const String dataset, Functor&& functor,
                    RowRange&& row_range,
                    ColRange&& col_range = ranges::make(0, 1),
                    layout order = layout::col_major) {
    assert_write_mode();
    write_array_from_f_(dataset, std::forward<Functor>(functor), row_range,
                        col_range, order);
  }

  ///@}

  /// \brief Read/Writes dataset's \p name attribute \p attrName to \p value
  template <class T>
  inline auto attribute(const String name, const String attrName,
                        T& value) const -> void {
    attribute_impl_(AccessMode(), name, attrName, value);
  }

  template <class T>
  inline auto attribute(const String name, const String attrName,
                        T&& value = T{}) const -> T {
    attribute_impl_(AccessMode(), name, attrName, value);
    return value;
  }

  template <class T>
  inline void attribute(const String attrName, T&& value) const {
    attribute_impl_(AccessMode(), "/", attrName, std::forward<T>(value));
  }

  template <class T> inline T attribute(const String attrName) const {
    T att;
    attribute("/", attrName, att);
    return att;
  }

 private:
  const String fileName_;                ///< File name
  const boost::mpi::communicator comm_;  ///< Communicator of IO processes
  const hid_t fileId_;                   ///< File handler
  memory::buffer::buffer buffer_;        ///< IO buffer

  /// \name Read array implementations
  ///@{

  /// \brief Reads array \p name to \p memory
  template <class T>
  inline void read_array_to_memory_(const String name, T* const memory) const {
    assert_read_mode();
    herror(H5LTread_dataset(fileId_, name.c_str(), hdf5_t(T{}), memory));
  }

  /// \brief Reads array \p name to \p functor
  /// \warning Not Thread-Safe!
  template <class Functor>
  inline void read_array_to_f_(const String name, Functor&& functor) {
    assert_read_mode();
    using T = decltype(functor(Ind{}, Ind{}));
    static_assert(std::is_reference<T>::value,
                  "Cannot read to functor if it doesn't return a reference!");
    using return_type = std::remove_reference_t<T>;
    Ind no_rows, no_cols;
    std::tie(no_rows, no_cols) = dataset_dimensions(name);
    auto const row_range = ranges::make<Ind>(0, no_rows);
    auto const col_range = ranges::make<Ind>(0, no_cols);
    auto b = buffer<return_type>(no_rows * no_cols);
    read_array_to_memory_(name, b.data());
    ordered_execute(row_range, no_rows, col_range, no_cols, dataset_order(name),
                    [&](const Ind offset, const Ind rowIdx, const Ind colIdx) {
      functor(rowIdx, colIdx) = b(offset);
    });
  }

  ///@}

  /// \name Write array implementations
  ///@{

  /// \brief Writes \p memory to \p dataset
  /// using \p order with \p no_rows and \p no_cols.
  template <class T>
  inline void write_array_from_memory_(const String dataset,
                                       const T* const memory, const Ind no_rows,
                                       const Ind no_cols = 1,
                                       layout order = layout::col_major) const {
    assert_write_mode();
    const std::array<hsize_t, 1> dim
     = {{static_cast<hsize_t>(no_rows * no_cols)}};
    herror(H5LTmake_dataset(fileId_, dataset.c_str(), 1, dim.data(),
                            hdf5_t(T{}), memory));
    attribute(dataset, "no_rows", no_rows);
    attribute(dataset, "no_cols", no_cols);
    attribute(dataset, "order", order_to_value(order));
  }

  /// \brief Writes \p dataset from \p functor (\p row_range, \p col_range)
  /// using \p order.
  /// \warning Not Thread-Safe!
  template <class F, class RR, class CR>
  inline void write_array_from_f_(const String dataset, F&& f, RR&& row_range,
                                  CR&& col_range, layout order) {
    assert_write_mode();
    using RT = typename std::remove_reference_t<RR>::value_type;
    using CT = typename std::remove_reference_t<CR>::value_type;
    using T = decltype(f(RT{}, CT{}));
    const Ind no_rows = boost::distance(row_range);
    const Ind no_cols = boost::distance(col_range);
    auto b = buffer<T>(no_rows * no_cols);
    ordered_execute(row_range, no_rows, col_range, no_cols, order,
                    [&](const Ind idx, const Ind rowIdx,
                        const Ind colIdx) { b(idx) = f(rowIdx, colIdx); });
    write_array_from_memory_(dataset, b.data(), no_rows, no_cols, order);
  }

  ///@}

  /// \name Read/Write attribute implementations
  ///@{

  /// \brief Dispatch to get_attribute in read-mode
  template <class T>
  inline void attribute_impl_(access_mode::read, const String name,
                              const String attrName, T&& value) const {
    get_attribute_(name, attrName, std::forward<T>(value));
  }

  /// \brief Dispatch to set_attribute in write-mode
  template <class T>
  inline void attribute_impl_(access_mode::write, const String name,
                              const String attrName, const T value) const {
    set_attribute_(name, attrName, value);
  }

  /// \name Set attribute maps
  ///@{

  inline void set_attribute_(const String name, const String attrName,
                             const long long value) const {
    herror(H5LTset_attribute_long_long(fileId_, name.c_str(), attrName.c_str(),
                                       &value, 1));
  }

  // inline void set_attribute_(const String name, const String attrName,
  //                            const unsigned long long value) const {
  //   herror(H5LTset_attribute_unsigned_long_long(fileId_, name.c_str(), attrName.c_str(),
  //                                      &value, 1));
  // }

  inline void set_attribute_(const String name, const String attrName,
                             const long value) const {
    herror(H5LTset_attribute_long(fileId_, name.c_str(), attrName.c_str(),
                                       &value, 1));
  }

  inline void set_attribute_(const String name, const String attrName,
                             const unsigned long value) const {
    herror(H5LTset_attribute_ulong(fileId_, name.c_str(), attrName.c_str(),
                                       &value, 1));
  }


  inline void set_attribute_(const String name, const String attrName,
                             const int value) const {
    herror(
     H5LTset_attribute_int(fileId_, name.c_str(), attrName.c_str(), &value, 1));
  }

  inline void set_attribute_(const String name, const String attrName,
                             const unsigned int value) const {
    herror(H5LTset_attribute_uint(fileId_, name.c_str(), attrName.c_str(),
                                       &value, 1));
  }
  inline void set_attribute_(const String name, const String attrName,
                             const short value) const {
    herror(
     H5LTset_attribute_short(fileId_, name.c_str(), attrName.c_str(), &value, 1));
  }

  inline void set_attribute_(const String name, const String attrName,
                             const unsigned short value) const {
    herror(H5LTset_attribute_ushort(fileId_, name.c_str(), attrName.c_str(),
                                       &value, 1));
  }


  inline void set_attribute_(const String name, const String attrName,
                             const double value) const {
    herror(H5LTset_attribute_double(fileId_, name.c_str(), attrName.c_str(),
                                    &value, 1));
  }
  inline void set_attribute_(const String name, const String attrName,
                             const float value) const {
    herror(H5LTset_attribute_float(fileId_, name.c_str(), attrName.c_str(),
                                    &value, 1));
  }
  inline void set_attribute_(const String name, const String attrName,
                             const char value) const {
    herror(H5LTset_attribute_char(fileId_, name.c_str(), attrName.c_str(),
                                    &value, 1));
  }
    inline void set_attribute_(const String name, const String attrName,
                             const unsigned char value) const {
    herror(H5LTset_attribute_uchar(fileId_, name.c_str(), attrName.c_str(),
                                    &value, 1));
  }
  ///@}

  /// \name Get attribute maps
  ///@{

  inline void get_attribute_(const String name, const String attrName,
                             long long& value) const {
    herror(H5LTget_attribute_long_long(fileId_, name.c_str(), attrName.c_str(),
                                       &value));
  }
  // inline void get_attribute_(const String name, const String attrName,
  //                            unsigned long long& value) const {
  //   herror(H5LTget_attribute_unsigned_long_long(fileId_, name.c_str(), attrName.c_str(),
  //                                      &value));
  // }
  inline void get_attribute_(const String name, const String attrName,
                             long& value) const {
    herror(H5LTget_attribute_long(fileId_, name.c_str(), attrName.c_str(),
                                       &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             unsigned long& value) const {
    herror(H5LTget_attribute_ulong(fileId_, name.c_str(), attrName.c_str(),
                                       &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             int& value) const {
    herror(
     H5LTget_attribute_int(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             unsigned int& value) const {
    herror(
     H5LTget_attribute_uint(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             short& value) const {
    herror(
     H5LTget_attribute_short(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             unsigned short& value) const {
    herror(
     H5LTget_attribute_ushort(fileId_, name.c_str(), attrName.c_str(), &value));
  }

  inline void get_attribute_(const String name, const String attrName,
                             double& value) const {
    herror(
     H5LTget_attribute_double(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             float& value) const {
    herror(
     H5LTget_attribute_float(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             char& value) const {
    herror(
     H5LTget_attribute_char(fileId_, name.c_str(), attrName.c_str(), &value));
  }
  inline void get_attribute_(const String name, const String attrName,
                             unsigned char& value) const {
    herror(
     H5LTget_attribute_uchar(fileId_, name.c_str(), attrName.c_str(), &value));
  }


  ///@}

  ///@}

  /// \name Dataset information
  ///@{

  /// \brief Returns \p dataset 's (no_rows, no_cols)
  inline auto dataset_dimensions(
   const String dataset) const -> std::tuple<Ind, Ind> {
    Ind no_rows, no_cols;
    attribute(dataset, "no_rows", no_rows);
    attribute(dataset, "no_cols", no_cols);
    return std::make_tuple(no_rows, no_cols);
  }

  /// \brief Returns \p dataset 's \p order
  layout dataset_order(const String name) const {
    Ind order;
    attribute(name, "order", order);
    return array_value_to_order(order);
  }
  ///@}

  /// \name Dataset's order conversion functions
  ///@{

  /// \brief Converts dataset's \p order to unique value
  auto order_to_value(layout order) const noexcept -> SInt {
    switch (order) {
      case layout::row_major: {
        return 0;
      }
      case layout::col_major: {
        return 1;
      }
      default: { TERMINATE("Unknown array order"); }
    }
  }

  /// \brief Converts dataset's \p order unique value to layout
  auto array_value_to_order(Ind order) const noexcept -> layout {
    switch (order) {
      case 0: {
        return layout::row_major;
      }
      case 1: {
        return layout::col_major;
      }
      default: { TERMINATE("Unknown array order"); }
    }
  }

  ///@}

  /// \brief Open files
  ///@{

  /// \brief Opens file in read-only mode.
  hid_t open(access_mode::read) const {
    if (!file::exists(fileName_, comm_)) {
      throw error{"File: " + fileName_ + " doesn't exist!"};
    }
    return H5Fopen(fileName_.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  }
  /// \brief Creates a file and opens it in read-write mode (or if file exists
  /// in append mode).
  hid_t open(access_mode::write) const {
    if (!file::exists(fileName_, comm_)) {
      return H5Fcreate(fileName_.c_str(), H5F_ACC_EXCL, H5P_DEFAULT,
                       H5P_DEFAULT);
    } else { return H5Fopen(fileName_.c_str(), H5F_ACC_RDWR, H5P_DEFAULT); }
    ASSERT(file::exists(fileName_, comm_), "file doesn't exist!");
  }

  ///@}

  /// \brief Handle errors.
  void handle_error(const herr_t status, const String position) const {
    if (status < 0) {
      std::cerr << "There was an error in HDF5 at positon:\n" << position
                << "\n Printing the HDF5 Error stack:\n";
      H5Eprint(H5E_DEFAULT, stderr);
      throw error{"HDF5 error!"};
    }
  }

  /// \brief Executes ternary F \p f in the specified \p order.
  ///
  /// RR \p row_range is a range of row indices
  /// \p no_rows is the #of rows
  /// CR \p col_range is a range of column indices
  /// \p no_cols is the #of columns
  /// \p order is the data layout: row-major or col-major
  /// \p f is a ternary ficate taking a:
  ///   - one-dimensional index (a memoryOffset)
  ///   - and its corresponding row and column indices
  ///
  /// F must not throw!
  ///
  template <class RR, class CR, class F>
  void ordered_execute(RR row_range, const Ind no_rows, CR col_range,
                       const Ind no_cols, const layout order, F&& f) const
   noexcept {
    if (order == layout::row_major) {
      for (auto&& row : row_range) {
        for (auto&& col : col_range) { f(row * no_cols + col, row, col); }
      }
    } else if (order == layout::col_major) {
      for (auto&& col : col_range) {
        for (auto&& row : row_range) { f(col * no_rows + row, row, col); }
      }
    } else { TERMINATE("Unknown order."); }
  }

  /// \brief Returns a buffer able to hold \p noTs \p T.
  template <class T> inline memory::buffer::view<T> buffer(const Ind noTs) {
    return memory::buffer::view<T>{buffer_, noTs};
  }

  /// \name Type-map, hdf5_t: HOM3 Type -> HDF5 Type
  ///
  /// Note: The H5T_... are _objects_ of a magical type having a magical state.
  ///@{
  static inline auto hdf5_t(long long) RETURNS(H5T_NATIVE_LLONG);
  // static inline auto hdf5_t(unsigned long long) RETURNS(H5T_NATIVE_ULLONG);
  static inline auto hdf5_t(long) RETURNS(H5T_NATIVE_LONG);
  static inline auto hdf5_t(unsigned long) RETURNS(H5T_NATIVE_ULONG);
  static inline auto hdf5_t(int) RETURNS(H5T_NATIVE_INT);
  static inline auto hdf5_t(unsigned int) RETURNS(H5T_NATIVE_UINT);
  static inline auto hdf5_t(double) RETURNS(H5T_NATIVE_DOUBLE);
  static inline auto hdf5_t(float) RETURNS(H5T_NATIVE_FLOAT);
  static inline auto hdf5_t(char) RETURNS(H5T_NATIVE_CHAR);
  static inline auto hdf5_t(unsigned char) RETURNS(H5T_NATIVE_UCHAR);
  ///@}
};

}  // namespace data
}  // namespace io
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
#undef herror
#undef assert_read_mode
#undef assert_write_mode
////////////////////////////////////////////////////////////////////////////////
