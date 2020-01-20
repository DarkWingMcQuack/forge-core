#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace forge::utils {

template<class InputIterator,
         class OutputIterator,
         class UnaryOperation,
         class UnaryPredicate>
OutputIterator transform_if(InputIterator first,
                            InputIterator last,
                            OutputIterator result,
                            UnaryOperation op,
                            UnaryPredicate test)
{
    while(first != last) {
        if(test(*first))
            *result++ = op(*first);
        ++first;
    }
    return result;
}

template<class InputIterator,
         class UnaryOperation>
auto transform_into_vector(InputIterator first,
                           InputIterator last,
                           UnaryOperation op)
{
    using ContainerType = typename InputIterator::value_type;
    using OpReturnType = std::invoke_result_t<UnaryOperation, ContainerType>;
    using VecType = std::vector<OpReturnType>;

    VecType ret_vec;
    std::transform(first,
                   last,
                   std::back_inserter(ret_vec),
                   op);

    return ret_vec;
}

template<class T,
         class Alloc,
         class UnaryOperation>
auto transform(const std::vector<T, Alloc>& vec,
               UnaryOperation op)
{
    using OpReturnType = std::invoke_result_t<UnaryOperation, const T&>;
    using VecType = std::vector<OpReturnType>;

    VecType ret_vec;
    std::transform(std::cbegin(vec),
                   std::cend(vec),
                   std::back_inserter(ret_vec),
                   op);

    return ret_vec;
}

template<class T,
         class Alloc,
         class UnaryOperation>
auto transform(std::vector<T, Alloc>&& vec,
               UnaryOperation op)
{
    using OpReturnType = std::invoke_result_t<UnaryOperation, T&&>;
    using VecType = std::vector<OpReturnType>;

    VecType ret_vec;
    std::transform(std::make_move_iterator(std::begin(vec)),
                   std::make_move_iterator(std::end(vec)),
                   std::back_inserter(ret_vec),
                   op);

    return ret_vec;
}

template<class Key,
         class T,
         class Compare,
         class Alloc,
         class Pred>
void erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred)
{
    for(auto i = c.begin(), last = c.end(); i != last;) {
        if(pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
}

template<class T,
         class Compare,
         class Alloc,
         class Pred>
void erase_if(std::set<T, Compare, Alloc>& c, Pred pred)
{
    for(auto i = c.begin(), last = c.end(); i != last;) {
        if(pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
}

template<class Key,
         class T,
         class Hash,
         class KeyEqual,
         class Alloc,
         class Pred>
void erase_if(std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& c, Pred pred)
{
    for(auto i = c.begin(), last = c.end(); i != last;) {
        if(pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
}

template<class Key,
         class Hash,
         class KeyEqual,
         class Alloc,
         class Pred>
void erase_if(std::unordered_set<Key, Hash, KeyEqual, Alloc>& c, Pred pred)
{
    for(auto i = c.begin(), last = c.end(); i != last;) {
        if(pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
}

template<class T,
         class Alloc,
         class Pred>
void erase_if(std::vector<T, Alloc>& c, Pred pred)
{
    c.erase(std::remove_if(c.begin(),
                           c.end(),
                           pred),
            c.end());
}


} // namespace forge::utils
