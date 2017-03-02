#include <hm3/utility/small_vector.hpp>
#include <hm3/utility/test.hpp>

template struct hm3::small_vector<int, 0>;
template struct hm3::small_vector<int, 10>;
template struct hm3::small_vector<std::vector<int>, 10>;

// for these we would need to make std::vector "behave":
// template struct hm3::small_vector<const int, 0>;
// template struct hm3::small_vector<const int, 10>;
// template struct hm3::small_vector<const std::vector<int>, 10>;

using namespace hm3;

struct tint {
  int i;
  tint()                      = default;
  constexpr tint(tint const&) = default;
  constexpr tint(tint&&)      = default;
  constexpr tint& operator=(tint const&) = default;
  constexpr tint& operator=(tint&&) = default;

  constexpr tint(int j) : i(j) {}
  operator int() { return i; }
};

static_assert(std::is_trivial<tint>{} and std::is_copy_constructible<tint>{}
               and std::is_move_constructible<tint>{},
              "");

struct moint final {
  int i;
  moint()             = default;
  moint(moint const&) = delete;
  moint& operator=(moint const&) = delete;
  moint(moint&&)                 = default;
  moint& operator=(moint&&) = default;
  operator int() { return i; }
  constexpr moint(int j) : i(j) {}
  // it seems that deleting the copy constructor is not enough to make this
  // non-trivial using libstdc++:
  virtual void foo() {}
};

static_assert(!std::is_trivial<moint>{} and !std::is_copy_constructible<moint>{}
               and std::is_move_constructible<moint>{},
              "");

template <typename T, std::size_t N>
constexpr bool test_bounds(small_vector<T, N> const& v, std::size_t sz) {
  CHECK(v.size() == sz);
  CHECK(v.max_size() == N);
  CHECK(v.capacity() == N);

  std::decay_t<T> count = std::decay_t<T>();
  for (std::size_t i = 0; i != sz; ++i) { CHECK(v[i] == ++count); }

  return true;
}

class A {
  int i_;
  double d_;

  A(const A&);
  A& operator=(const A&);

 public:
  A(int i, double d) : i_(i), d_(d) {}

  A(A&& a) : i_(a.i_), d_(a.d_) {
    a.i_ = 0;
    a.d_ = 0;
  }

  A& operator=(A&& a) {
    i_   = a.i_;
    d_   = a.d_;
    a.i_ = 0;
    a.d_ = 0;
    return *this;
  }

  int geti() const { return i_; }
  double getd() const { return d_; }
};

template <typename T, int N>
struct vec {
  vec() {}
  vec(std::initializer_list<T>) {}
};

