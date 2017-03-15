// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused"

#include <complex>
#include <functional>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/test.hpp>
#include <iostream>
#include <vector>

struct caller {
  template <class T>
  caller(T fun) {
    fun();
  }
};
#define CAT2(X, Y) X##Y
#define CAT(X, Y) CAT2(X, Y)
#define TEST(NAME) caller NAME = []

enum state {
  s_default_constructed,
  s_value_copy_constructed,
  s_value_move_constructed,
  s_copy_constructed,
  s_move_constructed,
  s_move_assigned,
  s_copy_assigned,
  s_value_copy_assigned,
  s_value_move_assigned,
  s_moved_from,
  s_value_constructed
};

struct oracle_val {
  state s;
  int i;
  explicit oracle_val(int i = 0) : s(s_value_constructed), i(i) {}
};

struct oracle {
  state s;
  oracle_val val;

  oracle() : s(s_default_constructed) {}
  oracle(const oracle_val& v) : s(s_value_copy_constructed), val(v) {}
  oracle(oracle_val&& v)
   : s(s_value_move_constructed), val(std::move(v)) {  // NOLINT
    v.s = s_moved_from;
  }
  oracle(const oracle& o) : s(s_copy_constructed), val(o.val) {}
  oracle(oracle&& o) : s(s_move_constructed), val(std::move(o.val)) {  // NOLINT
    o.s = s_moved_from;
  }

  oracle& operator=(const oracle_val& v) {
    s   = s_value_copy_constructed;
    val = v;
    return *this;
  }
  oracle& operator=(oracle_val&& v) {
    s   = s_value_move_constructed;
    val = std::move(v);  // NOLINT
    v.s = s_moved_from;
    return *this;
  }
  oracle& operator=(const oracle& o) {
    s   = s_copy_constructed;
    val = o.val;
    return *this;
  }
  oracle& operator=(oracle&& o) {
    s   = s_move_constructed;
    val = std::move(o.val);  // NOLINT
    o.s = s_moved_from;
    return *this;
  }
};

struct guard {
  std::string val;
  guard() : val{} {}
  explicit guard(std::string s, int = 0) : val(std::move(s)) {}
  guard(const guard&) = delete;
  guard(guard&&)      = delete;
  void operator=(const guard&) = delete;
  void operator=(guard&&) = delete;
};

struct explicit_str {
  std::string s;
  explicit explicit_str(const char* chp) : s(chp){};
};

struct date {
  int i;
  date() = delete;
  explicit date(int i) : i{i} {};
  date(date&& d) : i(d.i) { d.i = 0; }
  date(const date&)             = delete;
  date& operator=(const date&) = delete;
  date& operator               =(date&& d) {
    i   = d.i;
    d.i = 0;
    return *this;
  };
};

bool operator==(oracle const& a, oracle const& b) { return a.val.i == b.val.i; }
bool operator!=(oracle const& a, oracle const& b) { return a.val.i != b.val.i; }

namespace tr2 = hm3::optional_detail;

TEST(disengaged_ctor) {
  tr2::optional<int> o1;
  CHECK(!o1);

  tr2::optional<int> o2 = tr2::nullopt;
  CHECK(!o2);

  tr2::optional<int> o3 = o2;
  CHECK(!o3);

  CHECK(o1 == tr2::nullopt);
  CHECK(o1 == tr2::optional<int>{});
  CHECK(!o1);
  CHECK(!bool(o1));

  CHECK(o2 == tr2::nullopt);
  CHECK(o2 == tr2::optional<int>{});
  CHECK(!o2);
  CHECK(!bool(o2));

  CHECK(o3 == tr2::nullopt);
  CHECK(o3 == tr2::optional<int>{});
  CHECK(!o3);
  CHECK(!bool(o3));

  CHECK(o1 == o2);
  CHECK(o2 == o1);
  CHECK(o1 == o3);
  CHECK(o3 == o1);
  CHECK(o2 == o3);
  CHECK(o3 == o2);
};

TEST(value_ctor) {
  oracle_val v;
  tr2::optional<oracle> oo1(v);
  CHECK(oo1 != tr2::nullopt);
  CHECK(oo1 != tr2::optional<oracle>{});
  CHECK(oo1 == tr2::optional<oracle>{v});
  CHECK(!!oo1);
  CHECK(bool(oo1));
  // NA: CHECK (oo1->s == s_value_copy_constructed);
  CHECK(oo1->s == s_move_constructed);
  CHECK(v.s == s_value_constructed);

  tr2::optional<oracle> oo2(std::move(v));  // NOLINT
  CHECK(oo2 != tr2::nullopt);
  CHECK(oo2 != tr2::optional<oracle>{});
  CHECK(oo2 == oo1);
  CHECK(!!oo2);
  CHECK(bool(oo2));
  // NA: CHECK (oo2->s == s_value_move_constructed);
  CHECK(oo2->s == s_move_constructed);
  CHECK(v.s == s_moved_from);

  {
    oracle_val v;
    tr2::optional<oracle> oo1{tr2::in_place, v};
    CHECK(oo1 != tr2::nullopt);
    CHECK(oo1 != tr2::optional<oracle>{});
    CHECK(oo1 == tr2::optional<oracle>{v});
    CHECK(!!oo1);
    CHECK(bool(oo1));
    CHECK(oo1->s == s_value_copy_constructed);
    CHECK(v.s == s_value_constructed);

    tr2::optional<oracle> oo2{tr2::in_place, std::move(v)};  // NOLINT
    CHECK(oo2 != tr2::nullopt);
    CHECK(oo2 != tr2::optional<oracle>{});
    CHECK(oo2 == oo1);
    CHECK(!!oo2);
    CHECK(bool(oo2));
    CHECK(oo2->s == s_value_move_constructed);
    CHECK(v.s == s_moved_from);
  }
};

