#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/list_c.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/equal.hpp>

#include "catch.hpp"


namespace mpl = boost::mpl;


////////////////////////////////////////////////////////////////////////////////
template <typename IteratorSeq>
struct zip_iterator
{
    using category = mpl::forward_iterator_tag;

    using base = IteratorSeq;

    using type = typename mpl::transform<
                                IteratorSeq,
                                mpl::deref<mpl::placeholders::_>
                            >::type;
};


template <typename... Sequence>
struct zip_view
        : mpl::iterator_range<
                zip_iterator<
                        typename mpl::transform<
                                        mpl::vector<Sequence...>,
                                        mpl::begin<mpl::placeholders::_>
                                    >::type
                >,
                zip_iterator<
                        typename mpl::transform<
                                        mpl::vector<Sequence...>,
                                        mpl::end<mpl::placeholders::_>
                                    >::type
                >
            >
{ };


namespace boost { namespace mpl {
        template <typename IteratorSeq>
        struct next<zip_iterator<IteratorSeq>>
        {
            using type = zip_iterator<
                                typename mpl::transform<
                                                IteratorSeq,
                                                next<mpl::placeholders::_>
                                            >::type
                            >;
        };

        // NOTE: There is no specialization of mpl::deref for zip_iterator.
        //          It seems that the 'type' member of zip_iterator is used for mpl::deref's return type.
        //          mpl::begin and mpl::end will be provided by the mpl::iterator_range automatically.
}} // namespace boost::mpl