void libcxx_tests() {
  auto test_contiguous = [](auto&& c) {
    for (size_t i = 0; i < c.size(); ++i) {
      if (*(c.begin() + i) != *(std::addressof(*c.begin()) + i)) {
        CHECK(false);
      }
    }
  };

  {  // contiguous
    typedef int T;
    typedef small_vector<T, 3> C;
    auto e = C();
    CHECK(e.empty());
    test_contiguous(e);
    test_contiguous(C(3, 5));
  }
  {  // default construct element
    typedef int T;
    typedef small_vector<T, 3> C;
    C c(1);
    CHECK(c.back() == 0);
    CHECK(c.front() == 0);
    CHECK(c[0] == 0);
  }

  {  // iterator
    typedef int T;
    typedef small_vector<T, 3> C;
    C c;
    C::iterator i = c.begin();
    C::iterator j = c.end();
    CHECK(std::distance(i, j) == 0);
    CHECK(i == j);
  }
  {  // const iterator
    typedef int T;
    typedef small_vector<T, 3> C;
    const C c{};
    C::const_iterator i = c.begin();
    C::const_iterator j = c.end();
    CHECK(std::distance(i, j) == 0);
    CHECK(i == j);
  }
  {  // cbegin/cend
    typedef int T;
    typedef small_vector<T, 3> C;
    C c;
    C::const_iterator i = c.cbegin();
    C::const_iterator j = c.cend();
    CHECK(std::distance(i, j) == 0);
    CHECK(i == j);
    CHECK(i == c.end());
  }
  {  // iterator constructor
    typedef int T;
    typedef small_vector<T, 10> C;
    const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    C c(std::begin(t), std::end(t));
    CHECK(std::equal(std::begin(t), std::end(t), std::begin(c), std::end(c)));
    C::iterator i = c.begin();
    CHECK(*i == 0);
    ++i;
    CHECK(*i == 1);
    *i = 10;
    CHECK(*i == 10);
    CHECK(std::distance(std::begin(c), std::end(c)) == 10);
  }
  {  // N3644 testing
    typedef small_vector<int, 10> C;
    C::iterator ii1{}, ii2{};
    C::iterator ii4 = ii1;
    C::const_iterator cii{};
    CHECK(ii1 == ii2);
    CHECK(ii1 == ii4);

    CHECK(!(ii1 != ii2));

    CHECK((ii1 == cii));
    CHECK((cii == ii1));
    CHECK(!(ii1 != cii));
    CHECK(!(cii != ii1));
    CHECK(!(ii1 < cii));
    CHECK(!(cii < ii1));
    CHECK((ii1 <= cii));
    CHECK((cii <= ii1));
    CHECK(!(ii1 > cii));
    CHECK(!(cii > ii1));
    CHECK((ii1 >= cii));
    CHECK((cii >= ii1));
    CHECK((cii - ii1) == 0);
    CHECK((ii1 - cii) == 0);
  }

  {  // capacity
    small_vector<int, 10> a;
    CHECK(a.capacity() == std::size_t(10));
    CHECK(a.empty());
    for (int i = 0; i != 10; ++i) { a.push_back(0); }
    CHECK(a.capacity() == std::size_t(10));
    CHECK(a.size() == std::size_t(10));
    CHECK(!a.empty());
  }

  {  // resize copyable
    using Copyable = int;
    small_vector<Copyable, 10> a(std::size_t(10), 5);
    CHECK(a.size() == std::size_t(10));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    for (int i = 0; i != 10; ++i) CHECK(a[i] == 5);
    a.resize(5);
    CHECK(a.size() == std::size_t(5));

    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    a.resize(9);
    CHECK(a[4] == 5);
    for (int i = 5; i != 9; ++i) CHECK(a[i] == 0);
    CHECK(a.size() == std::size_t(9));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    a.resize(10, 3);
    CHECK(a[4] == 5);
    CHECK(a[8] == 0);
    CHECK(a[9] == 3);
    CHECK(a.size() == std::size_t(10));
    CHECK(a.capacity() == std::size_t(10));
    a.resize(5, 2);
    for (int i = 0; i != 5; ++i) CHECK(a[i] == 5);
    test_contiguous(a);
  }
  {  // resize move-only
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 10> a(10);
    CHECK(a.size() == std::size_t(10));
    CHECK(a.capacity() == std::size_t(10));
    a.resize(5);
    test_contiguous(a);
    CHECK(a.size() == std::size_t(5));
    CHECK(a.capacity() == std::size_t(10));
    a.resize(9);
    CHECK(a.size() == std::size_t(9));
    CHECK(a.capacity() == std::size_t(10));
  }

  {  // resize value:
    using Copyable = int;
    small_vector<Copyable, 10> a(std::size_t(10));
    CHECK(a.size() == std::size_t(10));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    for (int i = 0; i != 10; ++i) CHECK(a[i] == 0);
    a.resize(5);
    CHECK(a.size() == std::size_t(5));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    for (int i = 0; i != 5; ++i) CHECK(a[i] == 0);
    a.resize(9, 5);
    for (int i = 0; i != 5; ++i) CHECK(a[i] == 0);
    for (int i = 5; i != 9; ++i) CHECK(a[i] == 5);
    CHECK(a.size() == std::size_t(9));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
    a.resize(10, 3);
    for (int i = 0; i != 5; ++i) CHECK(a[i] == 0);
    for (int i = 5; i != 9; ++i) CHECK(a[i] == 5);
    CHECK(a[9] == 3);
    CHECK(a.size() == std::size_t(10));
    CHECK(a.capacity() == std::size_t(10));
    test_contiguous(a);
  }

  {  // assign copy
    small_vector<int, 3> z(3, 5);
    small_vector<int, 3> a = {0, 1, 2};
    CHECK(a.size() == std::size_t{3});
    small_vector<int, 3> b;
    CHECK(b.size() == std::size_t{0});
    b = a;
    CHECK(b.size() == std::size_t{3});
    CHECK(std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b)));
  }

  {  // copy construct
    small_vector<int, 3> a = {0, 1, 2};
    CHECK(a.size() == std::size_t{3});
    small_vector<int, 3> b(a);
    CHECK(b.size() == std::size_t{3});

    CHECK(std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b)));
  }

  {  // assign move
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 3> a(3);
    CHECK(a.size() == std::size_t{3});
    small_vector<MoveOnly, 3> b;
    CHECK(b.size() == std::size_t{0});
    b = std::move(a);
    CHECK(b.size() == std::size_t{3});
    CHECK(a.size() == std::size_t{3});
  }

  {  // move construct
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 3> a(3);
    CHECK(a.size() == std::size_t{3});
    small_vector<MoveOnly, 3> b(std::move(a));
    CHECK(b.size() == std::size_t{3});
    CHECK(a.size() == std::size_t{3});
  }

  {  // old tests
    using vec_t = small_vector<int, 5>;
    vec_t vec1(5);
    vec1[0] = 0;
    vec1[1] = 1;
    vec1[2] = 2;
    vec1[3] = 3;
    vec1[4] = 4;
    {
      vec_t vec2;
      vec2.push_back(5);
      vec2.push_back(6);
      vec2.push_back(7);
      vec2.push_back(8);
      vec2.push_back(9);
      CHECK(vec1[0] == 0);
      CHECK(vec1[4] == 4);
      CHECK(vec2[0] == 5);
      CHECK(vec2[4] == 9);
    }
    {
      auto vec2 = vec1;
      CHECK(vec2[0] == 0);
      CHECK(vec2[4] == 4);
      CHECK(vec1[0] == 0);
      CHECK(vec1[4] == 4);
    }
    {
      int count_ = 0;
      for (auto i : vec1) { CHECK(i == count_++); }
    }

    {
      std::vector<int> vec2(5);
      vec2[0] = 4;
      vec2[1] = 3;
      vec2[2] = 2;
      vec2[3] = 1;
      vec2[4] = 0;
      vec_t vec(vec2.size());
      copy(std::begin(vec2), std::end(vec2), std::begin(vec));
      int count_ = 4;
      for (auto i : vec) { CHECK(i == count_--); }
    }
  }

  {  // back and front:
    using C = small_vector<int, 2>;
    C c(1);
    CHECK(c.back() == 0);
    CHECK(c.front() == 0);
    CHECK(c[0] == 0);
    c.clear();
    int one = 1;
    c.push_back(one);
    CHECK(c.back() == 1);
    CHECK(c.front() == 1);
    CHECK(c[0] == 1);
    CHECK(c.size() == 1);
    c.push_back(2);
    CHECK(c.back() == 2);
    CHECK(c.front() == 1);
    CHECK(c[0] == 1);
    CHECK(c[1] == 2);
    CHECK(c.size() == 2);
    c.pop_back();
    CHECK(c.front() == 1);
    CHECK(c[0] == 1);
    CHECK(c.back() == 1);
    c.pop_back();
    CHECK(c.size() == 0);
  }

  {  // const back:
    using C = small_vector<int, 2>;
    const C c(1);
    CHECK(c.back() == 0);
    CHECK(c.front() == 0);
    CHECK(c[0] == 0);
    CHECK(c.size() == 1);
  }

  {  // swap: same type
    using C = small_vector<int, 5>;
    C c0(3, 5);
    C c1(5, 1);
    C c2(0);
    CHECK(c0.size() == std::size_t(3));
    CHECK(c1.size() == std::size_t(5));
    CHECK(c2.size() == std::size_t(0));
    for (int i = 0; i != 3; ++i) { CHECK(c0[i] == 5); }
    for (int i = 0; i != 5; ++i) { CHECK(c1[i] == 1); }
    c0.swap(c1);
    CHECK(c0.size() == std::size_t(5));
    CHECK(c1.size() == std::size_t(3));
    for (int i = 0; i != 5; ++i) { CHECK(c0[i] == 1); }
    for (int i = 0; i != 3; ++i) { CHECK(c1[i] == 5); }
    c2.swap(c1);
    CHECK(c1.size() == std::size_t(0));
    CHECK(c2.size() == std::size_t(3));
    for (int i = 0; i != 3; ++i) { CHECK(c2[i] == 5); }
  }

  {  // std::swap: same type
    using C = small_vector<int, 5>;
    C c0(3, 5);
    C c1(5, 1);
    C c2(0);
    CHECK(c0.size() == std::size_t(3));
    CHECK(c1.size() == std::size_t(5));
    CHECK(c2.size() == std::size_t(0));
    for (int i = 0; i != 3; ++i) { CHECK(c0[i] == 5); }
    for (int i = 0; i != 5; ++i) { CHECK(c1[i] == 1); }
    std::swap(c0, c1);
    CHECK(c0.size() == std::size_t(5));
    CHECK(c1.size() == std::size_t(3));
    for (int i = 0; i != 5; ++i) { CHECK(c0[i] == 1); }
    for (int i = 0; i != 3; ++i) { CHECK(c1[i] == 5); }
    std::swap(c2, c1);
    CHECK(c1.size() == std::size_t(0));
    CHECK(c2.size() == std::size_t(3));
    for (int i = 0; i != 3; ++i) { CHECK(c2[i] == 5); }
  }

  {  // TODO: throwing swap different types
    small_vector<int, 5> v;
    CHECK(v.data() != nullptr);

    small_vector<int, 0> v0;
    CHECK(v0.data() == nullptr);

    const small_vector<int, 5> cv;
    CHECK(cv.data() != nullptr);

    const small_vector<int, 0> cv0;
    CHECK(cv0.data() == nullptr);
  }

  {// emplace:
   {small_vector<A, 3> c;
  small_vector<A, 3>::iterator i = c.emplace(c.cbegin(), 2, 3.5);
  CHECK(i == c.begin());
  CHECK(c.size() == 1);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  i = c.emplace(c.cend(), 3, 4.5);
  CHECK(i == c.end() - 1);
  CHECK(c.size() == 2);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  CHECK(c.back().geti() == 3);
  CHECK(c.back().getd() == 4.5);
  i = c.emplace(c.cbegin() + 1, 4, 6.5);
  CHECK(i == c.begin() + 1);
  CHECK(c.size() == 3);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  CHECK(c[1].geti() == 4);
  CHECK(c[1].getd() == 6.5);
  CHECK(c.back().geti() == 3);
  CHECK(c.back().getd() == 4.5);
}
{
  small_vector<A, 3> c;
  small_vector<A, 3>::iterator i = c.emplace(c.cbegin(), 2, 3.5);
  CHECK(i == c.begin());
  CHECK(c.size() == 1);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  i = c.emplace(c.cend(), 3, 4.5);
  CHECK(i == c.end() - 1);
  CHECK(c.size() == 2);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  CHECK(c.back().geti() == 3);
  CHECK(c.back().getd() == 4.5);
  i = c.emplace(c.cbegin() + 1, 4, 6.5);
  CHECK(i == c.begin() + 1);
  CHECK(c.size() == 3);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  CHECK(c[1].geti() == 4);
  CHECK(c[1].getd() == 6.5);
  CHECK(c.back().geti() == 3);
  CHECK(c.back().getd() == 4.5);
}
}

