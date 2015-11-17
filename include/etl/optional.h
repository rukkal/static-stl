///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl

Copyright(c) 2015 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __ETL_OPTIONAL__
#define __ETL_OPTIONAL__

#include <type_traits>
#include "type_traits.h"
#include "exception.h"

namespace etl
{
  //*****************************************************************************
  /// A null option type.
  ///\ingroup utilities
  //*****************************************************************************
  class nullopt_t
  {
  public:

    // Convertible to any type of null non-member pointer.
    template<class T>
    operator T*() const
    {
      return 0;
    }

  private:

    // Can't take address of nullopt.
    void operator&() const;
  };

  //*****************************************************************************
  /// A null option.
  ///\ingroup utilities
  //*****************************************************************************
  const nullopt_t nullopt = {};

  //***************************************************************************
  /// Exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_exception : public exception
  {
  public:

    optional_exception(const char* what)
      : exception(what)
    {
    }
  };

  //***************************************************************************
  /// Invalid exception for optional.
  ///\ingroup list
  //***************************************************************************
  class optional_invalid : public optional_exception
  {
  public:

    optional_invalid()
      : optional_exception("optional: invalid")
    {
    }
  };

  //*****************************************************************************
  /// An optional type.
  /// If the optional type is not initialised then a type is not constructed.
  ///\tparam The type to store.
  ///\ingroup utilities
  //*****************************************************************************
  template <typename T>
  class optional
  {
  public:

    //***************************************************************************
    /// Constructor.
    //***************************************************************************
    optional()
      : valid(false)
    {
    }

    //***************************************************************************
    /// Constructor with nullopt.
    //***************************************************************************
    optional(etl::nullopt_t)
      : valid(false)
    {
    }

    //***************************************************************************
    /// Copy constructor.
    //***************************************************************************
    optional(const optional& other)
      : valid(bool(other))
    {
      if (valid)
      {
        new (&get_reference()) T(other.value());
      }
    }

    //***************************************************************************
    /// Constructor from value type.
    //***************************************************************************
    optional(const T& value)
    {
      new (&get_reference()) T(value);
      valid = true;
    }

    //***************************************************************************
    /// Destructor.
    //***************************************************************************
    ~optional()
    {
      if (valid)
      {
        get_reference().~T();
      }
    }

    //***************************************************************************
    /// Assignment operator from nullopt.
    //***************************************************************************
    optional& operator =(etl::nullopt_t)
    {
      if (valid)
      {
        get_reference().~T();
        valid = false;
      }

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from optional.
    //***************************************************************************
    optional& operator =(const optional& other)
    {
      if (valid && !bool(other))
      {
        get_reference().~T();
        valid = false;
      }
      else if (bool(other))
      {
        if (valid)
        {
          get_reference() = other.value();
        }
        else
        {
          new (&get_reference()) T(other.value());
          valid = true;
        }
      }

      return *this;
    }

    //***************************************************************************
    /// Assignment operator from value type.
    //***************************************************************************
    optional& operator =(const T& value)
    {
      if (valid)
      {
        get_reference() = value;
      }
      else
      {
        new (&get_reference()) T(value);
        valid = true;
      }

      return *this;
    }

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    T* operator ->()
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return &get_reference();
    }

    //***************************************************************************
    /// Pointer operator.
    //***************************************************************************
    const T* operator ->() const
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return &get_reference();
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    T& operator *()
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return get_reference();
    }

    //***************************************************************************
    /// Dereference operator.
    //***************************************************************************
    const T& operator *() const
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return get_reference();
    }

    //***************************************************************************
    /// Bool conversion operator.
    //***************************************************************************
    explicit operator bool() const
    {
      return valid;
    }

    //***************************************************************************
    /// Get a reference to the value.
    //***************************************************************************
    T& value()
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return get_reference();
    }

    //***************************************************************************
    /// Get a const reference to the value.
    //***************************************************************************
    const T& value() const
    {
#ifndef NDEBUG
      if (!valid)
      {
#ifdef ETL_THROW_EXCEPTIONS
        throw optional_invalid();
#else
        error_handler::error(optional_invalid());
#endif
      }
#endif

      return get_reference();
    }

    //***************************************************************************
    /// Gets the value or a default if no valid.
    //***************************************************************************
    T value_or(T default_value) const
    {
      return valid ? value() : default_value;
    }

    //***************************************************************************
    /// Swaps this value with another.
    //***************************************************************************
    void swap(optional& other)
    {
      optional temp(*this);
      *this = other;
      other = temp;
    }

  private:
    T& get_reference()
    {
      return reinterpret_cast<T&>(storage);
    }
    const T& get_reference() const
    {
      return reinterpret_cast<const T&>(storage);
    }

  private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    bool valid;
  };
}

//*************************************************************************
/// Swaps the values.
//*************************************************************************
template <typename T>
void swap(etl::optional<T>& lhs, etl::optional<T>& rhs)
{
  lhs.swap(rhs);
}

//***************************************************************************
/// Equality operator.
//***************************************************************************
template <typename T>
bool operator ==(const etl::optional<T>& lhs, const etl::optional<T>& rhs)
{
  if (bool(lhs) != bool(rhs))
  {
    return false;
  }
  else if (!bool(lhs) && !bool(rhs))
  {
    return true;
  }
  else
  {
    return lhs.value() == rhs.value();
  }
}

//***************************************************************************
/// Less than operator.
//***************************************************************************
template <typename T>
bool operator <(const etl::optional<T>& lhs, const etl::optional<T>& rhs)
{
  if (!bool(rhs))
  {
    return false;
  }
  else if (!bool(lhs))
  {
    return true;
  }
  else
  {
    return lhs.value() < rhs.value();
  }
}

//***************************************************************************
/// Equality operator.
//***************************************************************************
template <typename T>
bool operator ==(const etl::optional<T>& lhs, etl::nullopt_t)
{
  return !bool(lhs);
}

//***************************************************************************
/// Equality operator.
//***************************************************************************
template <typename T>
bool operator ==(etl::nullopt_t, const etl::optional<T>& rhs)
{
  return false;
}

//***************************************************************************
/// Less than operator.
//***************************************************************************
template <typename T>
bool operator <(const etl::optional<T>& lhs, etl::nullopt_t)
{
  return !bool(lhs);
}

//***************************************************************************
/// Less than operator.
//***************************************************************************
template <typename T>
bool operator <(etl::nullopt_t, const etl::optional<T>& rhs)
{
  return bool(rhs);
}

//***************************************************************************
/// Equality operator.
//**************************************************************************
template <typename T>
bool operator ==(const etl::optional<T>& lhs, const T& rhs)
{
  return bool(lhs) ? lhs.value() == rhs : false;
}

//***************************************************************************
/// Equality operator.
//**************************************************************************
template <typename T>
bool operator ==(const T& value, const etl::optional<T>& rhs)
{
  return bool(rhs) ? rhs.value() == value : false;
}

//***************************************************************************
/// Less than operator.
//***************************************************************************
template <typename T>
bool operator <(const etl::optional<T>& lhs, const T& rhs)
{
  return bool(lhs) ? lhs.value() < rhs : true;
}

//***************************************************************************
/// Make an optional.
//***************************************************************************
template <typename T>
etl::optional<typename etl::decay<T>::type> make_optional(T& value)
{
  return etl::optional<typename etl::decay<T>::type>(value);
}

#endif
