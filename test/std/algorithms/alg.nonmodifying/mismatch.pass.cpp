// -*- C++ -*-
//===-- mismatch.pass.cpp -------------------------------------------------===//
//
// Copyright (C) 2017-2019 Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// This file incorporates work covered by the following copyright and permission
// notice:
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
//
//===----------------------------------------------------------------------===//

#include "support/pstl_test_config.h"

#include _PSTL_TEST_HEADER(execution)
#include _PSTL_TEST_HEADER(algorithm)

#include "support/utils.h"

using namespace TestUtils;

template <typename Type, std::size_t Partition>
struct test_mismatch
{
    template <typename Policy, typename Iterator1, typename Iterator2>
    void
    operator()(Policy&& exec, Iterator1 first1, Iterator1 last1, Iterator2 first2)
    {
        using namespace std;
        typedef typename iterator_traits<Iterator1>::value_type T;
        {
            const auto expected = std::mismatch(first1, last1, first2, std::equal_to<T>());
            const auto res4 = mismatch(exec, first1, last1, first2);
            EXPECT_TRUE(expected == res4, "wrong return result from mismatch");
        }
    }
    template <typename Policy, typename Iterator1, typename Iterator2>
    void
    operator()(Policy&& exec, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
    {
        using namespace std;
        typedef typename iterator_traits<Iterator1>::value_type T;
        {
            const auto expected = mismatch(pstl::execution::seq, first1, last1, first2, last2, std::equal_to<T>());
            const auto res2 = mismatch(exec, first1, last1, first2, last2);
            EXPECT_TRUE(expected == res2, "wrong return result from mismatch");
        }
    }
};

template <typename Type, std::size_t Partition>
struct test_mismatch_predicate
{
    template <typename Policy, typename Iterator1, typename Iterator2>
    void
    operator()(Policy&& exec, Iterator1 first1, Iterator1 last1, Iterator2 first2)
    {
        using namespace std;
        typedef typename iterator_traits<Iterator1>::value_type T;
        {
            const auto expected = std::mismatch(first1, last1, first2, std::equal_to<T>());
            const auto res3 = mismatch(exec, first1, last1, first2, std::equal_to<T>());
            EXPECT_TRUE(expected == res3, "wrong return result from mismatch with predicate");
        }
    }
    template <typename Policy, typename Iterator1, typename Iterator2>
    void
    operator()(Policy&& exec, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
    {
        using namespace std;
        typedef typename iterator_traits<Iterator1>::value_type T;
        {
            const auto expected = mismatch(pstl::execution::seq, first1, last1, first2, last2, std::equal_to<T>());
            const auto res1 = mismatch(exec, first1, last1, first2, last2, std::equal_to<T>());
            EXPECT_TRUE(expected == res1, "wrong return result from mismatch with predicate");
        }
    }
};

template <typename T>
void
test_mismatch_by_type()
{
    using namespace std;
    for (size_t size = 0; size <= 100000; size = size <= 16 ? size + 1 : size_t(3.1415 * size))
    {
        const T val = T(-1);
        Sequence<T> in(size, [](size_t v) -> T { return T(v % 100); });
        {
            Sequence<T> in2(in);
            invoke_on_all_policies(test_mismatch<T, 0>(), in.begin(), in.end(), in2.begin(), in2.end());
            invoke_on_all_policies(test_mismatch<T, 1>(), in.begin(), in.end(), in2.begin());

            invoke_on_all_policies(test_mismatch_predicate<T, 0>(), in.begin(), in.end(), in2.begin(), in2.end());
            invoke_on_all_policies(test_mismatch_predicate<T, 1>(), in.begin(), in.end(), in2.begin());

            const size_t min_size = 3;
            if (size > min_size)
            {
                const size_t idx_for_1 = size / min_size;
                in[idx_for_1] = val, in[idx_for_1 + 1] = val, in[idx_for_1 + 2] = val;
                invoke_on_all_policies(test_mismatch<T, 2>(), in.begin(), in.end(), in2.begin(), in2.end());
                invoke_on_all_policies(test_mismatch<T, 3>(), in.begin(), in.end(), in2.begin());

                invoke_on_all_policies(test_mismatch_predicate<T, 2>(), in.begin(), in.end(), in2.begin(), in2.end());
                invoke_on_all_policies(test_mismatch_predicate<T, 3>(), in.begin(), in.end(), in2.begin());
            }

            const size_t idx_for_2 = 500;
            if (size >= idx_for_2 - 1)
            {
                in2[size / idx_for_2] = val;
                invoke_on_all_policies(test_mismatch<T, 4>(), in.cbegin(), in.cend(), in2.cbegin(), in2.cend());
                invoke_on_all_policies(test_mismatch<T, 5>(), in.cbegin(), in.cend(), in2.cbegin());

                invoke_on_all_policies(test_mismatch_predicate<T, 4>(), in.cbegin(), in.cend(), in2.cbegin(),
                                       in2.cend());
                invoke_on_all_policies(test_mismatch_predicate<T, 5>(), in.cbegin(), in.cend(), in2.cbegin());
            }
        }
        {
            Sequence<T> in2(100, [](size_t v) -> T { return T(v); });
            invoke_on_all_policies(test_mismatch<T, 6>(), in2.begin(), in2.end(), in.begin(), in.end());
            invoke_on_all_policies(test_mismatch_predicate<T, 6>(), in2.begin(), in2.end(), in.begin(), in.end());
            //  We can't call std::mismatch with semantic below when size of second sequence less than size of first sequence
            if (in2.size() <= in.size())
            {
                invoke_on_all_policies(test_mismatch<T, 7>(), in2.begin(), in2.end(), in.begin());
                invoke_on_all_policies(test_mismatch_predicate<T, 7>(), in2.begin(), in2.end(), in.begin());
            }

            const size_t idx = 97;
            in2[idx] = val;
            in2[idx + 1] = val;
            invoke_on_all_policies(test_mismatch<T, 8>(), in.cbegin(), in.cend(), in2.cbegin(), in2.cend());
            invoke_on_all_policies(test_mismatch_predicate<T, 8>(), in.cbegin(), in.cend(), in2.cbegin(), in2.cend());

            if (in.size() <= in2.size())
            {
                invoke_on_all_policies(test_mismatch<T, 9>(), in.cbegin(), in.cend(), in2.cbegin());
                invoke_on_all_policies(test_mismatch_predicate<T, 9>(), in.cbegin(), in.cend(), in2.cbegin());
            }
        }
        {
            Sequence<T> in2({});
            invoke_on_all_policies(test_mismatch<T, 10>(), in2.begin(), in2.end(), in.begin(), in.end());
            invoke_on_all_policies(test_mismatch<T, 11>(), in.cbegin(), in.cend(), in2.cbegin(), in2.cend());

            invoke_on_all_policies(test_mismatch_predicate<T, 10>(), in2.begin(), in2.end(), in.begin(), in.end());
            invoke_on_all_policies(test_mismatch_predicate<T, 11>(), in.cbegin(), in.cend(), in2.cbegin(), in2.cend());

            if (in.size() == 0)
            {
                invoke_on_all_policies(test_mismatch<T, 12>(), in.cbegin(), in.cend(), in2.cbegin());
                invoke_on_all_policies(test_mismatch_predicate<T, 12>(), in.cbegin(), in.cend(), in2.cbegin());
            }
        }
    }
}

template <typename T>
struct test_non_const
{
    template <typename Policy, typename FirstIterator, typename SecondInterator>
    void
    operator()(Policy&& exec, FirstIterator first_iter, SecondInterator second_iter)
    {
        mismatch(exec, first_iter, first_iter, second_iter, second_iter, non_const(std::less<T>()));
    }
};

int
main()
{

    test_mismatch_by_type<int32_t>();
    test_mismatch_by_type<float64_t>();
    test_mismatch_by_type<Wrapper<int32_t>>();

    test_algo_basic_double<int32_t>(run_for_rnd_fw<test_non_const<int32_t>>());

    std::cout << done() << std::endl;
    return 0;
}