TEST(assignment) {
  tr2::optional<int> oi;
  oi = tr2::optional<int>{1};
  CHECK(*oi == 1);

  oi = tr2::nullopt;
  CHECK(!oi);

  oi = 2;
  CHECK(*oi == 2);

  oi = {};
  CHECK(!oi);
};

template <class T>
struct move_awaree {
  T val;
  bool moved;
  move_awaree(T val) : val(val), moved(false) {}
  move_awaree(move_awaree const&) = delete;
  move_awaree(move_awaree&& rhs) : val(rhs.val), moved(rhs.moved) {
    rhs.moved = true;
  }
  move_awaree& operator=(move_awaree const&) = delete;
  move_awaree& operator                      =(move_awaree&& rhs) {
    val       = (rhs.val);
    moved     = (rhs.moved);
    rhs.moved = true;
    return *this;
  }
};

TEST(moved_from_state) {
  // first, test mock:
  move_awaree<int> i{1}, j{2};
  CHECK(i.val == 1);
  CHECK(!i.moved);
  CHECK(j.val == 2);
  CHECK(!j.moved);

  move_awaree<int> k = std::move(i);
  CHECK(k.val == 1);
  CHECK(!k.moved);
  CHECK(i.val == 1);
  CHECK(i.moved);

  k = std::move(j);
  CHECK(k.val == 2);
  CHECK(!k.moved);
  CHECK(j.val == 2);
  CHECK(j.moved);

  // now, test optional
  tr2::optional<move_awaree<int>> oi{1}, oj{2};
  assert(oi);
  CHECK(!oi->moved);
  assert(oj);
  CHECK(!oj->moved);

  tr2::optional<move_awaree<int>> ok = std::move(oi);
  assert(ok);
  CHECK(!ok->moved);
  assert(oi);
  CHECK(oi->moved);

  ok = std::move(oj);
  assert(ok);
  CHECK(!ok->moved);
  assert(oj);
  CHECK(oj->moved);
};

TEST(copy_move_ctor_optional_int) {
  tr2::optional<int> oi;
  tr2::optional<int> oj = oi;

  CHECK(!oj);
  CHECK(oj == oi);
  CHECK(oj == tr2::nullopt);
  CHECK(!bool(oj));

  oi                    = 1;
  tr2::optional<int> ok = oi;
  CHECK(!!ok);
  CHECK(bool(ok));
  CHECK(ok == oi);
  CHECK(ok != oj);
  CHECK(*ok == 1);

  tr2::optional<int> ol = std::move(oi);
  CHECK(!!ol);
  CHECK(bool(ol));
  CHECK(ol == oi);
  CHECK(ol != oj);
  CHECK(*ol == 1);
};

TEST(optional_optional) {
  tr2::optional<tr2::optional<int>> oi1 = tr2::nullopt;
  CHECK(oi1 == tr2::nullopt);
  CHECK(!oi1);

  {
    tr2::optional<tr2::optional<int>> oi2{tr2::in_place};
    CHECK(oi2 != tr2::nullopt);
    CHECK(bool(oi2));
    CHECK(*oi2 == tr2::nullopt);
    // CHECK (!(*oi2));
    // std::cout << typeid(**oi2).name() << std::endl;
  }

  {
    tr2::optional<tr2::optional<int>> oi2{tr2::in_place, tr2::nullopt};
    CHECK(oi2 != tr2::nullopt);
    CHECK(bool(oi2));
    CHECK(*oi2 == tr2::nullopt);
    CHECK(!*oi2);
  }

  {
    tr2::optional<tr2::optional<int>> oi2{tr2::optional<int>{}};
    CHECK(oi2 != tr2::nullopt);
    CHECK(bool(oi2));
    CHECK(*oi2 == tr2::nullopt);
    CHECK(!*oi2);
  }

  tr2::optional<int> oi;
  auto ooi = tr2::make_optional(oi);
  static_assert(
   std::is_same<tr2::optional<tr2::optional<int>>, decltype(ooi)>::value, "");
};

TEST(example_guard) {
  using namespace tr2;
  // FAILS: optional<guard> ogx(guard("res1"));
  // FAILS: optional<guard> ogx = "res1";
  // FAILS: optional<guard> ogx("res1");
  optional<guard> oga;  // guard is non-copyable (and non-moveable)
  optional<guard> ogb(in_place,
                      "res1");  // initialzes the contained value with "res1"
  CHECK(bool(ogb));
  CHECK(ogb->val == "res1");

  optional<guard> ogc(in_place);  // default-constructs the contained value
  CHECK(bool(ogc));
  CHECK(ogc->val == "");

  oga.emplace("res1");  // initialzes the contained value with "res1"
  CHECK(bool(oga));
  CHECK(oga->val == "res1");

  oga.emplace();  // destroys the contained value and
                  // default-constructs the new one
  CHECK(bool(oga));
  CHECK(oga->val == "");

  oga = nullopt;  // OK: make disengaged the optional guard
  CHECK(!(oga));
  // FAILS: ogb = {};                          // ERROR: guard is not Moveable
};