{// emplace_back
 {small_vector<A, 2> c;
c.emplace_back(2, 3.5);
CHECK(c.size() == 1);
CHECK(c.front().geti() == 2);
CHECK(c.front().getd() == 3.5);
c.emplace_back(3, 4.5);
CHECK(c.size() == 2);
CHECK(c.front().geti() == 2);
CHECK(c.front().getd() == 3.5);
CHECK(c.back().geti() == 3);
CHECK(c.back().getd() == 4.5);
}
{
  small_vector<A, 2> c;
  c.emplace_back(2, 3.5);
  CHECK(c.size() == 1);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  c.emplace_back(3, 4.5);
  CHECK(c.size() == 2);
  CHECK(c.front().geti() == 2);
  CHECK(c.front().getd() == 3.5);
  CHECK(c.back().geti() == 3);
  CHECK(c.back().getd() == 4.5);
}
}

{ // emplace extra:
 {//
  small_vector<int, 4> v;
v = {1, 2, 3};

v.emplace(v.begin(), v.back());
CHECK(v[0] == 3);
}
{
  small_vector<int, 4> v;
  v = {1, 2, 3};
  v.emplace(v.begin(), v.back());
  CHECK(v[0] == 3);
}
}

{// erase
 {int a1[] = {1, 2, 3};
small_vector<int, 4> l1(a1, a1 + 3);
CHECK(l1.size() == 3);
small_vector<int, 4>::const_iterator i = l1.begin();
++i;
small_vector<int, 4>::iterator j = l1.erase(i);
CHECK(l1.size() == 2);
CHECK(std::distance(l1.begin(), l1.end()) == 2);
CHECK(*j == 3);
CHECK(*l1.begin() == 1);
CHECK(*std::next(l1.begin()) == 3);
j = l1.erase(j);
CHECK(j == l1.end());
CHECK(l1.size() == 1);
CHECK(std::distance(l1.begin(), l1.end()) == 1);
CHECK(*l1.begin() == 1);
j = l1.erase(l1.begin());
CHECK(j == l1.end());
CHECK(l1.size() == 0);
CHECK(std::distance(l1.begin(), l1.end()) == 0);
}
}

