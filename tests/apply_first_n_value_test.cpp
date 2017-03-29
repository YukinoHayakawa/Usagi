#include <type_traits>

#include "../apply_first_n_value.hpp"

using namespace yuki;

// test cases combinations:
// exact/more/less args
// N =0, >0

template <int...>
struct apply_test { };

static_assert(std::is_same<apply_first_n_value<3, int, apply_test, 1, 2, 3, 4, 5>::type, apply_test<1, 2, 3>>::value, "test_more_args");
static_assert(std::is_same<apply_first_n_value<0, int, apply_test, 1, 2, 3, 4, 5>::type, apply_test<>>::value, "test_more_args_zero");

static_assert(std::is_same<apply_first_n_value<5, int, apply_test, 1, 2, 3, 4, 5>::type, apply_test<1, 2, 3, 4, 5>>::value, "test_exact_args");
static_assert(std::is_same<apply_first_n_value<0, int, apply_test>::type, apply_test<>>::value, "test_exact_args_zero");

static_assert(std::is_same<apply_first_n_value<5, int, apply_test, 1, 2, 3, 4, 5>::type, apply_test<1, 2, 3, 4, 5>>::value, "test_exact_args");
static_assert(std::is_same<apply_first_n_value<0, int, apply_test>::type, apply_test<>>::value, "test_exact_args_zero");

// todo write a test
// typedef apply_first_n_value<10, int, apply_test, 1, 2, 3, 4, 5>::type test_less_args; -> fail
