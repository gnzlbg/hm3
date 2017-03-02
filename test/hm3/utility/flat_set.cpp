#include <hm3/utility/flat_set.hpp>
#include <hm3/utility/test.hpp>

template struct hm3::flat_set<int>;

using namespace hm3;

template <typename FS>
void check_concepts() {
  static_assert(ranges::Swappable<FS&>{});
  static_assert(ranges::Assignable<FS, FS>{});
  static_assert(ranges::Assignable<FS&, FS&>{});
  static_assert(ranges::Assignable<FS&, FS&&>{});
  static_assert(MoveConstructible<FS>{});
  static_assert(CopyConstructible<FS>{});
  static_assert(Movable<FS>{});
  static_assert(Copyable<FS>{});
  static_assert(DefaultConstructible<FS>{});
  static_assert(ranges::Destructible<FS>{});
  static_assert(EqualityComparable<FS>{});
  static_assert(WeaklyOrdered<FS>{});
  static_assert(TotallyOrdered<FS>{});
  static_assert(SemiRegular<FS>{});
  static_assert(SemiRegular<FS>{});
  static_assert(Regular<FS>{});
  static_assert(RandomAccessRange<FS>{});
  static_assert(ranges::ContiguousRange<FS>{});
  static_assert(not ranges::SinglePass<FS>{});
}

template <typename S, typename T>
void has(S&& s, T&& t, bool result) {
  if (result) {
    CHECK(s.contains(t));
    CHECK(s.count(t) == 1_u);
    CHECK(s.find(t) != ranges::end(s));
  } else {
    CHECK(not s.contains(t));
    CHECK(s.count(t) == 0_u);
    CHECK(s.find(t) == ranges::end(s));
  }
}

template <typename FS>
void test_set() {
  check_concepts<FS>();

  FS s = {3, 5, 2, 4, 7, 1};

  test::check_equal(s, {1, 2, 3, 4, 5, 7});

  CHECK(s.size() == 6_u);
  CHECK(not s.empty());

  has(s, 4, true);
  has(s, 6, false);

  CHECK((s.find(4) - ranges::begin(s)) == 3);

  CHECK(second(s.insert(6)));
  CHECK(s.size() == 7_u);
  has(s, 6, true);
  CHECK(s[5] == 6);
  CHECK(not second(s.insert(4)));
  CHECK(s.size() == 7_u);
  CHECK(s[5] == 6);
  CHECK(s[4] == 5);
  CHECK(s[6] == 7);

  has(s, 0, false);
  CHECK(second(s.insert(0)));
  CHECK(s.size() == 8_u);
  has(s, 0, true);
  CHECK(s[6] == 6);

  has(s, -1, false);
  has(s, -2, false);
  has(s, -3, false);
  s.insert({-1, -3, -2});
  CHECK(s.size() == 11_u);
  has(s, -1, true);
  has(s, -2, true);
  has(s, -3, true);
  CHECK(s[0] == -3);
  CHECK(s.front() == -3);
  CHECK(s[1] == -2);
  CHECK(s.at(1) == -2);
  CHECK(s[2] == -1);
  CHECK(s[9] == 6);
  CHECK(s[10] == 7);
  CHECK(s.back() == 7);

  std::vector<int> vec = {1, 2, 8, 9};
  has(s, 8, false);
  has(s, 9, false);
  s.insert(vec);
  CHECK(s.size() == 13_u);
  has(s, 8, true);
  has(s, 9, true);

  has(s, 1, true);
  CHECK(s.erase(1) == 1_u);
  has(s, 1, false);
  CHECK(s.size() == 12_u);

  has(s, -3, true);
  CHECK(s.erase(ranges::begin(s)) == ranges::begin(s));
  has(s, -3, false);
  CHECK(s.size() == 11_u);

  has(s, 8, true);
  has(s, 9, true);
  CHECK(s.erase(ranges::begin(s) + s.size() - 2, ranges::end(s))
        == ranges::end(s));
  has(s, 8, false);
  has(s, 9, false);
  CHECK(s.size() == 9_u);

  auto v = s.release();
  CHECK(s.size() == 0_u);
  FS os;
  CHECK(os == s);
  os = FS(v);
  s  = FS(std::move(v));
  CHECK(s.size() == 9_u);
  CHECK(s == os);
  FS oss(std::move(os));
  s = oss;
  s = std::move(oss);

  FS oss3{};
  ranges::swap(s, oss3);
  std::swap(s, oss3);
}

int main() {
  test_set<flat_set<int>>();
  test_set<small_flat_set<int, 10>>();

  return test::result();
}
