#include "catch.hpp"

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <cstdlib>
#include <memory>
#include <string>

template<class T>
std::string type_name()
{
  typedef typename std::remove_reference<T>::type TR;
  std::unique_ptr<char, void (*)(void*)>          own(
#ifndef _MSC_VER
      abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
      nullptr,
#endif
      std::free);
  std::string r = own != nullptr ? own.get() : typeid(TR).name();
  if (std::is_const<TR>::value) r += " const";
  if (std::is_volatile<TR>::value) r += " volatile";
  if (std::is_lvalue_reference<T>::value)
    r += "&";
  else if (std::is_rvalue_reference<T>::value)
    r += "&&";
  return r;
}

template<typename ARRAY>
class ArrayEqual : public Catch::MatcherBase<ARRAY>
{
  ARRAY a;

 public:
  ArrayEqual(ARRAY a_) : a(a_) {}

  // Performs the test for this matcher
  bool match(ARRAY const& a_) const override
  {
    if (a_.size() != a.size()) return false;

    for (size_t i = 0; i < a_.size(); i++) {
      if (a[i] != a_[i]) {
        return false;
      }
    }

    return true;
  }

  // Produces a string describing what this matcher does. It should
  // include any provided data (the begin/ end in this case) and
  // be written as if it were stating a fact (in the output it will be
  // preceded by the value under test).
  std::string describe() const override
  {
    std::ostringstream ss;
    ss << "is same as {";
    for (size_t i = 0; i < a.size(); i++) ss << a[i] << ", ";
    ss << "}";
    return ss.str();
  }
};

// The builder function
template<typename ARRAY>
ArrayEqual<ARRAY> IsEqualToArray(ARRAY a_)
{
  return ArrayEqual<ARRAY>(a_);
}