void process() {}
void process(int) {}
void process_nil() {}

TEST(example1) {
  using namespace tr2;
  optional<int> oi;            // create disengaged object
  optional<int> oj = nullopt;  // alternative syntax
  oi               = oj;       // assign disengaged object
  optional<int> ok = oj;       // ok is disengaged

  if (oi) { CHECK(false); }              // 'if oi is engaged...'
  if (!oi) { static_assert(true, ""); }  // 'if oi is disengaged...'

  if (oi != nullopt) { CHECK(false); }             // 'if oi is engaged...'
  if (oi == nullopt) { static_assert(true, ""); }  // 'if oi is disengaged...'

  CHECK(oi == ok);  // two disengaged optionals compare equal

  ///////////////////////////////////////////////////////////////////////////
  optional<int> ol{1};  // ol is engaged; its contained value is 1
  ok = 2;               // ok becomes engaged; its contained value is 2
  oj = ol;              // oj becomes engaged; its contained value is 1

  CHECK(oi != ol);  // disengaged != engaged
  CHECK(ok != ol);  // different contained values
  CHECK(oj == ol);  // same contained value
  CHECK(oi < ol);   // disengaged < engaged
  CHECK(ol < ok);   // less by contained value

  /////////////////////////////////////////////////////////////////////////////
  optional<int> om{1};    // om is engaged; its contained value is 1
  optional<int> on = om;  // on is engaged; its contained value is 1
  om               = 2;   // om is engaged; its contained value is 2
  CHECK(on != om);        // on still contains 3. They are not pointers

  /////////////////////////////////////////////////////////////////////////////
  int i = *ol;  // i obtains the value contained in ol
  CHECK(i == 1);
  *ol = 9;  // the object contained in ol becomes 9
  CHECK(*ol == 9);
  CHECK(ol == make_optional(9));

  ///////////////////////////////////
  int p            = 1;
  optional<int> op = p;
  CHECK(*op == 1);
  p = 2;
  CHECK(*op == 1);  // value contained in op is separated from p

  ////////////////////////////////
  if (ol) {
    process(*ol);  // use contained value if present
  } else {
    process();  // proceed without contained value
  }
  if (!om) {
    process_nil();
  } else {
    process(*om);
  }

  /////////////////////////////////////////
  process(ol.value_or(0));  // use 0 if ol is disengaged

  ////////////////////////////////////////////
  ok = nullopt;  // if ok was engaged calls T's dtor
  oj = {};       // assigns a temporary disengaged optional
};

TEST(example_guard2) {
  using hm3::optional;
  const optional<int> c = 4;
  int i                 = *c;  // i becomes 4
  CHECK(i == 4);
  // FAILS: *c = i;                            // ERROR: cannot assign to const
  // int&
};

TEST(example_ref) {
  using namespace hm3;
  int i = 1;
  int j = 2;
  optional<int&> ora;      // disengaged optional reference to int
  optional<int&> orb = i;  // contained reference refers to object i

  *orb = 3;  // i becomes 3
  // FAILS: ora = j;                           // ERROR: optional refs do not
  // have assignment from T
  // FAILS: ora = {j};                         // ERROR: optional refs do not
  // have copy/move assignment
  // FAILS: ora = orb;                         // ERROR: no copy/move assignment
  ora.emplace(j);  // OK: contained reference refers to object j
  ora.emplace(i);  // OK: contained reference now refers to object i

  ora = nullopt;  // OK: ora becomes disengaged
};

template <typename T>
T get_value(tr2::optional<T> new_val     = tr2::nullopt,
            tr2::optional<T&> store_here = tr2::nullopt) {
  T cached{};

  if (new_val) {
    cached = *new_val;

    if (store_here) {
      *store_here = *new_val;  // LEGAL: assigning T to T
    }
  }
  return cached;
}

TEST(example_optional_arg) {
  int iii = 0;
  iii     = get_value<int>(iii, iii);
  iii     = get_value<int>(iii);
  iii     = get_value<int>();

  {
    using namespace hm3;
    optional<guard> grd1{in_place, "res1", 1};  // guard 1 initialized
    optional<guard> grd2;

    grd2.emplace("res2", 2);  // guard 2 initialized
    grd1 = nullopt;           // guard 1 released

  }  // guard 2 released (in dtor)
};

std::tuple<date, date, date> get_start_mid_end() {
  return std::tuple<date, date, date>{date{1}, date{2}, date{3}};
}
void run(date const&, date const&, date const&) {}

TEST(example_date) {
  using namespace hm3;
  optional<date> start, mid,
   end;  // date doesn't have default ctor (no good default date)

  std::tie(start, mid, end) = get_start_mid_end();
  run(*start, *mid, *end);
};

hm3::optional<char> read_next_char() { return {}; }

void run(hm3::optional<std::string>) {}
void run(std::complex<double>) {}

template <class T>
void assign_norebind(tr2::optional<T&>& optref, T& obj) {
  if (optref) {
    *optref = obj;
  } else {
    optref.emplace(obj);
  }
}

template <typename T>
void unused(T&&) {}