{  // erase iter iter
  int a1[]    = {1, 2, 3};
  using vec_t = small_vector<int, 5>;
  {
    vec_t l1(a1, a1 + 3);
    vec_t::iterator i = l1.erase(l1.cbegin(), l1.cbegin());
    CHECK(l1.size() == 3);
    CHECK(std::distance(l1.cbegin(), l1.cend()) == 3);
    CHECK(i == l1.begin());
  }
  {
    vec_t l1(a1, a1 + 3);
    vec_t::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin()));
    CHECK(l1.size() == 2);
    CHECK(std::distance(l1.cbegin(), l1.cend()) == 2);
    CHECK(i == l1.begin());
    CHECK(l1 == vec_t(a1 + 1, a1 + 3));
  }
  {
    vec_t l1(a1, a1 + 3);
    vec_t::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 2));
    CHECK(l1.size() == 1);
    CHECK(std::distance(l1.cbegin(), l1.cend()) == 1);
    CHECK(i == l1.begin());
    CHECK(l1 == vec_t(a1 + 2, a1 + 3));
  }
  {
    vec_t l1(a1, a1 + 3);
    vec_t::iterator i = l1.erase(l1.cbegin(), std::next(l1.cbegin(), 3));
    CHECK(l1.size() == 0);
    CHECK(std::distance(l1.cbegin(), l1.cend()) == 0);
    CHECK(i == l1.begin());
  }
  {
    small_vector<vec_t, 3> outer(2, vec_t(1));
    outer.erase(outer.begin(), outer.begin());
    CHECK(outer.size() == 2);
    CHECK(outer[0].size() == 1);
    CHECK(outer[1].size() == 1);
  }
}

