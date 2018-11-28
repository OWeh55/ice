#ifndef GEN_DTYPE_H
#define GEN_DTYPE_H

#include <vector>
#include <string>

/**
 *  \brief Class describing kind of data elements
 *
 *  data packets are vectors\n
 *  type:  element type\n
 *  size: nr of elements\n
 */
class DType
{
public:
  /**
   * ctor with given values
   * \param type is element type
   * \param size id nr of elements
   */
  DType(): type(unknown), size(0) {}
  DType(int type, int size): type(type), size(size) {}

  /**
   * constants for data element types
   */
  static const int unknown = 0;
  static const int integer = 1;
  static const int floatingpoint = integer << 1;
  static const int string_type = floatingpoint << 1;
  static const int point = string_type << 1;
  static const int contur = point << 1;
  static const int region = contur << 1;
  static const int image = region << 1;
  static const int graph = image << 1;

  /**
   *  get textual description of type
   *
   *  \param i - "type"-Part of DType
   */
  static std::string getString(int i);

  /**
   *  get textual description of type
   *
   *  \param i - "type"-Part of DType
   */
  std::string getString() const;

  /**
   * \brief checks if this type matches a given pattern ignoring size
   *
   * normal types are int with one bit set,
   * in pattern more than one bit may be set, decribing a set of types
   * \param pattern is set of accepted types
   * \return test result
   * \sa match()
   */

  virtual bool matchType(DType pattern)
  {
    // pattern.type is bit mask
    return (type & pattern.type) == type;
  }

  virtual bool matchType(int pattern)
  {
    // pattern is bit mask
    return (type & pattern) == type;
  }

  /**
   * \brief checks if this type matches a given pattern
   *
   * Here type and size arce checked. \\
   * type may contain a "set of types"
   * \param pattern is set of valid types + size
   * \return test result
   * \sa matchType()
   */
  virtual bool match(DType pattern)
  {
    if (!matchType(pattern))
      return false;
    return size == pattern.size;
  }

  /**
   * \brief equality of DType instances
   */
  virtual bool operator==(const DType& second) const
  {
    return type == second.type && size == second.size;
  }

  virtual bool operator!=(const DType& second) const
  {
    return !(*this == second);
  }

//! element type
  int type;
//! number of elements
  int size;
};
#endif