TEST(example_conceptual_model) {
  using namespace hm3;

  optional<int> oi = 0;
  optional<int> oj = 1;
  optional<int> ok = nullopt;

  oi = 1;
  oj = nullopt;
  ok = 0;

  unused(oi == nullopt);
  unused(oj == 0);
  unused(ok == 1);
};

TEST(example_rationale) {
  using namespace hm3;
  if (optional<char> ch = read_next_char()) {
    unused(ch);
    // ...
  }

  //////////////////////////////////
  optional<int> opt1 = nullopt;
  optional<int> opt2 = {};

  opt1 = nullopt;
  opt2 = {};

  if (opt1 == nullopt) {}
  if (!opt2) {}
  if (opt2 == optional<int>{}) {}

  ////////////////////////////////

  run(nullopt);  // pick the second overload
  // FAILS: run({});              // ambiguous

  if (opt1 == nullopt) {}  // fine
  // FAILS: if (opt2 == {}) {}   // ilegal

  ////////////////////////////////
  static_assert(optional<unsigned>{} < optional<unsigned>{0}, "");
  static_assert(optional<unsigned>{0} < optional<unsigned>{1}, "");
  static_assert(!(optional<unsigned>{} < optional<unsigned>{}), "");
  static_assert(!(optional<unsigned>{1} < optional<unsigned>{1}), "");

  static_assert(optional<unsigned>{} != optional<unsigned>{0}, "");
  static_assert(optional<unsigned>{0} != optional<unsigned>{1}, "");
  static_assert(optional<unsigned>{} == optional<unsigned>{}, "");
  static_assert(optional<unsigned>{0} == optional<unsigned>{0}, "");

  /////////////////////////////////
  optional<int> o;
  o = make_optional(1);  // copy/move assignment
  o = 1;                 // assignment from T
  o.emplace(1);          // emplacement

  ////////////////////////////////////
  int isas = 0, i = 9;
  optional<int&> asas = i;
  assign_norebind(asas, isas);

  /////////////////////////////////////
  ////tr2::optional<std::vector<int>> ov2 = {2, 3};
  ////CHECK (bool(ov2));
  ////CHECK ((*ov2)[1] == 3);
  ////
  ////////////////////////////////
  ////std::vector<int> v = {1, 2, 4, 8};
  ////optional<std::vector<int>> ov = {1, 2, 4, 8};

  ////CHECK (v == *ov);
  ////
  ////ov = {1, 2, 4, 8};

  ////std::allocator<int> a;
  ////optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

  ////CHECK (ou == ov);

  //////////////////////////////
  // inconvenient syntax:
  {
    tr2::optional<std::vector<int>> ov2{tr2::in_place, {2, 3}};

    CHECK(bool(ov2));
    CHECK((*ov2)[1] == 3);

    ////////////////////////////

    std::vector<int> v = {1, 2, 4, 8};
    optional<std::vector<int>> ov{tr2::in_place, {1, 2, 4, 8}};

    CHECK(v == *ov);

    ov.emplace({1, 2, 4, 8});
    /*
          std::allocator<int> a;
          optional<std::vector<int>> ou { in_place, {1, 2, 4, 8}, a };

          CHECK (ou == ov);
    */
  }

  /////////////////////////////////
  {
    typedef int T;
    optional<optional<T>> ot{in_place};
    optional<optional<T>> ou{in_place, nullopt};
    optional<optional<T>> ov{optional<T>{}};

    optional<int> oi;
    auto ooi = make_optional(oi);
    static_assert(std::is_same<optional<optional<int>>, decltype(ooi)>::value,
                  "");
  }
};

bool fun(std::string, hm3::optional<int> oi = hm3::nullopt) { return bool(oi); }

TEST(example_converting_ctor) {
  using namespace hm3;

  CHECK(fun("dog", 2));
  CHECK(!fun("dog"));
  CHECK(!fun("dog", nullopt));  // just to be explicit
};

TEST(bad_comparison) {
  tr2::optional<int> oi, oj;
  int i;
  bool b = (oi == oj);
  b      = (oi >= i);
  b      = (oi == i);
  unused(b);
};

//// NOT APPLICABLE ANYMORE
////TEST(perfect_ctor)
////{
////  //tr2::optional<std::string> ois = "OS";
////  CHECK (*ois == "OS");
////
////  // FAILS: tr2::optional<explicit_str> oes = "OS";
////  tr2::optional<explicit_str> oes{"OS"};
////  CHECK (oes->s == "OS");
////};

TEST(value_or) {
  tr2::optional<int> oi = 1;
  int i                 = oi.value_or(0);
  CHECK(i == 1);

  oi = tr2::nullopt;
  CHECK(oi.value_or(3) == 3);

  tr2::optional<std::string> os{"AAA"};
  CHECK(os.value_or("BBB") == "AAA");
  os = {};
  CHECK(os.value_or("BBB") == "BBB");
};

