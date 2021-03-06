/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_EQUAL_TO_05052005_1208)
#define FUSION_EQUAL_TO_05052005_1208

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/fusion/support/is_iterator.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace fusion
{
    namespace extension
    {
        template <typename Tag>
        struct equal_to_impl
        {
            // default implementation
            template <typename I1, typename I2>
            struct apply
            {
                typedef typename
                    is_same<
                        typename add_const<I1>::type
                      , typename add_const<I2>::type
                    >::type
                type;                
                static bool const value = type::value;
            };
        };
    }

    namespace result_of
    {
        template <typename I1, typename I2>
        struct equal_to
            : extension::equal_to_impl<typename I1::ftag>::
                template apply<I1, I2>::type {};
    }

    namespace iterator_operators
    {
        template <typename Iter1, typename Iter2>
        inline typename 
        enable_if<
            mpl::and_<is_fusion_iterator<Iter1>, is_fusion_iterator<Iter2> >
            , bool
            >::type
        operator==(Iter1 const&, Iter2 const&)
        {
            return result_of::equal_to<Iter1, Iter2>::value;
        }

        template <typename Iter1, typename Iter2>
        inline typename 
        enable_if<
            mpl::and_<is_fusion_iterator<Iter1>, is_fusion_iterator<Iter2> >
            , bool
            >::type
        operator!=(Iter1 const&, Iter2 const&)
        {
            return !result_of::equal_to<Iter1, Iter2>::value;
        }
    }

    using iterator_operators::operator==;
    using iterator_operators::operator!=;
}}

#endif