TEST_CASE("7-0", "[tmp]")
{
    using tc0 = mpl::vector<char, int, float, double>;
    using tc1 = mpl::list<double, float, int, char>;
    using tc2 = mpl::vector<char *, int *, float *, double *>;

    using zipped_t = zip_view<tc0, tc1, tc2>;

    static_assert(
            mpl::equal<
                    mpl::vector<char, double, char *>,
                    mpl::deref<
                            mpl::begin<zipped_t>::type
                    >::type
            >(),
            ""
    );

    static_assert(
            mpl::equal<
                    mpl::vector<float, int, float *>,
                    mpl::deref<
                            mpl::advance_c<mpl::begin<zipped_t>::type, 2>::type
                    >::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename RotationCenterPositionIterator>
struct rotate_view
        : mpl::joint_view<
                mpl::iterator_range<
                        RotationCenterPositionIterator,
                        typename mpl::end<Sequence>::type
                >,
                mpl::iterator_range<
                        typename mpl::begin<Sequence>::type,
                        RotationCenterPositionIterator
                >
            >
{ };


TEST_CASE("7-3", "[tmp]")
{
    using v = mpl::vector_c<int, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4>;
    using view = rotate_view<
                        v,
                        mpl::advance_c<mpl::begin<v>::type, 5>::type
                    >;

    static_assert(
            mpl::equal<
                    view,
                    mpl::range_c<int, 0, 10>
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename IndexSequenceIterator, typename RandomAccessIteratorBegin>
struct permutation_iterator
{
    using category = mpl::forward_iterator_tag;

    using type = typename mpl::deref<
                                  typename mpl::advance<
                                                   RandomAccessIteratorBegin,
                                                   typename mpl::deref<IndexSequenceIterator>::type
                                               >::type
                            >::type;
};


namespace boost { namespace mpl {
        template <typename IndexSequenceIterator, typename RandomAccessIteratorBegin>
        struct next<permutation_iterator<IndexSequenceIterator, RandomAccessIteratorBegin>>
        {
            using type = permutation_iterator<
                                typename mpl::next<IndexSequenceIterator>::type,
                                RandomAccessIteratorBegin
                            >;
        };
}} // namespace boost::mpl


template <typename IndexSequence, typename RandomAccessSequence>
struct permutation_view
        : mpl::iterator_range<
                permutation_iterator<
                        typename mpl::begin<IndexSequence>::type,
                        typename mpl::begin<RandomAccessSequence>::type
                >,
                permutation_iterator<
                        typename mpl::end<IndexSequence>::type,
                        typename mpl::begin<RandomAccessSequence>::type
                >
            >
{ };


TEST_CASE("7-6", "[tmp]")
{
    static_assert(
            mpl::equal<
                    mpl::vector_c<int, 33, 22, 44, 11, 33>,
                    permutation_view<
                            mpl::list_c<int, 2, 1, 3, 0, 2>,
                            mpl::vector_c<int, 11, 22, 33, 44>
                    >
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename Iterator>
struct reverse_iterator
{
    using category = typename Iterator::category;

    using type = typename mpl::deref<
                                typename mpl::prior<Iterator>::type
                            >::type;
};


namespace boost { namespace mpl {
        template <typename Sequence, typename Iterator>
        struct next<reverse_iterator<Sequence, Iterator>>
        {
            using type = reverse_iterator<
                                Sequence,
                                typename mpl::eval_if<
                                                 std::is_same<
                                                         typename mpl::begin<Sequence>::type,
                                                         typename mpl::prior<Iterator>::type
                                                 >,
                                                 mpl::identity<void>,
                                                 mpl::prior<Iterator>
                                             >::type
                            >;
        };
}} // namespace boost::mpl


template <typename Sequence>
struct reverse_view
        : mpl::iterator_range<
                reverse_iterator<
                        Sequence,
                        typename mpl::end<Sequence>::type
                >,
                reverse_iterator<
                        Sequence,
                        void
                >
            >
{ };


TEST_CASE("7-7", "[tmp]")
{
    static_assert(
            mpl::equal<
                    mpl::vector_c<int, 5, 4, 3, 2, 1>,
                    reverse_view<mpl::vector_c<int, 1, 2, 3, 4, 5>>
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename Sequence1, typename Iterator1, typename Sequence2, typename Iterator2>
struct crossproduct_iterator
{
    using category = mpl::forward_iterator_tag;

    using type = mpl::pair<
                        typename mpl::deref<Iterator1>::type,
                        typename mpl::deref<Iterator2>::type
                    >;
};


template <typename Sequence1, typename Sequence2>
struct crossproduct_view
        : mpl::iterator_range<
                crossproduct_iterator<
                        Sequence1,
                        typename mpl::begin<Sequence1>::type,
                        Sequence2,
                        typename mpl::begin<Sequence2>::type
                >,
                crossproduct_iterator<
                        Sequence1,
                        typename mpl::end<Sequence1>::type,
                        Sequence2,
                        typename mpl::end<Sequence2>::type
                >
            >
{ };


namespace boost { namespace mpl {
        template <typename Sequence1, typename Iterator1, typename Sequence2, typename Iterator2>
        struct next<crossproduct_iterator<Sequence1, Iterator1, Sequence2, Iterator2>>
        {
            using type = typename mpl::if_<
                                        std::is_same<
                                                typename mpl::end<Sequence2>::type,
                                                typename mpl::next<Iterator2>::type
                                        >,
                                        crossproduct_iterator<
                                                Sequence1,
                                                typename mpl::next<Iterator1>::type,
                                                Sequence2,
                                                typename mpl::if_<
                                                                std::is_same<
                                                                        typename mpl::next<Iterator1>::type,
                                                                        typename mpl::end<Sequence1>::type
                                                                >,
                                                                typename mpl::end<Sequence2>::type,
                                                                typename mpl::begin<Sequence2>::type
                                                           >::type
                                        >,
                                        crossproduct_iterator<
                                                Sequence1,
                                                Iterator1,
                                                Sequence2,
                                                typename mpl::next<Iterator2>::type
                                        >
                                    >::type;
        };
}} // namespace boost::mpl


TEST_CASE("7-8", "[tmp]")
{
    static_assert(
            mpl::equal<
                    mpl::vector<
                            mpl::pair<char, mpl::int_<1>>,
                            mpl::pair<char, mpl::int_<2>>,
                            mpl::pair<int, mpl::int_<1>>,
                            mpl::pair<int, mpl::int_<2>>,
                            mpl::pair<double, mpl::int_<1>>,
                            mpl::pair<double, mpl::int_<2>>
                    >,
                    crossproduct_view<
                            mpl::vector<char, int, double>,
                            mpl::vector<mpl::int_<1>, mpl::int_<2>>
                    >
            >(),
            ""
    );
}