TEST(mixed_order) {
  using namespace hm3;

  optional<int> o_n{nullopt};
  optional<int> o0{0};
  optional<int> o1{1};

  CHECK((o_n < 0));
  CHECK((o_n < 1));
  CHECK(!(o0 < 0));
  CHECK((o0 < 1));
  CHECK(!(o1 < 0));
  CHECK(!(o1 < 1));

  CHECK(!(o_n >= 0));
  CHECK(!(o_n >= 1));
  CHECK((o0 >= 0));
  CHECK(!(o0 >= 1));
  CHECK((o1 >= 0));
  CHECK((o1 >= 1));

  CHECK(!(o_n > 0));
  CHECK(!(o_n > 1));
  CHECK(!(o0 > 0));
  CHECK(!(o0 > 1));
  CHECK((o1 > 0));
  CHECK(!(o1 > 1));

  CHECK((o_n <= 0));
  CHECK((o_n <= 1));
  CHECK((o0 <= 0));
  CHECK((o0 <= 1));
  CHECK(!(o1 <= 0));
  CHECK((o1 <= 1));

  CHECK((0 > o_n));
  CHECK((1 > o_n));
  CHECK(!(0 > o0));
  CHECK((1 > o0));
  CHECK(!(0 > o1));
  CHECK(!(1 > o1));

  CHECK(!(0 <= o_n));
  CHECK(!(1 <= o_n));
  CHECK((0 <= o0));
  CHECK(!(1 <= o0));
  CHECK((0 <= o1));
  CHECK((1 <= o1));

  CHECK(!(0 < o_n));
  CHECK(!(1 < o_n));
  CHECK(!(0 < o0));
  CHECK(!(1 < o0));
  CHECK((0 < o1));
  CHECK(!(1 < o1));

  CHECK((0 >= o_n));
  CHECK((1 >= o_n));
  CHECK((0 >= o0));
  CHECK((1 >= o0));
  CHECK(!(0 >= o1));
  CHECK((1 >= o1));
};

struct bad_relops {
  int i;
};

constexpr bool operator<(bad_relops a, bad_relops b) { return a.i < b.i; }
constexpr bool operator>(bad_relops a, bad_relops b) {
  return a.i < b.i;
}  // intentional error!

TEST(bad_relops_) {
  using namespace hm3;
  bad_relops a{1}, b{2};
  CHECK(a < b);
  CHECK(a > b);

  optional<bad_relops> oa = a, ob = b;
  CHECK(oa < ob);
  CHECK(!(oa > ob));

  CHECK(oa < b);
  CHECK(oa > b);

  optional<bad_relops&> ra = a, rb = b;
  CHECK(ra < rb);
  CHECK(!(ra > rb));

  CHECK(ra < b);
  CHECK(ra > b);
};

TEST(mixed_equality) {
  using namespace hm3;

  CHECK(make_optional(0) == 0);
  CHECK(make_optional(1) == 1);
  CHECK(make_optional(0) != 1);
  CHECK(make_optional(1) != 0);

  optional<int> o_n{nullopt};
  optional<int> o0{0};
  optional<int> o1{1};

  CHECK(o0 == 0);
  CHECK(0 == o0);
  CHECK(o1 == 1);
  CHECK(1 == o1);
  CHECK(o1 != 0);
  CHECK(0 != o1);
  CHECK(o0 != 1);
  CHECK(1 != o0);

  CHECK(1 != o_n);
  CHECK(0 != o_n);
  CHECK(o_n != 1);
  CHECK(o_n != 0);
  CHECK(!(1 == o_n));
  CHECK(!(0 == o_n));
  CHECK(!(o_n == 1));
  CHECK(!(o_n == 0));

  std::string cat{"cat"}, dog{"dog"};
  optional<std::string> o_nil{}, o_dog{"dog"}, o_cat{"cat"};

  CHECK(o_cat == cat);
  CHECK(cat == o_cat);
  CHECK(o_dog == dog);
  CHECK(dog == o_dog);
  CHECK(o_dog != cat);
  CHECK(cat != o_dog);
  CHECK(o_cat != dog);
  CHECK(dog != o_cat);

  CHECK(dog != o_nil);
  CHECK(cat != o_nil);
  CHECK(o_nil != dog);
  CHECK(o_nil != cat);
  CHECK(!(dog == o_nil));
  CHECK(!(cat == o_nil));
  CHECK(!(o_nil == dog));
  CHECK(!(o_nil == cat));
};

TEST(const_propagation) {
  using namespace hm3;

  optional<int> mmi{0};
  static_assert(std::is_same<decltype(*mmi), int&>::value, "WTF");

  const optional<int> cmi{0};
  static_assert(std::is_same<decltype(*cmi), const int&>::value, "WTF");

  optional<const int> mci{0};
  static_assert(std::is_same<decltype(*mci), const int&>::value, "WTF");

  optional<const int> cci{0};
  static_assert(std::is_same<decltype(*cci), const int&>::value, "WTF");
};

// static_assert(
//  std::is_base_of<std::logic_error, hm3::bad_optional_access>::value, "");

TEST(safe_value) {
  using namespace hm3;

  try {
    optional<int> ov_n{}, ov1{1};

    int& r1 = ov1.value();
    CHECK(r1 == 1);

    // try {
    //   ov_n.value();
    //   CHECK(false);
    // } catch (bad_optional_access const&) {}

    {  // ref variant
      int i1 = 1;
      optional<int&> or_n{}, or1{i1};

      int& r2 = or1.value();
      CHECK(r2 == 1);

      // try {
      //   or_n.value();
      //   CHECK(false);
      // } catch (bad_optional_access const&) {}
    }
  } catch (...) { CHECK(false); }
};

