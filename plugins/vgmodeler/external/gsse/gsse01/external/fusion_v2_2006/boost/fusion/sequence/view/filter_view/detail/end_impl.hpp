/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_END_IMPL_05062005_0906)
#define FUSION_END_IMPL_05062005_0906

namespace boost { namespace fusion
{
    struct filter_view_tag;

    template <typename First, typename Last, typename Pred>
    struct filter_iterator;

    namespace extension
    {
        template <typename Tag>
        struct end_impl;

        template <>
        struct end_impl<filter_view_tag>
        {
            template <typename Sequence>
            struct apply
            {
                typedef typename Sequence::last_type last_type;
                typedef typename Sequence::pred_type pred_type;
                typedef filter_iterator<last_type, last_type, pred_type> type;

                static type
                call(Sequence& s)
                {
                    return type(s.last());
                }
            };
        };
    }
}}

#endif


