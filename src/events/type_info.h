/* wrapper for type_info, for use in stl containers
   borrowed from gcc source, so we don't need C++11
*/
struct type_index
{
type_index(const type_info& __rhs) noexcept
: _M_target(&__rhs) { }

bool
operator==(const type_index& __rhs) const noexcept
{ return *_M_target == *__rhs._M_target; }

bool
operator!=(const type_index& __rhs) const noexcept
{ return *_M_target != *__rhs._M_target; }

bool
operator<(const type_index& __rhs) const noexcept
{ return _M_target->before(*__rhs._M_target); }

bool
operator<=(const type_index& __rhs) const noexcept
{ return !__rhs._M_target->before(*_M_target); }

bool
operator>(const type_index& __rhs) const noexcept
{ return __rhs._M_target->before(*_M_target); }

bool
operator>=(const type_index& __rhs) const noexcept
{ return !_M_target->before(*__rhs._M_target); }

size_t
hash_code() const noexcept
{ return _M_target->hash_code(); }

const char*
name() const noexcept
{ return _M_target->name(); }

private:
const type_info* _M_target;
};

template<typename _Tp> struct hash;

/// std::hash specialization for type_index.
template<>
struct hash<type_index>
{
  typedef size_t        result_type;
  typedef type_index  argument_type;

  size_t
  operator()(const type_index& __ti) const noexcept
  { return __ti.hash_code(); }
};