TEST(optional_ref) {
  using namespace tr2;
  // FAILS: optional<int&&> orr;
  // FAILS: optional<nullopt_t&> on;
  int i = 8;
  optional<int&> ori;
  CHECK(!ori);
  ori.emplace(i);
  CHECK(bool(ori));
  CHECK(*ori == 8);
  CHECK(&*ori == &i);
  *ori = 9;
  CHECK(i == 9);

  // FAILS: int& ir = ori.value_or(i);
  int ii = ori.value_or(i);
  CHECK(ii == 9);
  ii = 7;
  CHECK(*ori == 9);

  int j     = 22;
  auto&& oj = make_optional(std::ref(j));
  *oj       = 23;
  CHECK(&*oj == &j);
  CHECK(j == 23);
};

TEST(optional_ref_const_propagation) {
  using namespace hm3;

  int i                   = 9;
  const optional<int&> mi = i;
  int& r                  = *mi;
  optional<const int&> ci = i;
  static_assert(std::is_same<decltype(*mi), int&>::value, "WTF");
  static_assert(std::is_same<decltype(*ci), const int&>::value, "WTF");

  unused(r);
};

TEST(optional_ref_assign) {
  using namespace hm3;

  int i              = 9;
  optional<int&> ori = i;

  int j = 1;
  ori   = optional<int&>{j};
  ori   = {j};
  // FAILS: ori = j;

  optional<int&> orx = ori;
  ori                = orx;

  optional<int&> orj = j;

  CHECK(ori);
  CHECK(*ori == 1);
  CHECK(ori == orj);
  CHECK(i == 9);

  *ori = 2;
  CHECK(*ori == 2);
  CHECK(ori == 2);
  CHECK(2 == ori);
  CHECK(ori != 3);

  CHECK(ori == orj);
  CHECK(j == 2);
  CHECK(i == 9);

  ori = {};
  CHECK(!ori);
  CHECK(ori != orj);
  CHECK(j == 2);
  CHECK(i == 9);
};

TEST(optional_ref_swap) {
  using namespace hm3;
  int i             = 0;
  int j             = 1;
  optional<int&> oi = i;
  optional<int&> oj = j;

  CHECK(&*oi == &i);
  CHECK(&*oj == &j);

  swap(oi, oj);
  CHECK(&*oi == &j);
  CHECK(&*oj == &i);
};

TEST(optional_initialization) {
  using namespace tr2;
  using std::string;
  string s = "STR";

  optional<string> os{s};
  optional<string> ot = s;
  optional<string> ou{"STR"};
  optional<string> ov = string{"STR"};
};

#include <unordered_set>

TEST(optional_hashing) {
  using namespace tr2;
  using std::string;

  std::hash<int> hi;
  std::hash<optional<int>> hoi;
  std::hash<string> hs;
  std::hash<optional<string>> hos;

  CHECK(hi(0) == hoi(optional<int>{0}));
  CHECK(hi(1) == hoi(optional<int>{1}));
  CHECK(hi(3198) == hoi(optional<int>{3198}));

  CHECK(hs("") == hos(optional<string>{""}));
  CHECK(hs("0") == hos(optional<string>{"0"}));
  CHECK(hs("Qa1#") == hos(optional<string>{"Qa1#"}));

  std::unordered_set<optional<string>> set;
  CHECK(set.find({"Qa1#"}) == set.end());

  set.insert({"0"});
  CHECK(set.find({"Qa1#"}) == set.end());

  set.insert({"Qa1#"});
  CHECK(set.find({"Qa1#"}) != set.end());
};

// optional_ref_emulation
template <class T>
struct generic {
  typedef T type;
};

template <class U>
struct generic<U&> {
  typedef std::reference_wrapper<U> type;
};

template <class T>
using Generic = typename generic<T>::type;

template <class X>
bool generic_fun() {
  hm3::optional<Generic<X>> op;
  return bool(op);
}

TEST(optional_ref_emulation) {
  using namespace hm3;
  optional<Generic<int>> oi = 1;
  CHECK(*oi == 1);

  int i = 8;
  int j = 4;
  optional<Generic<int&>> ori{i};
  CHECK(*ori == 8);
  CHECK((void*)&*ori != (void*)&i);  // !DIFFERENT THAN optional<T&>

  *ori = j;
  CHECK(*ori == 4);
};

TEST(moved_on_value_or) {
  using namespace tr2;
  optional<oracle> oo{in_place};

  CHECK(oo);
  CHECK(oo->s == s_default_constructed);

  oracle o = std::move(oo).value_or(oracle{oracle_val{}});
  CHECK(oo);
  CHECK(oo->s == s_moved_from);
  CHECK(o.s == s_move_constructed);

  optional<move_awaree<int>> om{in_place, 1};
  assert(om);
  CHECK(!om->moved);

  /*move_awaree<int> m =*/std::move(om).value_or(move_awaree<int>{1});
  assert(om);
  CHECK(om->moved);

  {
    date d = optional<date>{in_place, 1}.value();
    CHECK(d.i);  // to silence compiler warning

    date d2 = *optional<date>{in_place, 1};
    CHECK(d2.i);  // to silence compiler warning
  }
};

TEST(optional_ref_hashing) {
  using namespace tr2;
  using std::string;

  std::hash<int> hi;
  std::hash<optional<int&>> hoi;
  std::hash<string> hs;
  std::hash<optional<string&>> hos;

  int i0 = 0;
  int i1 = 1;
  CHECK(hi(0) == hoi(optional<int&>{i0}));
  CHECK(hi(1) == hoi(optional<int&>{i1}));

  string s;
  string s0{"0"};
  string s_cat{"CAT"};
  CHECK(hs("") == hos(optional<string&>{s}));
  CHECK(hs("0") == hos(optional<string&>{s0}));
  CHECK(hs("CAT") == hos(optional<string&>{s_cat}));

  std::unordered_set<optional<string&>> set;
  CHECK(set.find({s_cat}) == set.end());

  set.insert({s0});
  CHECK(set.find({s_cat}) == set.end());

  set.insert({s_cat});
  CHECK(set.find({s_cat}) != set.end());
};

