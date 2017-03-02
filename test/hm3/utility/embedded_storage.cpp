#include <hm3/utility/embedded_storage.hpp>
#include <memory>

template struct hm3::embedded_storage_detail::empty_storage<int>;
template struct hm3::embedded_storage_detail::trivial_storage<int, 10>;
template struct hm3::embedded_storage_detail::non_trivial_storage<
 std::unique_ptr<int>, 10>;

template struct hm3::embedded_storage_detail::empty_storage<const int>;
template struct hm3::embedded_storage_detail::trivial_storage<const int, 10>;
template struct hm3::embedded_storage_detail::non_trivial_storage<
 const std::unique_ptr<int>, 10>;

int main() {
  static_assert(
   std::is_same<hm3::embedded_storage<int, 0>,
                hm3::embedded_storage_detail::empty_storage<int>>{},
   "");
  static_assert(
   std::is_same<hm3::embedded_storage<int, 10>,
                hm3::embedded_storage_detail::trivial_storage<int, 10>>{},
   "");
  static_assert(std::is_same<hm3::embedded_storage<std::unique_ptr<int>, 10>,
                             hm3::embedded_storage_detail::non_trivial_storage<
                              std::unique_ptr<int>, 10>>{},
                "");

  return 0;
}
