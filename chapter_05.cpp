#include "catch.hpp"

#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/prior.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/push_back.hpp>


namespace mpl = boost::mpl;


struct none
{ };

struct tiny_tag
{ };


template <typename T0 = none, typename T1 = none, typename T2 = none>
struct tiny
{
    using type = tiny;
    using tag = tiny_tag;

    using t0 = T0;
    using t1 = T1;
    using t2 = T2;
};


template <typename Tiny, int N>
struct tiny_at;

template <typename Tiny>
struct tiny_at<Tiny, 0>
{
    using type = typename Tiny::t0;
};

template <typename Tiny>
struct tiny_at<Tiny, 1>
{
    using type = typename Tiny::t1;
};

template <typename Tiny>
struct tiny_at<Tiny, 2>
{
    using type = typename Tiny::t2;
};


template <typename T0, typename T1, typename T2>
struct tiny_size : mpl::int_<3>
{ };

template <typename T0, typename T1>
struct tiny_size<T0, T1, none> : mpl::int_<2>
{ };

template <typename T0>
struct tiny_size<T0, none, none> : mpl::int_<1>
{ };

template <>
struct tiny_size<none, none, none> : mpl::int_<0>
{ };


template <typename Tiny, typename T, int N>
struct tiny_push_back;

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 0>
        : tiny<T, none, none>
{ };

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 1>
        : tiny<typename Tiny::t0, T, none>
{ };

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 2>
        : tiny<typename Tiny::t0, typename Tiny::t1, T>
{ };


template <typename Tiny, typename Pos>
struct tiny_iterator
{
    using category = mpl::random_access_iterator_tag;
};


namespace boost { namespace mpl {
        template <typename Tiny, typename Pos>
        struct next<tiny_iterator<Tiny, Pos>>
        {
            using type = typename tiny_iterator<
                                            Tiny,
                                            typename mpl::next<Pos>::type
                                        >::type;
        };

        template <typename Tiny, typename Pos>
        struct prior<tiny_iterator<Tiny, Pos>>
        {
            using type = typename tiny_iterator<
                                            Tiny,
                                            typename mpl::prior<Pos>::type
                                        >::type;
        };

        template <>
        struct at_impl<tiny_tag>
        {
            template <typename Tiny, typename N>
            struct apply : tiny_at<Tiny, N::value>
            { };
        };

        template <typename Tiny, typename Pos>
        struct deref<tiny_iterator<Tiny, Pos>>
                : at<Tiny, Pos>
        { };

        template <typename Tiny, typename Pos, typename N>
        struct advance<tiny_iterator<Tiny, Pos>, N>
        {
            using type = tiny_iterator<
                                Tiny,
                                typename mpl::plus<Pos, N>::type
                            >;
        };

        template <typename Tiny, typename Pos1, typename Pos2>
        struct distance<tiny_iterator<Tiny, Pos1>, tiny_iterator<Tiny, Pos2>>
                : mpl::minus<Pos2, Pos1>
        { };

        template <>
        struct begin_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                using type = tiny_iterator<Tiny, int_<0>>;
            };
        };

        template <>
        struct end_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                using type = tiny_iterator<
                                    Tiny,
                                    typename tiny_size<
                                                    typename Tiny::t0,
                                                    typename Tiny::t1,
                                                    typename Tiny::t2
                                                >::type
                                >;
            };
        };

        template <>
        struct size_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
                    : tiny_size<
                            typename Tiny::t0,
                            typename Tiny::t1,
                            typename Tiny::t2
                        >
            { };
        };

        template <>
        struct clear_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply : tiny<>
            { };
        };

        template <>
        struct push_front_impl<tiny_tag>
        {
            template <typename Tiny, typename T>
            struct apply
                    : tiny<T, typename Tiny::t0, typename Tiny::t1>
            { };
        };

        template <>
        struct push_back_impl<tiny_tag>
        {
            template <typename Tiny, typename T>
            struct apply
                    : tiny_push_back<Tiny, T, size<Tiny>::value>
            { };
        };
}} // namespace boost::mpl


TEST_CASE("5-0", "[tmp]")
{

}