struct combined {
  int m = 0;
  int n = 1;

  constexpr combined() : m{5}, n{6} {}
  constexpr combined(int m, int n) : m{m}, n{n} {}
};

struct nasty {
  int m = 0;
  int n = 1;

  constexpr nasty() : m{5}, n{6} {}
  constexpr nasty(int m, int n) : m{m}, n{n} {}

  int operator&() { return n; }
  int operator&() const { return n; }
};

TEST(arrow_operator) {
  using namespace hm3;

  optional<combined> oc1{in_place, 1, 2};
  CHECK(oc1);
  CHECK(oc1->m == 1);
  CHECK(oc1->n == 2);

  optional<nasty> on{in_place, 1, 2};
  CHECK(on);
  CHECK(on->m == 1);
  CHECK(on->n == 2);
};

TEST(arrow_wit_optional_ref) {
  using namespace hm3;

  combined c{1, 2};
  optional<combined&> oc = c;
  CHECK(oc);
  CHECK(oc->m == 1);
  CHECK(oc->n == 2);

  nasty n{1, 2};
  nasty m{3, 4};
  nasty p{5, 6};

  optional<nasty&> on{n};
  CHECK(on);
  CHECK(on->m == 1);
  CHECK(on->n == 2);

  on = {m};
  CHECK(on);
  CHECK(on->m == 3);
  CHECK(on->n == 4);

  on.emplace(p);
  CHECK(on);
  CHECK(on->m == 5);
  CHECK(on->n == 6);

  optional<nasty&> om{in_place, n};
  CHECK(om);
  CHECK(om->m == 1);
  CHECK(om->n == 2);
};

TEST(no_dangling_reference_in_value) {
  // this mostly tests compiler warnings
  using namespace hm3;
  optional<int> oi{2};
  unused(oi.value());
  const optional<int> coi{3};
  unused(coi.value());
};

struct counted_object {
 private:
  bool throw_;

 public:
  static int counter;

  counted_object(bool b) : throw_(b) { ++counter; }
  counted_object(counted_object const& rhs) : throw_(rhs.throw_) {
    if (throw_) { throw int(); }
  }
  ~counted_object() { --counter; }
};

int counted_object::counter = 0;

TEST(exception_safety) {
  using namespace hm3;
  try {
    optional<counted_object> oo(in_place, true);  // throw
    optional<counted_object> o1(oo);
  } catch (...) {
    //
  }
  CHECK(counted_object::counter == 0);

  try {
    optional<counted_object> oo(in_place, true);  // throw
    optional<counted_object> o1(std::move(oo));   // now move
  } catch (...) {
    //
  }
  CHECK(counted_object::counter == 0);
};

//// constexpr tests

// these 4 classes have different noexcept signatures in move operations
struct nothrow_both {
  nothrow_both(nothrow_both&&) noexcept(true){};
  void operator=(nothrow_both&&) noexcept(true){};  // NOLINT
};
struct nothrow_ctor {
  nothrow_ctor(nothrow_ctor&&) noexcept(true){};
  void operator=(nothrow_ctor&&) noexcept(false){};  // NOLINT
};
struct nothrow_assign {
  nothrow_assign(nothrow_assign&&) noexcept(false){};
  void operator=(nothrow_assign&&) noexcept(true){};  // NOLINT
};
struct nothrow_none {
  nothrow_none(nothrow_none&&) noexcept(false){};
  void operator=(nothrow_none&&) noexcept(false){};  // NOLINT
};

void test_noexcept() {
  {
    tr2::optional<nothrow_both> b1, b2;
    static_assert(
     noexcept(tr2::optional<nothrow_both>{tr2::constexpr_move(b1)}),
     "bad noexcept!");
    static_assert(noexcept(b1 = tr2::constexpr_move(b2)), "bad noexcept!");
  }
  {
    tr2::optional<nothrow_ctor> c1, c2;
    static_assert(
     noexcept(tr2::optional<nothrow_ctor>{tr2::constexpr_move(c1)}),
     "bad noexcept!");
    static_assert(!noexcept(c1 = tr2::constexpr_move(c2)), "bad noexcept!");
  }
  {
    tr2::optional<nothrow_assign> a1, a2;
    static_assert(
     !noexcept(tr2::optional<nothrow_assign>{tr2::constexpr_move(a1)}),
     "bad noexcept!");
    static_assert(!noexcept(a1 = tr2::constexpr_move(a2)), "bad noexcept!");
  }
  {
    tr2::optional<nothrow_none> n1, n2;
    static_assert(
     !noexcept(tr2::optional<nothrow_none>{tr2::constexpr_move(n1)}),
     "bad noexcept!");
    static_assert(!noexcept(n1 = tr2::constexpr_move(n2)), "bad noexcept!");
  }
}

