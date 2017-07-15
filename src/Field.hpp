#ifndef Field_hpp
#define Field_hpp

#include <type_traits>
#include <typeinfo>
#include <vector>
#include <ostream>

#include <boost/multi_array.hpp>
using namespace boost;

#include "CoordinateSystem.hpp"
#include "Utils.hpp"

/** @file Field.hpp
  * @brief
  * @author C.D. Clark III
  * @date 06/13/17
  */

/** @class Field
  * @brief A class for storing data in a field.
  * @author C.D. Clark III
  */
template <typename T, std::size_t N>
using arrayND = multi_array<T, N, std::allocator<T>>;
template <typename T, std::size_t N>
using viewND = detail::multi_array::multi_array_view<T, N>;

template <typename QUANT, size_t NUMDIMS, typename COORD = QUANT,
          template <typename, size_t> class ARRAYND = arrayND,
          template <typename> class ARRAY1D = array1D>
class Field
{
  public:
    typedef ARRAYND<QUANT, NUMDIMS> array_type;
    typedef typename array_type::index index_type;
    typedef CoordinateSystem<COORD, NUMDIMS, ARRAY1D> cs_type;

  protected:
    std::shared_ptr<array_type> d;
    std::shared_ptr<cs_type> cs;

  public:
    template <typename... Args>
    Field(Args... args)
    {
        cs.reset(new cs_type(args...));
        array<int, NUMDIMS> sizes({args...});
        d.reset(new array_type(sizes));
    }
    template <typename I>
    Field(array<I, NUMDIMS> sizes)
    {
        cs.reset(new cs_type(sizes));
        d.reset(new array_type(sizes));
    }
    Field(std::shared_ptr<cs_type> cs_)
    {
        cs = cs_;

        std::vector<size_t> sizes(NUMDIMS);
        for (int i = 0; i < NUMDIMS; ++i)
            sizes[i] = cs->size(i);

        d.reset(new array_type(sizes));
    }

    Field(cs_type &cs_, array_type &d_)
    {
        d.reset(new array_type(d_));
        cs.reset(new cs_type(cs_.getAxes()));
    };
    virtual ~Field(){};


    // TODO: allow both of these function to be called set.
    template<typename F>
    auto set_f( F f )
    {
        auto N = d->num_elements();
        for (int i = 0; i < N; ++i) {
          auto ind = this->_1d2nd(i);
          d->operator()(ind) = f(ind, cs);
        }
    }

    template <typename Q>
    auto set(Q q)
    {
        auto N = d->num_elements();
        #pragma omp parallel for
        for (int i = 0; i < N; ++i) {
          auto ind = this->_1d2nd(i);
          d->operator()(ind) = q;
        }
    }

    // ELEMENT ACCESS

    // using an index container ( C array, std::vector, boost::array, etc )
    template <typename I,
              typename std::enable_if<is_index_cont<I>::value,int>::type = 0>
    const auto &operator()(I i) const
    {
        return (*d)(i);
    }

    template <typename I,
              typename std::enable_if<is_index_cont<I>::value,int>::type = 0>
    auto &operator()(I i)
    {
        return (*d)(i);
    }


    // indices given as separate arguments
    template <typename I,
              typename... Args,
              typename std::enable_if<is_integral<I>::value,int>::type = 0>
    const auto &operator()(I i, Args... args) const
    {
        return (*d)(array<I, NUMDIMS>({i,args...}));
    }

    template <typename I,
              typename... Args,
              typename std::enable_if<is_integral<I>::value,int>::type = 0>
    auto &operator()(I i, Args... args)
    {
        return (*d)(array<I, NUMDIMS>({i,args...}));
    }

    template <typename I>
    auto operator[](I i) const
    {
        return (*d)[i];
    };

    // coord system access
    auto &getCoordinateSystem() { return *cs; };
    auto getCoordinateSystemPtr() { return cs; };
    auto &getAxis(size_t i) { return cs->getAxis(i); }
    template <typename... Args>
    auto setCoordinateSystem(Args... args)
    {
        cs->set(args...);
    }

    template <typename... Args>
    auto getCoord(Args... args) const
    {
        return cs->getCoord(args...);
    }

    template <typename... Args>
    auto lower_bound(Args... args) const
    {
        return cs->lower_bound(args...);
    }

    template <typename... Args>
    auto upper_bound(Args... args) const
    {
        return cs->upper_bound(args...);
    }

    // data access
    const auto &getData() const { return *d; };
    auto &getData() { return *d; };

    const auto data() const { return d->data(); }
    auto data() { return d->data(); }

    template <int NDims>
    const auto
    slice(const detail::multi_array::index_gen<NUMDIMS, NDims> &ind) const
    {
        // get sliced data
        auto d_ = d->operator[](ind);
        // get sliced coordinate system
        auto cs_ = cs->slice(ind);

        return Field<QUANT, NDims, COORD, viewND, view1D>(cs_, d_);
    }

    template <int NDims>
    auto slice(const detail::multi_array::index_gen<NUMDIMS, NDims> &ind)
    {
        // get sliced data
        auto d_ = d->operator[](ind);
        // get sliced coordinate system
        auto cs_ = cs->slice(ind);

        return Field<QUANT, NDims, COORD, viewND, view1D>(cs_, d_);
    }

    auto size() const { return d->num_elements(); }
    auto size(int i) const { return d->shape()[i]; }

    friend std::ostream &operator<<(std::ostream &output, const Field &F)
    {
        auto N = F.d->num_elements();
        for (int i = 0; i < N; ++i) {
          auto ind = F._1d2nd(i);
          // insert blank lines between coordinate blocks
          if( i > 0)
            for(int j = NUMDIMS-1; j > 0; --j)
             if( ind[j] == 0 )
              output <<"\n";

          for(int j = 0; j < NUMDIMS; ++j)
            output << F.cs->getAxis(j)[ind[j]] << " ";
          output << F.d->operator()(ind) << "\n";
        }
        return output;
    }

  protected:
    auto _1d2nd(size_t i) const
    {
        auto shape = d->shape();

        array<size_t, NUMDIMS> ind;
        int NN = shape[0];
        for (int j = 1; j < NUMDIMS; ++j)
            NN *= shape[j];
        for (int j = 0; j < NUMDIMS; ++j) {
            NN /= shape[j];
            ind[j] = i / NN;
            i -= ind[j] * NN;
        }
        return ind;
    }
};

#endif