{// insert init list
 {small_vector<int, 15> d(10, 1);
small_vector<int, 15>::iterator i = d.insert(d.cbegin() + 2, {3, 4, 5, 6});
CHECK(d.size() == 14);
CHECK(i == d.begin() + 2);
CHECK(d[0] == 1);
CHECK(d[1] == 1);
CHECK(d[2] == 3);
CHECK(d[3] == 4);
CHECK(d[4] == 5);
CHECK(d[5] == 6);
CHECK(d[6] == 1);
CHECK(d[7] == 1);
CHECK(d[8] == 1);
CHECK(d[9] == 1);
CHECK(d[10] == 1);
CHECK(d[11] == 1);
CHECK(d[12] == 1);
CHECK(d[13] == 1);
}
}

{// insert iter iter
 {small_vector<int, 120> v(100);
int a[]             = {1, 2, 3, 4, 5};
const std::size_t N = sizeof(a) / sizeof(a[0]);
small_vector<int, 120>::iterator i
 = v.insert(v.cbegin() + 10, (a + 0), (a + N));
CHECK(v.size() == 100 + N);
CHECK(i == v.begin() + 10);
std::size_t j;
for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
for (std::size_t k = 0; k < N; ++j, ++k) CHECK(v[j] == a[k]);
for (; j < 105; ++j) CHECK(v[j] == 0);
}
{
  small_vector<int, 120> v(100);
  size_t sz        = v.size();
  int a[]          = {1, 2, 3, 4, 5};
  const unsigned N = sizeof(a) / sizeof(a[0]);
  small_vector<int, 120>::iterator i
   = v.insert(v.cbegin() + 10, (a + 0), (a + N));
  CHECK(v.size() == sz + N);
  CHECK(i == v.begin() + 10);
  std::size_t j;
  for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
  for (std::size_t k = 0; k < N; ++j, ++k) CHECK(v[j] == a[k]);
  for (; j < v.size(); ++j) CHECK(v[j] == 0);
}
}