void constexpr_test_disengaged() {
  constexpr tr2::optional<int> g0{};
  constexpr tr2::optional<int> g1{tr2::nullopt};
  static_assert(!g0, "initialized!");
  static_assert(!g1, "initialized!");

  static_assert(bool(g1) == bool(g0), "ne!");

  static_assert(g1 == g0, "ne!");
  static_assert(!(g1 != g0), "ne!");
  static_assert(g1 >= g0, "ne!");
  static_assert(!(g1 > g0), "ne!");
  static_assert(g1 <= g0, "ne!");
  static_assert(!(g1 < g0), "ne!");

  static_assert(g1 == tr2::nullopt, "!");
  static_assert(!(g1 != tr2::nullopt), "!");
  static_assert(g1 <= tr2::nullopt, "!");
  static_assert(!(g1 < tr2::nullopt), "!");
  static_assert(g1 >= tr2::nullopt, "!");
  static_assert(!(g1 > tr2::nullopt), "!");

  static_assert((tr2::nullopt == g0), "!");
  static_assert(!(tr2::nullopt != g0), "!");
  static_assert((tr2::nullopt >= g0), "!");
  static_assert(!(tr2::nullopt > g0), "!");
  static_assert((tr2::nullopt <= g0), "!");
  static_assert(!(tr2::nullopt < g0), "!");

  static_assert((g1 != tr2::optional<int>(1)), "!");
  static_assert(!(g1 == tr2::optional<int>(1)), "!");
  static_assert((g1 < tr2::optional<int>(1)), "!");
  static_assert((g1 <= tr2::optional<int>(1)), "!");
  static_assert(!(g1 > tr2::optional<int>(1)), "!");
  static_assert(!(g1 > tr2::optional<int>(1)), "!");
}

constexpr tr2::optional<int> g0{};
constexpr tr2::optional<int> g2{2};
static_assert(g2, "not initialized!");
static_assert(*g2 == 2, "not 2!");
static_assert(g2 == tr2::optional<int>(2), "not 2!");
static_assert(g2 != g0, "eq!");

static_assert(*tr2::optional<int>{3} == 3, "WTF!");
static_assert(tr2::optional<int>{3}.value() == 3, "WTF!");
static_assert(tr2::optional<int>{3}.value_or(1) == 3, "WTF!");
static_assert(tr2::optional<int>{}.value_or(4) == 4, "WTF!");

constexpr tr2::optional<combined> gc0{tr2::in_place};
static_assert(gc0->n == 6, "WTF!");

// optional refs
int gi                             = 0;
constexpr tr2::optional<int&> gori = gi;
constexpr tr2::optional<int&> gorn{};
constexpr int& gri = *gori;
static_assert(gori, "WTF");
static_assert(!gorn, "WTF");
static_assert(gori != tr2::nullopt, "WTF");
static_assert(gorn == tr2::nullopt, "WTF");
static_assert(&gri == &*gori, "WTF");

constexpr int gci                         = 1;
constexpr tr2::optional<int const&> gorci = gci;
constexpr tr2::optional<int const&> gorcn{};

static_assert(gorcn < gorci, "WTF");
static_assert(gorcn <= gorci, "WTF");
static_assert(gorci == gorci, "WTF");
static_assert(*gorci == 1, "WTF");
static_assert(gorci == gci, "WTF");

namespace constexpr_optional_ref_and_arrow {
using namespace hm3;
constexpr combined c{1, 2};
constexpr optional<combined const&> oc = c;
static_assert(oc, "WTF!");
static_assert(oc->m == 1, "WTF!");
static_assert(oc->n == 2, "WTF!");
}  // namespace constexpr_optional_ref_and_arrow

namespace init_list {
using namespace hm3;

struct const_init_lister {
  template <typename T>
  constexpr const_init_lister(std::initializer_list<T> il) : len(il.size()) {}
  size_t len;
};

constexpr const_init_lister cil{2, 3, 4};
static_assert(cil.len == 3, "WTF!");

constexpr optional<const_init_lister> oil{in_place, {4, 5, 6, 7}};
static_assert(oil, "WTF!");
static_assert(oil->len == 4, "WTF!");
}  // namespace init_list

// end constexpr tests

#include <string>

struct vec {
  std::vector<int> v;
  template <typename... X>
  vec(X&&... x) : v(std::forward<X>(x)...) {}

  template <typename U, typename... X>
  vec(std::initializer_list<U> il, X&&... x) : v(il, std::forward<X>(x)...) {}
};

int main() {
  tr2::optional<int> oi = 1;
  CHECK(bool(oi));
  oi.operator=({});
  CHECK(!oi);

  vec v = {5, 6};

  // if (OPTIONAL_HAS_THIS_RVALUE_REFS) {
  //    std::cout << "Optional has rvalue references for *this" << std::endl;
  // } else {
  //   std::cout << "Optional doesn't have rvalue references for *this"
  //             << std::endl;
  // }
  // if (OPTIONAL_HAS_CONSTEXPR_INIT_LIST) {
  // std::cout << "Optional has constexpr initializer_list" << std::endl;
  // } else {
  //   std::cout << "Optional doesn't have constexpr initializer_list"
  //             << std::endl;
  // }
  // if (OPTIONAL_HAS_MOVE_ACCESSORS) {
  // std::cout << "Optional has constexpr move accessors" << std::endl;
  // } else {
  //   std::cout << "Optional doesn't have constexpr move accessors" <<
  //   std::endl;
  // }
}

#pragma clang diagnostic pop
