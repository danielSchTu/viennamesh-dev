/*=============================================================================
    Copyright (c) 2001-2004 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef PHOENIX_STATEMENT_IF_HPP
#define PHOENIX_STATEMENT_IF_HPP

#include <boost/spirit/phoenix/core/composite.hpp>
#include <boost/spirit/phoenix/core/as_actor.hpp>
#include <boost/spirit/phoenix/core/compose.hpp>

#if defined(BOOST_MSVC)
# pragma warning(disable:4355)
#endif

namespace boost { namespace phoenix
{
    struct if_else_eval
    {
        template <typename Env, typename Cond, typename Then, typename Else>
        struct apply
        {
            typedef void type;
        };

        template <
            typename RT, typename Env
          , typename Cond, typename Then, typename Else>
        static void
        eval(Env const& env, Cond& cond, Then& then, Else& else_)
        {
            if (cond.eval(env))
                then.eval(env);
            else
                else_.eval(env);
        }
    };

    struct if_eval
    {
        template <typename Env, typename Cond, typename Then>
        struct apply
        {
            typedef void type;
        };

        template <typename RT, typename Env, typename Cond, typename Then>
        static void
        eval(Env const& env, Cond& cond, Then& then)
        {
            if (cond.eval(env))
                then.eval(env);
        }
    };

    template <typename Cond, typename Then>
    struct if_composite;

    template <typename Cond, typename Then>
    struct else_gen
    {
        else_gen(if_composite<Cond, Then> const& source)
            : source(source) {}

        template <typename Else>
        actor<typename as_composite<if_else_eval, Cond, Then, Else>::type>
        operator[](Else const& else_) const
        {
            return compose<if_else_eval>(
                fusion::get<0>(source) // cond
              , fusion::get<1>(source) // then
              , else_ // else
            );
        }

        if_composite<Cond, Then> const& source;
    };

    template <typename Cond, typename Then>
    struct if_composite : composite<if_eval, fusion::tuple<Cond, Then> >
    {
        if_composite(Cond const& cond, Then const& then)
            : composite<if_eval, fusion::tuple<Cond, Then> >(cond, then)
            , else_(*this) {}

        else_gen<Cond, Then> else_;
    };

    template <typename Cond>
    struct if_gen
    {
        if_gen(Cond const& cond)
            : cond(cond) {}

        template <typename Then>
        actor<if_composite<Cond, typename as_actor<Then>::type> >
        operator[](Then const& then) const
        {
            return actor<if_composite<Cond, typename as_actor<Then>::type> >(
                cond, as_actor<Then>::convert(then));
        }

        Cond cond;
    };

    template <typename Cond>
    inline if_gen<typename as_actor<Cond>::type>
    if_(Cond const& cond)
    {
        return if_gen<typename as_actor<Cond>::type>(
            as_actor<Cond>::convert(cond));
    }
}}

#endif