{// insert iter rvalue
 {small_vector<moint, 103> v(100);
small_vector<moint, 103>::iterator i = v.insert(v.cbegin() + 10, moint(3));
CHECK(v.size() == 101);
CHECK(i == v.begin() + 10);
std::size_t j;
for (j = 0; j < 10; ++j)
  if (v[j] != moint()) { CHECK(false); };
if (v[j] != moint(3)) { CHECK(false); };
for (++j; j < 101; ++j)
  if (v[j] != moint()) { CHECK(false); };
}
}

{// insert iter size
 {small_vector<int, 130> v(100);
small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 5, 1);
CHECK(v.size() == 105);
CHECK(i == v.begin() + 10);
std::size_t j;
for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
for (; j < 15; ++j) CHECK(v[j] == 1);
for (++j; j < 105; ++j) CHECK(v[j] == 0);
}
{
  small_vector<int, 130> v(100);
  size_t sz = v.size();
  small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 5, 1);
  CHECK(v.size() == sz + 5);
  CHECK(i == v.begin() + 10);
  std::size_t j;
  for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
  for (; j < 15; ++j) CHECK(v[j] == 1);
  for (++j; j < v.size(); ++j) CHECK(v[j] == 0);
}
{
  small_vector<int, 130> v(100);
  size_t sz = v.size();
  small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 5, 1);
  CHECK(v.size() == sz + 5);
  CHECK(i == v.begin() + 10);
  std::size_t j;
  for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
  for (; j < 15; ++j) CHECK(v[j] == 1);
  for (++j; j < v.size(); ++j) CHECK(v[j] == 0);
}
}

{// iter value:
 {small_vector<int, 130> v(100);
small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 1);
CHECK(v.size() == 101);
CHECK(i == v.begin() + 10);
std::size_t j;
for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
CHECK(v[j] == 1);
for (++j; j < 101; ++j) CHECK(v[j] == 0);
}
{
  small_vector<int, 130> v(100);
  size_t sz = v.size();
  small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 1);
  CHECK(v.size() == sz + 1);
  CHECK(i == v.begin() + 10);
  std::size_t j;
  for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
  CHECK(v[j] == 1);
  for (++j; j < v.size(); ++j) CHECK(v[j] == 0);
}
{
  small_vector<int, 130> v(100);
  v.pop_back();
  v.pop_back();  // force no reallocation
  size_t sz = v.size();
  small_vector<int, 130>::iterator i = v.insert(v.cbegin() + 10, 1);
  CHECK(v.size() == sz + 1);
  CHECK(i == v.begin() + 10);
  std::size_t j;
  for (j = 0; j < 10; ++j) CHECK(v[j] == 0);
  CHECK(v[j] == 1);
  for (++j; j < v.size(); ++j) CHECK(v[j] == 0);
}
}

{  // push back move only
  {
    small_vector<moint, 6> c;
    c.push_back(moint(0));
    CHECK(c.size() == 1);
    for (std::size_t j = 0; j < c.size(); ++j)
      if (c[j] != moint(j)) { CHECK(false); }
    c.push_back(moint(1));
    CHECK(c.size() == 2);
    for (std::size_t j = 0; j < c.size(); ++j)
      if (c[j] != moint(j)) { CHECK(false); }
    c.push_back(moint(2));
    CHECK(c.size() == 3);
    for (std::size_t j = 0; j < c.size(); ++j)
      if (c[j] != moint(j)) { CHECK(false); }
    c.push_back(moint(3));
    CHECK(c.size() == 4);
    for (std::size_t j = 0; j < c.size(); ++j)
      if (c[j] != moint(j)) { CHECK(false); }
    c.push_back(moint(4));
    CHECK(c.size() == 5);
    for (std::size_t j = 0; j < c.size(); ++j)
      if (c[j] != moint(j)) { CHECK(false); }
  }
}
}

