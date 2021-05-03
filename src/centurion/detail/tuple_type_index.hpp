#ifndef CENTURION_DETAIL_TUPLE_TYPE_INDEX_HEADER
#define CENTURION_DETAIL_TUPLE_TYPE_INDEX_HEADER

#include <cstddef>      // size_t
#include <tuple>        // tuple
#include <type_traits>  // is_same_v
#include <utility>      // index_sequence, index_sequence_for

/// \cond FALSE
namespace cen::detail {

template <typename Target, typename Tuple>
class tuple_type_index;

template <typename Target, typename... T>
class tuple_type_index<Target, std::tuple<T...>>
{
  template <std::size_t... index>
  constexpr static auto find(std::index_sequence<index...>) -> int
  {
    return -1 + ((std::is_same_v<Target, T> ? index + 1 : 0) + ...);
  }

 public:
  inline constexpr static auto value = find(std::index_sequence_for<T...>{});
};

template <typename Target, typename... T>
inline constexpr int tuple_type_index_v = tuple_type_index<Target, T...>::value;

}  // namespace cen::detail
/// \endcond

#endif  // CENTURION_DETAIL_TUPLE_TYPE_INDEX_HEADER
