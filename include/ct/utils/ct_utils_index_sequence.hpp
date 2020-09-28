#ifndef CT__UTILS__INDEX_SEQUENCE_HPP
#define CT__UTILS__INDEX_SEQUENCE_HPP

namespace ct {

/**
    Alternate impl of `std::integer_sequency` from C++14

    - `ct::int_seq::index`   - analogue of `std::integer_sequnece<std::size_t ...>`
    - `ct::int_seq::gen_seq` - analogue of `std::make_index_sequence<std::size_t N>`

    TODO:
        - add references
        - add tests
*/

namespace ind_seq {

template <int... Is>
struct index {};

template <int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

template <int... Is>
struct gen_seq<0, Is...> : index<Is...> { };

} // namespace ind_seq

} // namespace ct

#endif // CT__UTILS__INDEX_SEQUENCE_HPP