int main() {
  {  // const
    small_vector<int, 3> v = {1, 2, 3};
    CHECK(v[0] == 1);
    CHECK(v[1] == 2);
    CHECK(v[2] == 3);
  }

  auto test_contiguous = [](auto&& c) {
    for (size_t i = 0; i < c.size(); ++i) {
      CHECK(*(c.begin() + i) == *(std::addressof(*c.begin()) + i));
    }
  };

  {  // contiguous
    typedef int T;
    typedef small_vector<T, 3> C;
    test_contiguous(C());
    test_contiguous(C(3, 5));
  }

  {  // default construct element
    typedef int T;
    typedef small_vector<T, 3> C;
    C c(1);
    CHECK(back(c) == 0);
    CHECK(front(c) == 0);
    CHECK(c[0] == 0);
  }

  {  // iterator
    typedef int T;
    typedef small_vector<T, 3> C;
    C c;
    C::iterator i = begin(c);
    C::iterator j = end(c);
    CHECK(ranges::distance(i, j) == 0);
    CHECK(i == j);
  }
  {  // const iterator
    typedef int T;
    typedef small_vector<T, 3> C;
    const C c{};
    C::const_iterator i = begin(c);
    C::const_iterator j = end(c);
    CHECK(ranges::distance(i, j) == 0);
    CHECK(i == j);
  }
  {  // cbegin/cend
    typedef int T;
    typedef small_vector<T, 3> C;
    C c;
    C::const_iterator i = cbegin(c);
    C::const_iterator j = cend(c);
    CHECK(ranges::distance(i, j) == 0);
    CHECK(i == j);
    CHECK(i == end(c));
  }
  {  // range constructor
    typedef int T;
    typedef small_vector<T, 10> C;
    const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    C c(t);
    test::check_equal(t, c);
    C::iterator i = begin(c);
    CHECK(*i == 0);
    ++i;
    CHECK(*i == 1);
    *i = 10;
    CHECK(*i == 10);
    CHECK(ranges::distance(c) == 10);
  }
  {  // iterator constructor
    typedef int T;
    typedef small_vector<T, 10> C;
    const T t[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    C c(begin(t), end(t));
    test::check_equal(t, c);
    C::iterator i = begin(c);
    CHECK(*i == 0);
    ++i;
    CHECK(*i == 1);
    *i = 10;
    CHECK(*i == 10);
    CHECK(ranges::distance(c) == 10);
  }
  {  // N3644 testing
    typedef small_vector<int, 10> C;
    C::iterator ii1{}, ii2{};
    C::iterator ii4 = ii1;
    C::const_iterator cii{};
    CHECK(ii1 == ii2);
    CHECK(ii1 == ii4);

    CHECK(!(ii1 != ii2));

    CHECK((ii1 == cii));
    CHECK((cii == ii1));
    CHECK(!(ii1 != cii));
    CHECK(!(cii != ii1));
    CHECK(!(ii1 < cii));
    CHECK(!(cii < ii1));
    CHECK((ii1 <= cii));
    CHECK((cii <= ii1));
    CHECK(!(ii1 > cii));
    CHECK(!(cii > ii1));
    CHECK((ii1 >= cii));
    CHECK((cii >= ii1));
    CHECK((cii - ii1) == 0);
    CHECK((ii1 - cii) == 0);
  }
  {  // types
    auto check_types = [](auto&& c, auto&& t) {
      using C = uncvref_t<decltype(c)>;
      using T = uncvref_t<decltype(t)>;
      static_assert(Container<C>{}, "");
      static_assert(Same<meta::_t<value_type<C>>, T>{}, "");
      // static_assert(Same<meta::_t<size_type<C>>, std::size_t>{}, "");
      static_assert(Same<meta::_t<difference_type<C>>, std::ptrdiff_t>{}, "");
      static_assert(Same<typename C::reference, T&>{}, "");
      static_assert(Same<typename C::const_reference, T const&>{}, "");
      static_assert(Same<typename C::pointer, T*>{}, "");
      static_assert(Same<typename C::const_pointer, T const*>{}, "");
      static_assert(
       Same<
        typename std::iterator_traits<typename C::iterator>::iterator_category,
        std::random_access_iterator_tag>{},
       "");
      static_assert(Same<typename std::iterator_traits<
                          typename C::const_iterator>::iterator_category,
                         std::random_access_iterator_tag>{},
                    "");
    };

    using m = small_vector<int, 10>;
    using c = const m;
    check_types(m{}, int{});
    check_types(c{}, int{});

    using mm = small_vector<std::unique_ptr<int>, 10>;
    using mc = const mm;
    check_types(mm{}, std::unique_ptr<int>{});
    check_types(mc{}, std::unique_ptr<int>{});
  }

  {  // capacity
    small_vector<int, 10> a;
    CHECK(a.capacity() == 10_u);
    for (int i = 0; i != 10; ++i) { a.push_back(0); }
    CHECK(a.capacity() == 10_u);
    CHECK(a.size() == 10_u);
  }

  {
   // erase
  }

  {  // resize copyable
    using Copyable = int;
    small_vector<Copyable, 10> a(10_u, 5);
    CHECK(a.size() == 10_u);
    CHECK(a.capacity() == 10_u);
    test_contiguous(a);
    CHECK(a[0] == 5);
    CHECK(a[9] == 5);
    a.resize(5);
    CHECK(a.size() == 5_u);
    CHECK(a.capacity() == 10_u);
    test_contiguous(a);
    a.resize(9);
    CHECK(a[4] == 5);
    CHECK(a[5] == 0);
    CHECK(a[8] == 0);
    CHECK(a.size() == 9_u);
    CHECK(a.capacity() == 10_u);
    test_contiguous(a);
    a.resize(10, 3);
    CHECK(a[4] == 5);
    CHECK(a[8] == 0);
    CHECK(a[9] == 3);
    CHECK(a.size() == 10_u);
    CHECK(a.capacity() == 10_u);
    test_contiguous(a);
  }

  {  // resize move-only
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 10> a(10);
    CHECK(a.size() == 10_u);
    CHECK(a.capacity() == 10_u);
    a.resize(5);
    CHECK(a.size() == 5_u);
    CHECK(a.capacity() == 10_u);
    a.resize(9);
    CHECK(a.size() == 9_u);
    CHECK(a.capacity() == 10_u);
  }

  {  // assign copy
    small_vector<int, 3> a = {0, 1, 2};
    CHECK(a.size() == 3_u);
    small_vector<int, 3> b;
    CHECK(b.size() == 0_u);
    b = a;
    CHECK(b.size() == 3_u);
    test::check_equal(a, b);
  }

  {  // copy construct
    small_vector<int, 3> a = {0, 1, 2};
    CHECK(a.size() == 3_u);
    small_vector<int, 3> b(a);
    CHECK(b.size() == 3_u);

    test::check_equal(a, b);
  }

  {  // assign move
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 3> a(3);
    CHECK(a.size() == 3_u);
    small_vector<MoveOnly, 3> b;
    CHECK(b.size() == 0_u);
    b = std::move(a);
    CHECK(b.size() == 3_u);
    CHECK(a.size() == 3_u);
  }

  {  // move construct
    using MoveOnly = std::unique_ptr<int>;
    small_vector<MoveOnly, 3> a(3);
    CHECK(a.size() == 3_u);
    small_vector<MoveOnly, 3> b(std::move(a));
    CHECK(b.size() == 3_u);
    CHECK(a.size() == 3_u);
  }

  {  // old tests

    using stack_vec = small_vector<int, 5>;
    stack_vec vec1(5);
    vec1[0] = 0;
    vec1[1] = 1;
    vec1[2] = 2;
    vec1[3] = 3;
    vec1[4] = 4;
    {
      stack_vec vec2;
      vec2.push_back(5);
      vec2.push_back(6);
      vec2.push_back(7);
      vec2.push_back(8);
      vec2.push_back(9);
      CHECK(vec1[0] == 0);
      CHECK(vec1[4] == 4);
      CHECK(vec2[0] == 5);
      CHECK(vec2[4] == 9);
    }
    {
      auto vec2 = vec1;
      CHECK(vec2[0] == 0);
      CHECK(vec2[4] == 4);
      CHECK(vec1[0] == 0);
      CHECK(vec1[4] == 4);
    }
    {
      int count_ = 0;
      for (auto i : vec1) {
        CHECK(i == count_);
        ++count_;
      }
    }

    {
      std::vector<int> vec2(5);
      vec2[0] = 4;
      vec2[1] = 3;
      vec2[2] = 2;
      vec2[3] = 1;
      vec2[4] = 0;
      stack_vec vec(vec2.size());
      ranges::copy(vec2, begin(vec));
      int count_ = 4;
      for (auto i : vec) {
        CHECK(i == count_);
        --count_;
      }
    }
  }
  {
    using stack_vec = small_vector<int, 0>;
    auto a          = stack_vec{};
    CHECK(a.size() == std::size_t{0});
  }

  libcxx_tests();

  return test::result();
}
