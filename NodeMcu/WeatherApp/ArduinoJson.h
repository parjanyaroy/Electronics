// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#ifdef __cplusplus

#define ARDUINOJSON_VERSION "6.2.3-beta"
#define ARDUINOJSON_VERSION_MAJOR 6
#define ARDUINOJSON_VERSION_MINOR 2
#define ARDUINOJSON_VERSION_REVISION 3
#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t
#include <string.h>
#ifndef ARDUINOJSON_EMBEDDED_MODE
#if defined(ARDUINO) || defined(__IAR_SYSTEMS_ICC__) || defined(__XC) || \
    defined(__ARMCC_VERSION)
#define ARDUINOJSON_EMBEDDED_MODE 1
#else
#define ARDUINOJSON_EMBEDDED_MODE 0
#endif
#endif
#if ARDUINOJSON_EMBEDDED_MODE
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 0
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#ifndef ARDUINOJSON_USE_INT64
#define ARDUINOJSON_USE_INT64 0
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 0
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 0
#endif
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 10
#endif
#else  // ARDUINOJSON_EMBEDDED_MODE
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 1
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define ARDUINOJSON_USE_LONG_LONG 1
#else
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#endif
#ifndef ARDUINOJSON_USE_INT64
#if defined(_MSC_VER) && _MSC_VER <= 1700
#define ARDUINOJSON_USE_INT64 1
#else
#define ARDUINOJSON_USE_INT64 0
#endif
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 1
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 1
#endif
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 50
#endif
#endif  // ARDUINOJSON_EMBEDDED_MODE
#ifdef ARDUINO
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#endif
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#endif
#else  // ARDUINO
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#endif
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#endif
#endif  // ARDUINO
#ifndef ARDUINOJSON_ENABLE_PROGMEM
#ifdef PROGMEM
#define ARDUINOJSON_ENABLE_PROGMEM 1
#else
#define ARDUINOJSON_ENABLE_PROGMEM 0
#endif
#endif
#ifndef ARDUINOJSON_ENABLE_ALIGNMENT
#ifdef ARDUINO_ARCH_AVR
#define ARDUINOJSON_ENABLE_ALIGNMENT 0
#else
#define ARDUINOJSON_ENABLE_ALIGNMENT 1
#endif
#endif
#ifndef ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#endif
#ifndef ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD 1e-5
#endif
#if ARDUINOJSON_USE_LONG_LONG && ARDUINOJSON_USE_INT64
#error ARDUINOJSON_USE_LONG_LONG and ARDUINOJSON_USE_INT64 cannot be set together
#endif
#ifndef ARDUINOJSON_LITTLE_ENDIAN
#if defined(_MSC_VER) ||                                                      \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
    (defined(__LITTLE_ENDIAN__))
#define ARDUINOJSON_LITTLE_ENDIAN 1
#else
#define ARDUINOJSON_LITTLE_ENDIAN 0
#endif
#endif
#ifdef _MSC_VER  // Visual Studio
#define FORCE_INLINE  // __forceinline causes C4714 when returning std::string
#define NO_INLINE __declspec(noinline)
#define DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__)  // GCC or Clang
#define FORCE_INLINE __attribute__((always_inline))
#define NO_INLINE __attribute__((noinline))
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define DEPRECATED(msg) __attribute__((deprecated))
#endif
#else  // Other compilers
#define FORCE_INLINE
#define NO_INLINE
#define DEPRECATED(msg)
#endif
namespace ArduinoJson {
namespace Internals {
class JsonBuffer {
 public:
  virtual void *alloc(size_t size) = 0;
 protected:
  ~JsonBuffer() {}
  static FORCE_INLINE size_t round_size_up(size_t bytes) {
#if ARDUINOJSON_ENABLE_ALIGNMENT
    const size_t x = sizeof(void *) - 1;
    return (bytes + x) & ~x;
#else
    return bytes;
#endif
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class JsonBufferAllocated {
 public:
  void *operator new(size_t n, JsonBuffer *jsonBuffer) throw() {
    if (!jsonBuffer) return NULL;
    return jsonBuffer->alloc(n);
  }
  void operator delete(void *, JsonBuffer *)throw();
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct ListNode : public Internals::JsonBufferAllocated {
  ListNode() throw() : next(NULL) {}
  ListNode<T> *next;
  T content;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
class ListConstIterator {
 public:
  explicit ListConstIterator(const ListNode<T> *node = NULL) : _node(node) {}
  const T &operator*() const {
    return _node->content;
  }
  const T *operator->() {
    return &_node->content;
  }
  bool operator==(const ListConstIterator<T> &other) const {
    return _node == other._node;
  }
  bool operator!=(const ListConstIterator<T> &other) const {
    return _node != other._node;
  }
  ListConstIterator<T> &operator++() {
    if (_node) _node = _node->next;
    return *this;
  }
  ListConstIterator<T> &operator+=(size_t distance) {
    while (_node && distance) {
      _node = _node->next;
      --distance;
    }
    return *this;
  }
 private:
  const ListNode<T> *_node;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
class List;
template <typename T>
class ListIterator {
  friend class List<T>;
 public:
  explicit ListIterator(ListNode<T> *node = NULL) : _node(node) {}
  T &operator*() const {
    return _node->content;
  }
  T *operator->() {
    return &_node->content;
  }
  bool operator==(const ListIterator<T> &other) const {
    return _node == other._node;
  }
  bool operator!=(const ListIterator<T> &other) const {
    return _node != other._node;
  }
  ListIterator<T> &operator++() {
    if (_node) _node = _node->next;
    return *this;
  }
  ListIterator<T> &operator+=(size_t distance) {
    while (_node && distance) {
      _node = _node->next;
      --distance;
    }
    return *this;
  }
  operator ListConstIterator<T>() const {
    return ListConstIterator<T>(_node);
  }
 private:
  ListNode<T> *_node;
};
}
}
namespace ArduinoJson {
namespace Internals {
template <typename T>
class List {
 public:
  typedef T value_type;
  typedef ListNode<T> node_type;
  typedef ListIterator<T> iterator;
  typedef ListConstIterator<T> const_iterator;
  explicit List(JsonBuffer *buf) : _buffer(buf), _firstNode(NULL) {}
  size_t size() const {
    size_t nodeCount = 0;
    for (node_type *node = _firstNode; node; node = node->next) nodeCount++;
    return nodeCount;
  }
  iterator add() {
    node_type *newNode = new (_buffer) node_type();
    if (_firstNode) {
      node_type *lastNode = _firstNode;
      while (lastNode->next) lastNode = lastNode->next;
      lastNode->next = newNode;
    } else {
      _firstNode = newNode;
    }
    return iterator(newNode);
  }
  iterator begin() {
    return iterator(_firstNode);
  }
  iterator end() {
    return iterator(NULL);
  }
  const_iterator begin() const {
    return const_iterator(_firstNode);
  }
  const_iterator end() const {
    return const_iterator(NULL);
  }
  void remove(iterator it) {
    node_type *nodeToRemove = it._node;
    if (!nodeToRemove) return;
    if (nodeToRemove == _firstNode) {
      _firstNode = nodeToRemove->next;
    } else {
      for (node_type *node = _firstNode; node; node = node->next)
        if (node->next == nodeToRemove) node->next = nodeToRemove->next;
    }
  }
  JsonBuffer &buffer() const {
    return *_buffer;
  }
  JsonBuffer *_buffer;  // TODO!!
 protected:
  void clear() {
    _firstNode = 0;
  }
 private:
  node_type *_firstNode;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
#if ARDUINOJSON_USE_DOUBLE
typedef double JsonFloat;
#else
typedef float JsonFloat;
#endif
}
}
namespace ArduinoJson {
namespace Internals {
#if ARDUINOJSON_USE_LONG_LONG
typedef long long JsonInteger;
typedef unsigned long long JsonUInt;
#elif ARDUINOJSON_USE_INT64
typedef __int64 JsonInteger;
typedef unsigned _int64 JsonUInt;
#else
typedef long JsonInteger;
typedef unsigned long JsonUInt;
#endif
}
}
namespace ArduinoJson {
namespace Internals {
struct JsonArrayData;
struct JsonObjectData;
union JsonVariantContent {
  JsonFloat asFloat;         // used for double and float
  JsonUInt asInteger;        // used for bool, char, short, int and longs
  JsonArrayData* asArray;    // asArray cannot be null
  JsonObjectData* asObject;  // asObject cannot be null
  const char* asString;      // asString can be null
  struct {
    const char* data;
    size_t size;
  } asRaw;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct JsonVariantDefault {
  static T get() {
    return T();
  }
};
template <typename T>
struct JsonVariantDefault<const T> : JsonVariantDefault<T> {};
template <typename T>
struct JsonVariantDefault<T&> : JsonVariantDefault<T> {};
}
}
namespace ArduinoJson {
namespace Internals {
enum JsonVariantType {
  JSON_UNDEFINED,         // JsonVariant has not been initialized
  JSON_UNPARSED,          // JsonVariant contains an unparsed string
  JSON_STRING,            // JsonVariant stores a const char*
  JSON_BOOLEAN,           // JsonVariant stores a bool
  JSON_POSITIVE_INTEGER,  // JsonVariant stores an JsonUInt
  JSON_NEGATIVE_INTEGER,  // JsonVariant stores an JsonUInt that must be negated
  JSON_ARRAY,             // JsonVariant stores a pointer to a JsonArrayData
  JSON_OBJECT,            // JsonVariant stores a pointer to a JsonObjectData
  JSON_FLOAT              // JsonVariant stores a JsonFloat
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct JsonVariantAs {
  typedef T type;
};
template <>
struct JsonVariantAs<char*> {
  typedef const char* type;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TImpl>
class JsonVariantCasts {
 public:
  template <typename T>
  FORCE_INLINE operator T() const {
    return impl()->template as<T>();
  }
 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <bool Condition, typename T = void>
struct enable_if {};
template <typename T>
struct enable_if<true, T> {
  typedef T type;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T, T v>
struct integral_constant {
  static const T value = v;
};
typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct is_array : false_type {};
template <typename T>
struct is_array<T[]> : true_type {};
template <typename T, size_t N>
struct is_array<T[N]> : true_type {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TBase, typename TDerived>
class is_base_of {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  typedef char Yes[1];
  typedef char No[2];
  static Yes &probe(const TBase *);
  static No &probe(...);
 public:
  enum {
    value = sizeof(probe(reinterpret_cast<TDerived *>(0))) == sizeof(Yes)
  };
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct is_const : false_type {};
template <typename T>
struct is_const<const T> : true_type {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename>
struct is_floating_point : false_type {};
template <>
struct is_floating_point<float> : true_type {};
template <>
struct is_floating_point<double> : true_type {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T, typename U>
struct is_same : false_type {};
template <typename T>
struct is_same<T, T> : true_type {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct is_integral {
  static const bool value =
      is_same<T, signed char>::value || is_same<T, unsigned char>::value ||
      is_same<T, signed short>::value || is_same<T, unsigned short>::value ||
      is_same<T, signed int>::value || is_same<T, unsigned int>::value ||
      is_same<T, signed long>::value || is_same<T, unsigned long>::value ||
#if ARDUINOJSON_USE_LONG_LONG
      is_same<T, unsigned long long>::value ||
      is_same<T, signed long long>::value ||
#endif
#if ARDUINOJSON_USE_INT64
      is_same<T, unsigned __int64>::value ||
      is_same<T, signed __int64>::value ||
#endif
      is_same<T, char>::value;
};
template <typename T>
struct is_integral<const T> : is_integral<T> {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename>
struct is_signed : false_type {};
template <>
struct is_signed<char> : true_type {};
template <>
struct is_signed<signed char> : true_type {};
template <>
struct is_signed<signed short> : true_type {};
template <>
struct is_signed<signed int> : true_type {};
template <>
struct is_signed<signed long> : true_type {};
template <>
struct is_signed<float> : true_type {};
template <>
struct is_signed<double> : true_type {};
#if ARDUINOJSON_USE_LONG_LONG
template <>
struct is_signed<signed long long> : true_type {};
#endif
#if ARDUINOJSON_USE_INT64
template <>
struct is_signed<signed __int64> : true_type {};
#endif
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename>
struct is_unsigned : false_type {};
template <>
struct is_unsigned<bool> : true_type {};
template <>
struct is_unsigned<unsigned char> : true_type {};
template <>
struct is_unsigned<unsigned short> : true_type {};
template <>
struct is_unsigned<unsigned int> : true_type {};
template <>
struct is_unsigned<unsigned long> : true_type {};
#if ARDUINOJSON_USE_LONG_LONG
template <>
struct is_unsigned<unsigned long long> : true_type {};
#endif
#if ARDUINOJSON_USE_INT64
template <>
struct is_unsigned<unsigned __int64> : true_type {};
#endif
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct remove_const {
  typedef T type;
};
template <typename T>
struct remove_const<const T> {
  typedef T type;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
struct remove_reference {
  typedef T type;
};
template <typename T>
struct remove_reference<T&> {
  typedef T type;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class JsonVariantTag {};
template <typename T>
struct IsVariant : is_base_of<JsonVariantTag, T> {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename>
struct IsString : false_type {};
template <typename T>
struct IsString<const T> : IsString<T> {};
template <typename T>
struct IsString<T&> : IsString<T> {};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class FixedSizeRamString {
 public:
  FixedSizeRamString(const char* str, size_t n) : _str(str), _size(n) {}
  bool equals(const char* expected) const {
    const char* actual = reinterpret_cast<const char*>(_str);
    if (!actual || !expected) return actual == expected;
    return strcmp(actual, expected) == 0;
  }
  bool is_null() const {
    return !_str;
  }
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    if (!_str) return NULL;
    void* dup = buffer->alloc(_size);
    if (!dup) return NULL;
    memcpy(dup, _str, _size);
    return static_cast<const char*>(dup);
  }
  size_t size() const {
    return strlen(reinterpret_cast<const char*>(_str));
  }
 private:
  const char* _str;
  size_t _size;
};
template <typename TChar>
inline FixedSizeRamString makeString(const TChar* str, size_t size) {
  return FixedSizeRamString(reinterpret_cast<const char*>(str), size);
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class ZeroTerminatedRamString {
 public:
  ZeroTerminatedRamString(const char* str) : _str(str) {}
  bool equals(const char* expected) const {
    const char* actual = reinterpret_cast<const char*>(_str);
    if (!actual || !expected) return actual == expected;
    return strcmp(actual, expected) == 0;
  }
  bool is_null() const {
    return !_str;
  }
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    if (!_str) return NULL;
    size_t n = size() + 1;
    void* dup = buffer->alloc(n);
    if (!dup) return NULL;
    memcpy(dup, _str, n);
    return static_cast<const char*>(dup);
  }
  size_t size() const {
    return strlen(reinterpret_cast<const char*>(_str));
  }
 private:
  const char* _str;
};
template <typename TChar>
inline ZeroTerminatedRamString makeString(const TChar* str) {
  return ZeroTerminatedRamString(reinterpret_cast<const char*>(str));
}
template <typename TChar>
struct IsString<TChar*> {
  static const bool value = sizeof(TChar) == 1;
};
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
namespace ArduinoJson {
namespace Internals {
class StlString {
 public:
  StlString(const std::string& str) : _str(&str) {}
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    size_t n = _str->length() + 1;
    void* dup = buffer->alloc(n);
    if (dup != NULL) memcpy(dup, _str->c_str(), n);
    return static_cast<const char*>(dup);
  }
  bool is_null() const {
    return false;
  }
  bool equals(const char* expected) const {
    return *_str == expected;
  }
  const char* data() const {
    return _str->data();
  }
  size_t size() const {
    return _str->size();
  }
 private:
  const std::string* _str;
};
template <>
struct IsString<std::string> : true_type {};
inline StlString makeString(const std::string& str) {
  return StlString(str);
}
}  // namespace Internals
}  // namespace ArduinoJson
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#include <WString.h>
namespace ArduinoJson {
namespace Internals {
class ArduinoString {
 public:
  ArduinoString(const ::String& str) : _str(&str) {}
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    if (is_null()) return NULL;
    size_t n = _str->length() + 1;
    void* dup = buffer->alloc(n);
    if (dup != NULL) memcpy(dup, _str->c_str(), n);
    return static_cast<const char*>(dup);
  }
  bool is_null() const {
    return !_str->c_str();
  }
  bool equals(const char* expected) const {
    const char* actual = _str->c_str();
    if (!actual || !expected) return actual == expected;
    return 0 == strcmp(actual, expected);
  }
  const char* data() const {
    return _str->c_str();
  }
  size_t size() const {
    return _str->length();
  }
 private:
  const ::String* _str;
};
template <>
struct IsString< ::String> : true_type {};
template <>
struct IsString< ::StringSumHelper> : true_type {};
inline ArduinoString makeString(const ::String& str) {
  return ArduinoString(str);
}
}  // namespace Internals
}  // namespace ArduinoJson
#endif
#if ARDUINOJSON_ENABLE_PROGMEM
namespace ArduinoJson {
namespace Internals {
class FixedSizeFlashString {
 public:
  FixedSizeFlashString(const __FlashStringHelper* str, size_t sz)
      : _str(str), _size(sz) {}
  bool equals(const char* expected) const {
    const char* actual = reinterpret_cast<const char*>(_str);
    if (!actual || !expected) return actual == expected;
    return strncmp_P(expected, actual, _size) == 0;
  }
  bool is_null() const {
    return !_str;
  }
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    if (!_str) return NULL;
    void* dup = buffer->alloc(_size);
    if (dup != NULL) memcpy_P(dup, (const char*)_str, _size);
    return static_cast<const char*>(dup);
  }
  size_t size() const {
    return strlen_P(reinterpret_cast<const char*>(_str));
  }
 private:
  const __FlashStringHelper* _str;
  size_t _size;
};
inline FixedSizeFlashString makeString(const __FlashStringHelper* str,
                                       size_t sz) {
  return FixedSizeFlashString(str, sz);
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class ZeroTerminatedFlashString {
 public:
  ZeroTerminatedFlashString(const __FlashStringHelper* str) : _str(str) {}
  bool equals(const char* expected) const {
    const char* actual = reinterpret_cast<const char*>(_str);
    if (!actual || !expected) return actual == expected;
    return strcmp_P(expected, actual) == 0;
  }
  bool is_null() const {
    return !_str;
  }
  template <typename Buffer>
  const char* save(Buffer* buffer) const {
    if (!_str) return NULL;
    size_t n = size() + 1;  // copy the terminator
    void* dup = buffer->alloc(n);
    if (dup != NULL) memcpy_P(dup, (const char*)_str, n);
    return static_cast<const char*>(dup);
  }
  size_t size() const {
    return strlen_P(reinterpret_cast<const char*>(_str));
  }
 private:
  const __FlashStringHelper* _str;
};
inline ZeroTerminatedFlashString makeString(const __FlashStringHelper* str) {
  return ZeroTerminatedFlashString(str);
}
template <>
struct IsString<const __FlashStringHelper*> : true_type {};
}  // namespace Internals
}  // namespace ArduinoJson
#endif
namespace ArduinoJson {
class JsonArray;
class JsonObject;
namespace Internals {
template <typename TImpl>
class JsonVariantComparisons {
 public:
  template <typename TComparand>
  friend bool operator==(const JsonVariantComparisons &variant,
                         TComparand comparand) {
    return variant.equals(comparand);
  }
  template <typename TComparand>
  friend typename enable_if<!IsVariant<TComparand>::value, bool>::type
  operator==(TComparand comparand, const JsonVariantComparisons &variant) {
    return variant.equals(comparand);
  }
  template <typename TComparand>
  friend bool operator!=(const JsonVariantComparisons &variant,
                         TComparand comparand) {
    return !variant.equals(comparand);
  }
  template <typename TComparand>
  friend typename enable_if<!IsVariant<TComparand>::value, bool>::type
  operator!=(TComparand comparand, const JsonVariantComparisons &variant) {
    return !variant.equals(comparand);
  }
  template <typename TComparand>
  friend bool operator<=(const JsonVariantComparisons &left, TComparand right) {
    return left.as<TComparand>() <= right;
  }
  template <typename TComparand>
  friend bool operator<=(TComparand comparand,
                         const JsonVariantComparisons &variant) {
    return comparand <= variant.as<TComparand>();
  }
  template <typename TComparand>
  friend bool operator>=(const JsonVariantComparisons &variant,
                         TComparand comparand) {
    return variant.as<TComparand>() >= comparand;
  }
  template <typename TComparand>
  friend bool operator>=(TComparand comparand,
                         const JsonVariantComparisons &variant) {
    return comparand >= variant.as<TComparand>();
  }
  template <typename TComparand>
  friend bool operator<(const JsonVariantComparisons &varian,
                        TComparand comparand) {
    return varian.as<TComparand>() < comparand;
  }
  template <typename TComparand>
  friend bool operator<(TComparand comparand,
                        const JsonVariantComparisons &variant) {
    return comparand < variant.as<TComparand>();
  }
  template <typename TComparand>
  friend bool operator>(const JsonVariantComparisons &variant,
                        TComparand comparand) {
    return variant.as<TComparand>() > comparand;
  }
  template <typename TComparand>
  friend bool operator>(TComparand comparand,
                        const JsonVariantComparisons &variant) {
    return comparand > variant.as<TComparand>();
  }
 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
  template <typename T>
  const typename JsonVariantAs<T>::type as() const {
    return impl()->template as<T>();
  }
  template <typename T>
  bool is() const {
    return impl()->template is<T>();
  }
  template <typename TString>
  typename enable_if<IsString<TString>::value, bool>::type equals(
      const TString &comparand) const {
    return makeString(comparand).equals(as<const char *>());
  }
  template <typename TComparand>
  typename enable_if<
      !IsVariant<TComparand>::value && !IsString<TComparand>::value, bool>::type
  equals(const TComparand &comparand) const {
    return as<TComparand>() == comparand;
  }
  template <typename TVariant2>
  bool equals(const JsonVariantComparisons<TVariant2> &right) const {
    using namespace Internals;
    if (is<bool>() && right.template is<bool>())
      return as<bool>() == right.template as<bool>();
    if (is<JsonInteger>() && right.template is<JsonInteger>())
      return as<JsonInteger>() == right.template as<JsonInteger>();
    if (is<JsonFloat>() && right.template is<JsonFloat>())
      return as<JsonFloat>() == right.template as<JsonFloat>();
    if (is<JsonArray>() && right.template is<JsonArray>())
      return as<JsonArray>() == right.template as<JsonArray>();
    if (is<JsonObject>() && right.template is<JsonObject>())
      return as<JsonObject>() == right.template as<JsonObject>();
    if (is<char *>() && right.template is<char *>())
      return makeString(as<char *>()).equals(right.template as<char *>());
    return false;
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TImpl>
class JsonVariantOr {
 public:
  template <typename T>
  typename enable_if<!is_integral<T>::value, T>::type operator|(
      const T &defaultValue) const {
    if (impl()->template is<T>())
      return impl()->template as<T>();
    else
      return defaultValue;
  }
  const char *operator|(const char *defaultValue) const {
    const char *value = impl()->template as<const char *>();
    return value ? value : defaultValue;
  }
  template <typename Integer>
  typename enable_if<is_integral<Integer>::value, Integer>::type operator|(
      const Integer &defaultValue) const {
    if (impl()->template is<double>())
      return impl()->template as<Integer>();
    else
      return defaultValue;
  }
 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
class JsonArray;
class JsonObject;
namespace Internals {
class JsonArraySubscript;
template <typename TKey>
class JsonObjectSubscript;
template <typename TImpl>
class JsonVariantSubscripts {
 public:
  size_t size() const {
    return impl()->template as<JsonArray>().size() +
           impl()->template as<JsonObject>().size();
  }
  FORCE_INLINE const JsonArraySubscript operator[](size_t index) const;
  FORCE_INLINE JsonArraySubscript operator[](size_t index);
  template <typename TString>
  FORCE_INLINE
      typename enable_if<IsString<TString>::value,
                         const JsonObjectSubscript<const TString &> >::type
      operator[](const TString &key) const;
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value,
                                  JsonObjectSubscript<const TString &> >::type
  operator[](const TString &key);
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString *>::value,
                                  JsonObjectSubscript<TString *> >::type
  operator[](TString *key);
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString *>::value,
                                  const JsonObjectSubscript<TString *> >::type
  operator[](TString *key) const;
 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TImpl>
class JsonVariantBase : public JsonVariantCasts<TImpl>,
                        public JsonVariantComparisons<TImpl>,
                        public JsonVariantOr<TImpl>,
                        public JsonVariantSubscripts<TImpl>,
                        public JsonVariantTag {};
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#endif
#if ARDUINOJSON_ENABLE_STD_STRING
#endif
namespace ArduinoJson {
namespace Internals {
template <typename>
struct IsWriteableString : false_type {};
template <typename TString>
class DynamicStringWriter {};
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
template <>
struct IsWriteableString<String> : true_type {};
template <>
class DynamicStringWriter<String> {
 public:
  DynamicStringWriter(String &str) : _str(&str) {}
  size_t write(uint8_t c) {
    _str->operator+=(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t *s, size_t n) {
    _str->reserve(_str->length() + n);
    while (n > 0) {
      _str->operator+=(static_cast<char>(*s++));
      n--;
    }
    return n;
  }
 private:
  String *_str;
};
#endif
#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct IsWriteableString<std::string> : true_type {};
template <>
class DynamicStringWriter<std::string> {
 public:
  DynamicStringWriter(std::string &str) : _str(&str) {}
  size_t write(uint8_t c) {
    _str->operator+=(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t *s, size_t n) {
    _str->append(reinterpret_cast<const char *>(s), n);
    return n;
  }
 private:
  std::string *_str;
};
#endif
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
class SerializedValue {
 public:
  explicit SerializedValue(T str) : _str(str) {}
  operator T() const {
    return _str;
  }
  const char* data() const {
    return _str.c_str();
  }
  size_t size() const {
    return _str.length();
  }
 private:
  T _str;
};
template <typename TChar>
class SerializedValue<TChar*> {
 public:
  explicit SerializedValue(TChar* p, size_t n) : _data(p), _size(n) {}
  operator TChar*() const {
    return _data;
  }
  TChar* data() const {
    return _data;
  }
  size_t size() const {
    return _size;
  }
 private:
  TChar* _data;
  size_t _size;
};
}  // namespace Internals
template <typename T>
inline Internals::SerializedValue<T> serialized(T str) {
  return Internals::SerializedValue<T>(str);
}
template <typename TChar>
inline Internals::SerializedValue<TChar*> serialized(TChar* p) {
  return Internals::SerializedValue<TChar*>(p, Internals::makeString(p).size());
}
template <typename TChar>
inline Internals::SerializedValue<TChar*> serialized(TChar* p, size_t n) {
  return Internals::SerializedValue<TChar*>(p, n);
}
}  // namespace ArduinoJson
namespace ArduinoJson {
class JsonArray;
class JsonObject;
class JsonVariant : public Internals::JsonVariantBase<JsonVariant> {
 public:
  JsonVariant() : _type(Internals::JSON_UNDEFINED) {}
  JsonVariant(bool value) {
    using namespace Internals;
    _type = JSON_BOOLEAN;
    _content.asInteger = static_cast<JsonUInt>(value);
  }
  template <typename T>
  JsonVariant(T value,
              typename Internals::enable_if<
                  Internals::is_floating_point<T>::value>::type * = 0) {
    using namespace Internals;
    _type = JSON_FLOAT;
    _content.asFloat = static_cast<JsonFloat>(value);
  }
  template <typename T>
  JsonVariant(
      T value,
      typename Internals::enable_if<Internals::is_integral<T>::value &&
                                    Internals::is_signed<T>::value>::type * =
          0) {
    using namespace Internals;
    if (value >= 0) {
      _type = JSON_POSITIVE_INTEGER;
      _content.asInteger = static_cast<JsonUInt>(value);
    } else {
      _type = JSON_NEGATIVE_INTEGER;
      _content.asInteger = ~static_cast<JsonUInt>(value) + 1;
    }
  }
  template <typename T>
  JsonVariant(
      T value,
      typename Internals::enable_if<Internals::is_integral<T>::value &&
                                    Internals::is_unsigned<T>::value>::type * =
          0) {
    using namespace Internals;
    _type = JSON_POSITIVE_INTEGER;
    _content.asInteger = static_cast<JsonUInt>(value);
  }
  template <typename TChar>
  JsonVariant(const TChar *value,
              typename Internals::enable_if<sizeof(TChar) == 1>::type * = 0) {
    _type = Internals::JSON_STRING;
    _content.asString = reinterpret_cast<const char *>(value);
  }
  JsonVariant(Internals::SerializedValue<const char *> value) {
    _type = Internals::JSON_UNPARSED;
    _content.asRaw.data = value.data();
    _content.asRaw.size = value.size();
  }
  JsonVariant(JsonArray array);
  JsonVariant(JsonObject object);
  template <typename T>
  const typename Internals::enable_if<Internals::is_integral<T>::value, T>::type
  as() const {
    return variantAsInteger<T>();
  }
  template <typename T>
  const typename Internals::enable_if<Internals::is_same<T, bool>::value,
                                      T>::type
  as() const {
    return variantAsInteger<int>() != 0;
  }
  template <typename T>
  const typename Internals::enable_if<Internals::is_floating_point<T>::value,
                                      T>::type
  as() const {
    return variantAsFloat<T>();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, const char *>::value ||
                                    Internals::is_same<T, char *>::value,
                                const char *>::type
  as() const {
    return variantAsString();
  }
  template <typename T>
  typename Internals::enable_if<Internals::IsWriteableString<T>::value, T>::type
  as() const {
    const char *cstr = variantAsString();
    if (cstr) return T(cstr);
    T s;
    serializeJson(*this, s);
    return s;
  }
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonArray>::value,
      JsonArray>::type
  as() const;
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonObject>::value,
      T>::type
  as() const;
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonVariant>::value,
                                T>::type
  as() const {
    return *this;
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_integral<T>::value, bool>::type
  is() const {
    return variantIsInteger();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_floating_point<T>::value,
                                bool>::type
  is() const {
    return variantIsFloat();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, bool>::value, bool>::type
  is() const {
    return variantIsBoolean();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, const char *>::value ||
                                    Internals::is_same<T, char *>::value,
                                bool>::type
  is() const {
    return variantIsString();
  }
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonArray>::value,
      bool>::type
  is() const {
    return variantIsArray();
  }
  template <typename T>
  typename Internals::enable_if<
      Internals::is_same<typename Internals::remove_const<T>::type,
                         JsonObject>::value,
      bool>::type
  is() const {
    return variantIsObject();
  }
  bool isNull() const {
    return _type == Internals::JSON_UNDEFINED;
  }
  template <typename Visitor>
  void visit(Visitor &visitor) const {
    using namespace Internals;
    switch (_type) {
      case JSON_FLOAT:
        return visitor.acceptFloat(_content.asFloat);
      case JSON_ARRAY:
        return visitor.acceptArray(_content.asArray);
      case JSON_OBJECT:
        return visitor.acceptObject(_content.asObject);
      case JSON_STRING:
        return visitor.acceptString(_content.asString);
      case JSON_UNPARSED:
        return visitor.acceptRawJson(_content.asRaw.data, _content.asRaw.size);
      case JSON_NEGATIVE_INTEGER:
        return visitor.acceptNegativeInteger(_content.asInteger);
      case JSON_POSITIVE_INTEGER:
        return visitor.acceptPositiveInteger(_content.asInteger);
      case JSON_BOOLEAN:
        return visitor.acceptBoolean(_content.asInteger != 0);
      default:  // JSON_UNDEFINED
        return visitor.acceptNull();
    }
  }
 private:
  JsonArray variantAsArray() const;
  JsonObject variantAsObject() const;
  const char *variantAsString() const;
  template <typename T>
  T variantAsFloat() const;
  template <typename T>
  T variantAsInteger() const;
  bool variantIsBoolean() const;
  bool variantIsFloat() const;
  bool variantIsInteger() const;
  bool variantIsArray() const {
    return _type == Internals::JSON_ARRAY;
  }
  bool variantIsObject() const {
    return _type == Internals::JSON_OBJECT;
  }
  bool variantIsString() const {
    return _type == Internals::JSON_STRING;
  }
  Internals::JsonVariantType _type;
  Internals::JsonVariantContent _content;
};
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename Source, typename Enable = void>
struct ValueSaver {
  template <typename Destination>
  static bool save(JsonBuffer*, Destination& destination, Source source) {
    destination = source;
    return true;
  }
};
template <typename TString>
struct ValueSaver<
    TString, typename enable_if<IsString<TString>::value &&
                                !is_same<const char*, TString>::value>::type> {
  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dest, TString source) {
    const char* dup = makeString(source).save(buffer);
    if (!dup) return false;
    dest = dup;
    return true;
  }
};
template <typename TString>
struct ValueSaver<
    const SerializedValue<TString>&,
    typename enable_if<!is_same<const char*, TString>::value>::type> {
  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dest,
                   const SerializedValue<TString>& source) {
    const char* dup = makeString(source.data(), source.size()).save(buffer);
    if (!dup) return false;
    dest = SerializedValue<const char*>(dup, source.size());
    return true;
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
#define JSON_ARRAY_SIZE(NUMBER_OF_ELEMENTS)        \
  (sizeof(ArduinoJson::Internals::JsonArrayData) + \
   (NUMBER_OF_ELEMENTS) *                          \
       sizeof(ArduinoJson::Internals::JsonArrayData::node_type))
namespace ArduinoJson {
namespace Internals {
struct JsonArrayData : List<JsonVariant>, JsonBufferAllocated {
  explicit JsonArrayData(JsonBuffer *buf) throw() : List<JsonVariant>(buf) {}
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
class JsonObject;
namespace Internals {
class JsonArraySubscript;
}
class JsonArray {
  friend class JsonVariant;
 public:
  typedef Internals::JsonArrayData::iterator iterator;
  typedef Internals::JsonArrayData::const_iterator const_iterator;
  JsonArray() : _data(0) {}
  JsonArray(Internals::JsonArrayData* arr) : _data(arr) {}
  JsonArray(Internals::JsonBuffer* buf)
      : _data(new (buf) Internals::JsonArrayData(buf)) {}
  template <typename T>
  bool add(const T& value) {
    return add_impl<const T&>(value);
  }
  template <typename T>
  bool add(T* value) {
    return add_impl<T*>(value);
  }
  iterator begin() {
    if (!_data) return iterator();
    return _data->begin();
  }
  const_iterator begin() const {
    if (!_data) return const_iterator();
    return _data->begin();
  }
  iterator end() {
    return iterator();
  }
  const_iterator end() const {
    return const_iterator();
  }
  template <typename T, size_t N>
  bool copyFrom(T (&array)[N]) {
    return copyFrom(array, N);
  }
  template <typename T>
  bool copyFrom(T* array, size_t len) {
    bool ok = true;
    for (size_t i = 0; i < len; i++) {
      ok &= add(array[i]);
    }
    return ok;
  }
  template <typename T, size_t N1, size_t N2>
  bool copyFrom(T (&array)[N1][N2]) {
    bool ok = true;
    for (size_t i = 0; i < N1; i++) {
      JsonArray nestedArray = createNestedArray();
      for (size_t j = 0; j < N2; j++) {
        ok &= nestedArray.add(array[i][j]);
      }
    }
    return ok;
  }
  template <typename T, size_t N>
  size_t copyTo(T (&array)[N]) const {
    return copyTo(array, N);
  }
  template <typename T>
  size_t copyTo(T* array, size_t len) const {
    size_t i = 0;
    for (const_iterator it = begin(); it != end() && i < len; ++it)
      array[i++] = *it;
    return i;
  }
  template <typename T, size_t N1, size_t N2>
  void copyTo(T (&array)[N1][N2]) const {
    if (!_data) return;
    size_t i = 0;
    for (const_iterator it = begin(); it != end() && i < N1; ++it) {
      it->as<JsonArray>().copyTo(array[i++]);
    }
  }
  JsonArray createNestedArray();
  JsonObject createNestedObject();
  Internals::JsonArraySubscript operator[](size_t index);
  const Internals::JsonArraySubscript operator[](size_t index) const;
  bool operator==(const JsonArray& rhs) const {
    return _data == rhs._data;
  }
  template <typename T>
  typename Internals::JsonVariantAs<T>::type get(size_t index) const {
    const_iterator it = begin() += index;
    return it != end() ? it->as<T>() : Internals::JsonVariantDefault<T>::get();
  }
  template <typename T>
  bool is(size_t index) const {
    const_iterator it = begin() += index;
    return it != end() ? it->is<T>() : false;
  }
  void remove(iterator it) {
    if (!_data) return;
    _data->remove(it);
  }
  void remove(size_t index) {
    remove(begin() += index);
  }
  template <typename T>
  bool set(size_t index, const T& value) {
    if (!_data) return false;
    return set_impl<const T&>(index, value);
  }
  template <typename T>
  bool set(size_t index, T* value) {
    if (!_data) return false;
    return set_impl<T*>(index, value);
  }
  size_t size() const {
    if (!_data) return 0;
    return _data->size();
  }
  bool isNull() const {
    return _data == 0;
  }
  template <typename Visitor>
  void visit(Visitor& visitor) const {
    if (_data)
      return visitor.acceptArray(*this);
    else
      visitor.acceptNull();
  }
 private:
  template <typename TValueRef>
  bool set_impl(size_t index, TValueRef value) {
    iterator it = begin() += index;
    if (it == end()) return false;
    return Internals::ValueSaver<TValueRef>::save(_data->_buffer, *it, value);
  }
  template <typename TValueRef>
  bool add_impl(TValueRef value) {
    if (!_data) return false;
    iterator it = _data->add();
    if (it == end()) return false;
    return Internals::ValueSaver<TValueRef>::save(_data->_buffer, *it, value);
  }
  Internals::JsonArrayData* _data;
};
}  // namespace ArduinoJson
namespace ArduinoJson {
struct JsonPair {
  const char* key;
  JsonVariant value;
};
}  // namespace ArduinoJson
#define JSON_OBJECT_SIZE(NUMBER_OF_ELEMENTS)        \
  (sizeof(ArduinoJson::Internals::JsonObjectData) + \
   (NUMBER_OF_ELEMENTS) *                           \
       sizeof(ArduinoJson::Internals::JsonObjectData::node_type))
namespace ArduinoJson {
namespace Internals {
struct JsonObjectData : List<JsonPair>, JsonBufferAllocated {
  explicit JsonObjectData(JsonBuffer* buf) throw() : List<JsonPair>(buf) {}
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
class JsonObject {
  friend class JsonVariant;
 public:
  typedef Internals::JsonObjectData::iterator iterator;
  typedef Internals::JsonObjectData::const_iterator const_iterator;
  JsonObject() : _data(0) {}
  JsonObject(Internals::JsonObjectData* object) : _data(object) {}
  JsonObject(Internals::JsonBuffer* buf)
      : _data(new (buf) Internals::JsonObjectData(buf)) {}
  iterator begin() {
    if (!_data) return iterator();
    return _data->begin();
  }
  const_iterator begin() const {
    if (!_data) return const_iterator();
    return _data->begin();
  }
  template <typename TString>
  bool containsKey(const TString& key) const {
    return containsKey_impl<const TString&>(key);
  }
  template <typename TString>
  bool containsKey(TString* key) const {
    return containsKey_impl<TString*>(key);
  }
  iterator end() {
    return iterator();
  }
  const_iterator end() const {
    return const_iterator();
  }
  template <typename TString>
  JsonArray createNestedArray(const TString& key);
  template <typename TString>
  JsonArray createNestedArray(TString* key);
  template <typename TString>
  JsonObject createNestedObject(const TString& key) {
    if (!_data) return JsonObject();
    return createNestedObject_impl<const TString&>(key);
  }
  template <typename TString>
  JsonObject createNestedObject(TString* key) {
    return createNestedObject_impl<TString*>(key);
  }
  template <typename TValue, typename TString>
  typename Internals::JsonVariantAs<TValue>::type get(
      const TString& key) const {
    return get_impl<const TString&, TValue>(key);
  }
  template <typename TValue, typename TString>
  typename Internals::JsonVariantAs<TValue>::type get(TString* key) const {
    return get_impl<TString*, TValue>(key);
  }
  template <typename TValue, typename TString>
  bool is(const TString& key) const {
    return is_impl<const TString&, TValue>(key);
  }
  template <typename TValue, typename TString>
  bool is(TString* key) const {
    return is_impl<TString*, TValue>(key);
  }
  template <typename TString>
  Internals::JsonObjectSubscript<const TString&> operator[](
      const TString& key) {
    return Internals::JsonObjectSubscript<const TString&>(*this, key);
  }
  template <typename TString>
  Internals::JsonObjectSubscript<TString*> operator[](TString* key) {
    return Internals::JsonObjectSubscript<TString*>(*this, key);
  }
  template <typename TString>
  const Internals::JsonObjectSubscript<const TString&> operator[](
      const TString& key) const {
    return Internals::JsonObjectSubscript<const TString&>(*this, key);
  }
  template <typename TString>
  const Internals::JsonObjectSubscript<TString*> operator[](
      TString* key) const {
    return Internals::JsonObjectSubscript<TString*>(*this, key);
  }
  bool operator==(const JsonObject& rhs) const {
    return _data == rhs._data;
  }
  void remove(iterator it) {
    if (!_data) return;
    _data->remove(it);
  }
  template <typename TString>
  void remove(const TString& key) {
    remove_impl<const TString&>(key);
  }
  template <typename TString>
  void remove(TString* key) {
    remove_impl<TString*>(key);
  }
  template <typename TValue, typename TString>
  bool set(const TString& key, const TValue& value) {
    return set_impl<const TString&, const TValue&>(key, value);
  }
  template <typename TValue, typename TString>
  bool set(const TString& key, TValue* value) {
    return set_impl<const TString&, TValue*>(key, value);
  }
  template <typename TValue, typename TString>
  bool set(TString* key, const TValue& value) {
    return set_impl<TString*, const TValue&>(key, value);
  }
  template <typename TValue, typename TString>
  bool set(TString* key, TValue* value) {
    return set_impl<TString*, TValue*>(key, value);
  }
  size_t size() const {
    if (!_data) return 0;
    return _data->size();
  }
  bool isNull() const {
    return _data == 0;
  }
  template <typename Visitor>
  void visit(Visitor& visitor) const {
    if (_data)
      visitor.acceptObject(*this);
    else
      return visitor.acceptNull();
  }
 private:
  template <typename TStringRef>
  bool containsKey_impl(TStringRef key) const {
    return findKey<TStringRef>(key) != end();
  }
  template <typename TStringRef>
  JsonArray createNestedArray_impl(TStringRef key);
  template <typename TStringRef>
  JsonObject createNestedObject_impl(TStringRef key);
  template <typename TStringRef>
  iterator findKey(TStringRef key) {
    iterator it;
    for (it = begin(); it != end(); ++it) {
      if (Internals::makeString(key).equals(it->key)) break;
    }
    return it;
  }
  template <typename TStringRef>
  const_iterator findKey(TStringRef key) const {
    return const_cast<JsonObject*>(this)->findKey<TStringRef>(key);
  }
  template <typename TStringRef, typename TValue>
  typename Internals::JsonVariantAs<TValue>::type get_impl(
      TStringRef key) const {
    const_iterator it = findKey<TStringRef>(key);
    return it != end() ? it->value.as<TValue>()
                       : Internals::JsonVariantDefault<TValue>::get();
  }
  template <typename TStringRef, typename TValue>
  bool is_impl(TStringRef key) const {
    const_iterator it = findKey<TStringRef>(key);
    return it != end() ? it->value.is<TValue>() : false;
  }
  template <typename TStringRef>
  void remove_impl(TStringRef key) {
    if (!_data) return;
    _data->remove(findKey<TStringRef>(key));
  }
  template <typename TStringRef, typename TValueRef>
  bool set_impl(TStringRef key, TValueRef value) {
    if (!_data) return false;
    if (Internals::makeString(key).is_null()) return false;
    iterator it = findKey<TStringRef>(key);
    if (it == end()) {
      it = _data->add();
      if (it == end()) return false;
      bool key_ok =
          Internals::ValueSaver<TStringRef>::save(_data->_buffer, it->key, key);
      if (!key_ok) return false;
    }
    return Internals::ValueSaver<TValueRef>::save(_data->_buffer, it->value,
                                                  value);
  }
  Internals::JsonObjectData* _data;
};
}  // namespace ArduinoJson
#include <stdlib.h>
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
namespace ArduinoJson {
namespace Internals {
class DefaultAllocator {
 public:
  void* allocate(size_t size) {
    return malloc(size);
  }
  void deallocate(void* pointer) {
    free(pointer);
  }
};
template <typename TAllocator>
class DynamicJsonBufferBase : public JsonBuffer {
  struct Block;
  struct EmptyBlock {
    Block* next;
    size_t capacity;
    size_t size;
  };
  struct Block : EmptyBlock {
    uint8_t data[1];
  };
 public:
  enum { EmptyBlockSize = sizeof(EmptyBlock) };
  DynamicJsonBufferBase(size_t initialSize = 256)
      : _head(NULL), _nextBlockCapacity(initialSize) {}
  ~DynamicJsonBufferBase() {
    clear();
  }
  size_t size() const {
    size_t total = 0;
    for (const Block* b = _head; b; b = b->next) total += b->size;
    return total;
  }
  virtual void* alloc(size_t bytes) {
    alignNextAlloc();
    return canAllocInHead(bytes) ? allocInHead(bytes) : allocInNewBlock(bytes);
  }
  void clear() {
    Block* currentBlock = _head;
    while (currentBlock != NULL) {
      _nextBlockCapacity = currentBlock->capacity;
      Block* nextBlock = currentBlock->next;
      _allocator.deallocate(currentBlock);
      currentBlock = nextBlock;
    }
    _head = 0;
  }
  class String {
   public:
    String(DynamicJsonBufferBase* parent)
        : _parent(parent), _start(NULL), _length(0) {}
    void append(char c) {
      if (_parent->canAllocInHead(1)) {
        char* end = static_cast<char*>(_parent->allocInHead(1));
        *end = c;
        if (_length == 0) _start = end;
      } else {
        char* newStart =
            static_cast<char*>(_parent->allocInNewBlock(_length + 1));
        if (_start && newStart) memcpy(newStart, _start, _length);
        if (newStart) newStart[_length] = c;
        _start = newStart;
      }
      _length++;
    }
    const char* c_str() {
      append(0);
      return _start;
    }
   private:
    DynamicJsonBufferBase* _parent;
    char* _start;
    size_t _length;
  };
  String startString() {
    return String(this);
  }
 private:
  void alignNextAlloc() {
    if (_head) _head->size = this->round_size_up(_head->size);
  }
  bool canAllocInHead(size_t bytes) const {
    return _head != NULL && _head->size + bytes <= _head->capacity;
  }
  void* allocInHead(size_t bytes) {
    void* p = _head->data + _head->size;
    _head->size += bytes;
    return p;
  }
  void* allocInNewBlock(size_t bytes) {
    size_t capacity = _nextBlockCapacity;
    if (bytes > capacity) capacity = bytes;
    if (!addNewBlock(capacity)) return NULL;
    _nextBlockCapacity *= 2;
    return allocInHead(bytes);
  }
  bool addNewBlock(size_t capacity) {
    size_t bytes = EmptyBlockSize + capacity;
    Block* block = static_cast<Block*>(_allocator.allocate(bytes));
    if (block == NULL) return false;
    block->capacity = capacity;
    block->size = 0;
    block->next = _head;
    _head = block;
    return true;
  }
  TAllocator _allocator;
  Block* _head;
  size_t _nextBlockCapacity;
};
typedef Internals::DynamicJsonBufferBase<Internals::DefaultAllocator>
    DynamicJsonBuffer;
}  // namespace Internals
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
}  // namespace ArduinoJson
namespace ArduinoJson {
class DynamicJsonDocument {
  Internals::DynamicJsonBuffer _buffer;
  JsonVariant _root;
 public:
  uint8_t nestingLimit;
  DynamicJsonDocument() : nestingLimit(ARDUINOJSON_DEFAULT_NESTING_LIMIT) {}
  DynamicJsonDocument(size_t capacity)
      : _buffer(capacity), nestingLimit(ARDUINOJSON_DEFAULT_NESTING_LIMIT) {}
  template <typename T>
  bool is() const {
    return _root.is<T>();
  }
  template <typename T>
  typename Internals::JsonVariantAs<T>::type as() const {
    return _root.as<T>();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonObject>::value,
                                JsonObject>::type
  to() {
    clear();
    JsonObject object(&_buffer);
    _root = object;
    return object;
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonArray>::value,
                                JsonArray>::type
  to() {
    clear();
    JsonArray array(&_buffer);
    _root = array;
    return array;
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonVariant>::value,
                                T&>::type
  to() {
    clear();
    return _root;
  }
  Internals::DynamicJsonBuffer& buffer() {
    return _buffer;
  }
  void clear() {
    _buffer.clear();
    _root = JsonVariant();
  }
  size_t memoryUsage() const {
    return _buffer.size();
  }
  template <typename Visitor>
  void visit(Visitor& visitor) const {
    return _root.visit(visitor);
  }
};
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <size_t X, size_t Y, bool MaxIsX = (X > Y)>
struct Max {};
template <size_t X, size_t Y>
struct Max<X, Y, true> {
  static const size_t value = X;
};
template <size_t X, size_t Y>
struct Max<X, Y, false> {
  static const size_t value = Y;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class StaticJsonBufferBase : public JsonBuffer {
 public:
  class String {
   public:
    String(StaticJsonBufferBase* parent) : _parent(parent) {
      _start = parent->_buffer + parent->_size;
    }
    void append(char c) {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = c;
      }
    }
    const char* c_str() const {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = '\0';
        return _start;
      } else {
        return NULL;
      }
    }
   private:
    StaticJsonBufferBase* _parent;
    char* _start;
  };
  StaticJsonBufferBase(char* buffer, size_t capa)
      : _buffer(buffer), _capacity(capa), _size(0) {}
  size_t capacity() const {
    return _capacity;
  }
  size_t size() const {
    return _size;
  }
  virtual void* alloc(size_t bytes) {
    alignNextAlloc();
    if (!canAlloc(bytes)) return NULL;
    return doAlloc(bytes);
  }
  void clear() {
    _size = 0;
  }
  String startString() {
    return String(this);
  }
 protected:
  ~StaticJsonBufferBase() {}
 private:
  void alignNextAlloc() {
    _size = round_size_up(_size);
  }
  bool canAlloc(size_t bytes) const {
    return _size + bytes <= _capacity;
  }
  void* doAlloc(size_t bytes) {
    void* p = &_buffer[_size];
    _size += bytes;
    return p;
  }
  char* _buffer;
  size_t _capacity;
  size_t _size;
};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
template <size_t CAPACITY>
class StaticJsonBuffer : public Internals::StaticJsonBufferBase {
  static const size_t ACTUAL_CAPACITY = Internals::Max<1, CAPACITY>::value;
 public:
  explicit StaticJsonBuffer()
      : Internals::StaticJsonBufferBase(_buffer, ACTUAL_CAPACITY) {}
 private:
  char _buffer[ACTUAL_CAPACITY];
};
}  // namespace Internals
}  // namespace ArduinoJson
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
namespace ArduinoJson {
template <size_t CAPACITY = sizeof(JsonVariant)>
class StaticJsonDocument {
  Internals::StaticJsonBuffer<CAPACITY> _buffer;
  JsonVariant _root;
 public:
  uint8_t nestingLimit;
  StaticJsonDocument() : nestingLimit(ARDUINOJSON_DEFAULT_NESTING_LIMIT) {}
  Internals::StaticJsonBufferBase& buffer() {
    return _buffer;
  }
  template <typename T>
  bool is() const {
    return _root.is<T>();
  }
  template <typename T>
  typename Internals::JsonVariantAs<T>::type as() const {
    return _root.as<T>();
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonObject>::value,
                                JsonObject>::type
  to() {
    clear();
    JsonObject object(&_buffer);
    _root = object;
    return object;
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonArray>::value,
                                JsonArray>::type
  to() {
    clear();
    JsonArray array(&_buffer);
    _root = array;
    return array;
  }
  template <typename T>
  typename Internals::enable_if<Internals::is_same<T, JsonVariant>::value,
                                T&>::type
  to() {
    clear();
    return _root;
  }
  void clear() {
    _buffer.clear();
    _root = JsonVariant();
  }
  size_t memoryUsage() const {
    return _buffer.size();
  }
  template <typename Visitor>
  void visit(Visitor& visitor) const {
    return _root.visit(visitor);
  }
};
}  // namespace ArduinoJson
namespace ArduinoJson {
template <typename TString>
inline JsonArray JsonObject::createNestedArray(const TString& key) {
  return createNestedArray_impl<const TString&>(key);
}
template <typename TString>
inline JsonArray JsonObject::createNestedArray(TString* key) {
  return createNestedArray_impl<TString*>(key);
}
template <typename TStringRef>
inline JsonArray JsonObject::createNestedArray_impl(TStringRef key) {
  if (!_data) return JsonArray();
  JsonArray array(_data->_buffer);
  if (!array.isNull()) set(key, array);
  return array;
}
template <typename TStringRef>
inline JsonObject JsonObject::createNestedObject_impl(TStringRef key) {
  if (!_data) return JsonObject();
  JsonObject object(_data->_buffer);
  if (!object.isNull()) set(key, object);
  return object;
}
}  // namespace ArduinoJson
namespace ArduinoJson {
inline JsonArray JsonArray::createNestedArray() {
  if (!_data) return JsonArray();
  JsonArray array(_data->_buffer);
  if (!array.isNull()) add(array);
  return array;
}
inline JsonObject JsonArray::createNestedObject() {
  if (!_data) return JsonObject();
  JsonObject object(_data->_buffer);
  if (!object.isNull()) add(object);
  return object;
}
}  // namespace ArduinoJson
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif
namespace ArduinoJson {
namespace Internals {
class JsonArraySubscript : public JsonVariantBase<JsonArraySubscript> {
 public:
  FORCE_INLINE JsonArraySubscript(JsonArray array, size_t index)
      : _array(array), _index(index) {}
  FORCE_INLINE JsonArraySubscript& operator=(const JsonArraySubscript& src) {
    _array.set(_index, src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE JsonArraySubscript& operator=(const T& src) {
    _array.set(_index, src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE JsonArraySubscript& operator=(T* src) {
    _array.set(_index, src);
    return *this;
  }
  FORCE_INLINE bool isNull() const {
    return _index >= _array.size();
  }
  template <typename T>
  FORCE_INLINE typename JsonVariantAs<T>::type as() const {
    return _array.get<T>(_index);
  }
  template <typename T>
  FORCE_INLINE bool is() const {
    return _array.is<T>(_index);
  }
  template <typename TValue>
  FORCE_INLINE bool set(const TValue& value) {
    return _array.set(_index, value);
  }
  template <typename TValue>
  FORCE_INLINE bool set(TValue* value) {
    return _array.set(_index, value);
  }
  template <typename Visitor>
  void visit(Visitor& visitor) const {
    return _array.get<JsonVariant>(_index).visit(visitor);
  }
 private:
  JsonArray _array;
  const size_t _index;
};
template <typename TImpl>
inline JsonArraySubscript JsonVariantSubscripts<TImpl>::operator[](
    size_t index) {
  return impl()->template as<JsonArray>()[index];
}
template <typename TImpl>
inline const JsonArraySubscript JsonVariantSubscripts<TImpl>::operator[](
    size_t index) const {
  return impl()->template as<JsonArray>()[index];
}
}  // namespace Internals
inline Internals::JsonArraySubscript JsonArray::operator[](size_t index) {
  return Internals::JsonArraySubscript(*this, index);
}
inline const Internals::JsonArraySubscript JsonArray::operator[](
    size_t index) const {
  return Internals::JsonArraySubscript(*this, index);
}
}  // namespace ArduinoJson
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TStringRef>
class JsonObjectSubscript
    : public JsonVariantBase<JsonObjectSubscript<TStringRef> > {
  typedef JsonObjectSubscript<TStringRef> this_type;
 public:
  FORCE_INLINE JsonObjectSubscript(JsonObject object, TStringRef key)
      : _object(object), _key(key) {}
  FORCE_INLINE this_type &operator=(const this_type &src) {
    _object.set(_key, src);
    return *this;
  }
  template <typename TValue>
  FORCE_INLINE typename enable_if<!is_array<TValue>::value, this_type &>::type
  operator=(const TValue &src) {
    _object.set(_key, src);
    return *this;
  }
  template <typename TValue>
  FORCE_INLINE this_type &operator=(TValue *src) {
    _object.set(_key, src);
    return *this;
  }
  FORCE_INLINE bool isNull() const {
    return !_object.containsKey(_key);
  }
  template <typename TValue>
  FORCE_INLINE typename JsonVariantAs<TValue>::type as() const {
    return _object.get<TValue>(_key);
  }
  template <typename TValue>
  FORCE_INLINE bool is() const {
    return _object.is<TValue>(_key);
  }
  template <typename TValue>
  FORCE_INLINE typename enable_if<!is_array<TValue>::value, bool>::type set(
      const TValue &value) {
    return _object.set(_key, value);
  }
  template <typename TValue>
  FORCE_INLINE bool set(const TValue *value) {
    return _object.set(_key, value);
  }
  template <typename Visitor>
  void visit(Visitor &visitor) const {
    return _object.get<JsonVariant>(_key).visit(visitor);
  }
 private:
  JsonObject _object;
  TStringRef _key;
};
template <typename TImpl>
template <typename TString>
inline typename enable_if<IsString<TString>::value,
                          const JsonObjectSubscript<const TString &> >::type
    JsonVariantSubscripts<TImpl>::operator[](const TString &key) const {
  return impl()->template as<JsonObject>()[key];
}
template <typename TImpl>
template <typename TString>
inline typename enable_if<IsString<TString>::value,
                          JsonObjectSubscript<const TString &> >::type
    JsonVariantSubscripts<TImpl>::operator[](const TString &key) {
  return impl()->template as<JsonObject>()[key];
}
template <typename TImpl>
template <typename TString>
inline typename enable_if<IsString<TString *>::value,
                          JsonObjectSubscript<TString *> >::type
    JsonVariantSubscripts<TImpl>::operator[](TString *key) {
  return impl()->template as<JsonObject>()[key];
}
template <typename TImpl>
template <typename TString>
inline typename enable_if<IsString<TString *>::value,
                          const JsonObjectSubscript<TString *> >::type
    JsonVariantSubscripts<TImpl>::operator[](TString *key) const {
  return impl()->template as<JsonObject>()[key];
}
}  // namespace Internals
}  // namespace ArduinoJson
#ifdef _MSC_VER
#pragma warning(pop)
#endif
namespace ArduinoJson {
namespace Internals {
inline bool isdigit(char c) {
  return '0' <= c && c <= '9';
}
inline bool issign(char c) {
  return '-' == c || c == '+';
}
}
}
namespace ArduinoJson {
namespace Internals {
inline bool isFloat(const char* s) {
  if (!s) return false;
  if (!strcmp(s, "NaN")) return true;
  if (issign(*s)) s++;
  if (!strcmp(s, "Infinity")) return true;
  if (*s == '\0') return false;
  while (isdigit(*s)) s++;
  if (*s == '.') {
    s++;
    while (isdigit(*s)) s++;
  }
  if (*s == 'e' || *s == 'E') {
    s++;
    if (issign(*s)) s++;
    if (!isdigit(*s)) return false;
    while (isdigit(*s)) s++;
  }
  return *s == '\0';
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
inline bool isInteger(const char* s) {
  if (!s || !*s) return false;
  if (issign(*s)) s++;
  while (isdigit(*s)) s++;
  return *s == '\0';
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
#ifndef isnan
template <typename T>
bool isnan(T x) {
  return x != x;
}
#endif
#ifndef isinf
template <typename T>
bool isinf(T x) {
  return x != 0.0 && x * 2 == x;
}
#endif
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T, size_t = sizeof(T)>
struct FloatTraits {};
template <typename T>
struct FloatTraits<T, 8 /*64bits*/> {
  typedef int64_t mantissa_type;
  static const short mantissa_bits = 52;
  static const mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;
  typedef int16_t exponent_type;
  static const exponent_type exponent_max = 308;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1) m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = TExponent(-e);
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1) m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }
  static T positiveBinaryPowerOfTen(int index) {
    static T factors[] = {
        1e1,
        1e2,
        1e4,
        1e8,
        1e16,
        forge(0x4693B8B5, 0xB5056E17),  // 1e32
        forge(0x4D384F03, 0xE93FF9F5),  // 1e64
        forge(0x5A827748, 0xF9301D32),  // 1e128
        forge(0x75154FDD, 0x7F73BF3C)   // 1e256
    };
    return factors[index];
  }
  static T negativeBinaryPowerOfTen(int index) {
    static T factors[] = {
        forge(0x3FB99999, 0x9999999A),  // 1e-1
        forge(0x3F847AE1, 0x47AE147B),  // 1e-2
        forge(0x3F1A36E2, 0xEB1C432D),  // 1e-4
        forge(0x3E45798E, 0xE2308C3A),  // 1e-8
        forge(0x3C9CD2B2, 0x97D889BC),  // 1e-16
        forge(0x3949F623, 0xD5A8A733),  // 1e-32
        forge(0x32A50FFD, 0x44F4A73D),  // 1e-64
        forge(0x255BBA08, 0xCF8C979D),  // 1e-128
        forge(0x0AC80628, 0x64AC6F43)   // 1e-256
    };
    return factors[index];
  }
  static T negativeBinaryPowerOfTenPlusOne(int index) {
    static T factors[] = {
        1e0,
        forge(0x3FB99999, 0x9999999A),  // 1e-1
        forge(0x3F50624D, 0xD2F1A9FC),  // 1e-3
        forge(0x3E7AD7F2, 0x9ABCAF48),  // 1e-7
        forge(0x3CD203AF, 0x9EE75616),  // 1e-15
        forge(0x398039D6, 0x65896880),  // 1e-31
        forge(0x32DA53FC, 0x9631D10D),  // 1e-63
        forge(0x25915445, 0x81B7DEC2),  // 1e-127
        forge(0x0AFE07B2, 0x7DD78B14)   // 1e-255
    };
    return factors[index];
  }
  static T nan() {
    return forge(0x7ff80000, 0x00000000);
  }
  static T inf() {
    return forge(0x7ff00000, 0x00000000);
  }
  static T forge(uint32_t msb, uint32_t lsb) {
    union {
      uint64_t integerBits;
      T floatBits;
    };
    integerBits = (uint64_t(msb) << 32) | lsb;
    return floatBits;
  }
};
template <typename T>
struct FloatTraits<T, 4 /*32bits*/> {
  typedef int32_t mantissa_type;
  static const short mantissa_bits = 23;
  static const mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;
  typedef int8_t exponent_type;
  static const exponent_type exponent_max = 38;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1) m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = -e;
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1) m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }
  static T positiveBinaryPowerOfTen(int index) {
    static T factors[] = {1e1f, 1e2f, 1e4f, 1e8f, 1e16f, 1e32f};
    return factors[index];
  }
  static T negativeBinaryPowerOfTen(int index) {
    static T factors[] = {1e-1f, 1e-2f, 1e-4f, 1e-8f, 1e-16f, 1e-32f};
    return factors[index];
  }
  static T negativeBinaryPowerOfTenPlusOne(int index) {
    static T factors[] = {1e0f, 1e-1f, 1e-3f, 1e-7f, 1e-15f, 1e-31f};
    return factors[index];
  }
  static T forge(uint32_t bits) {
    union {
      uint32_t integerBits;
      T floatBits;
    };
    integerBits = bits;
    return floatBits;
  }
  static T nan() {
    return forge(0x7fc00000);
  }
  static T inf() {
    return forge(0x7f800000);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
inline T parseFloat(const char* s) {
  typedef FloatTraits<T> traits;
  typedef typename traits::mantissa_type mantissa_t;
  typedef typename traits::exponent_type exponent_t;
  if (!s) return 0;  // NULL
  bool negative_result = false;
  switch (*s) {
    case '-':
      negative_result = true;
      s++;
      break;
    case '+':
      s++;
      break;
  }
  if (*s == 't') return 1;  // true
  if (*s == 'n' || *s == 'N') return traits::nan();
  if (*s == 'i' || *s == 'I')
    return negative_result ? -traits::inf() : traits::inf();
  mantissa_t mantissa = 0;
  exponent_t exponent_offset = 0;
  while (isdigit(*s)) {
    if (mantissa < traits::mantissa_max / 10)
      mantissa = mantissa * 10 + (*s - '0');
    else
      exponent_offset++;
    s++;
  }
  if (*s == '.') {
    s++;
    while (isdigit(*s)) {
      if (mantissa < traits::mantissa_max / 10) {
        mantissa = mantissa * 10 + (*s - '0');
        exponent_offset--;
      }
      s++;
    }
  }
  int exponent = 0;
  if (*s == 'e' || *s == 'E') {
    s++;
    bool negative_exponent = false;
    if (*s == '-') {
      negative_exponent = true;
      s++;
    } else if (*s == '+') {
      s++;
    }
    while (isdigit(*s)) {
      exponent = exponent * 10 + (*s - '0');
      if (exponent + exponent_offset > traits::exponent_max) {
        if (negative_exponent)
          return negative_result ? -0.0f : 0.0f;
        else
          return negative_result ? -traits::inf() : traits::inf();
      }
      s++;
    }
    if (negative_exponent) exponent = -exponent;
  }
  exponent += exponent_offset;
  T result = traits::make_float(static_cast<T>(mantissa), exponent);
  return negative_result ? -result : result;
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
T parseInteger(const char *s) {
  if (!s) return 0;  // NULL
  if (*s == 't') return 1;  // "true"
  T result = 0;
  bool negative_result = false;
  switch (*s) {
    case '-':
      negative_result = true;
      s++;
      break;
    case '+':
      s++;
      break;
  }
  while (isdigit(*s)) {
    result = T(result * 10 + T(*s - '0'));
    s++;
  }
  return negative_result ? T(~result + 1) : result;
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
inline JsonVariant::JsonVariant(JsonArray array) {
  if (!array.isNull()) {
    _type = Internals::JSON_ARRAY;
    _content.asArray = array._data;
  } else {
    _type = Internals::JSON_UNDEFINED;
  }
}
inline JsonVariant::JsonVariant(JsonObject object) {
  if (!object.isNull()) {
    _type = Internals::JSON_OBJECT;
    _content.asObject = object._data;
  } else {
    _type = Internals::JSON_UNDEFINED;
  }
}
template <typename T>
inline typename Internals::enable_if<
    Internals::is_same<typename Internals::remove_const<T>::type,
                       JsonArray>::value,
    JsonArray>::type
JsonVariant::as() const {
  return variantAsArray();
}
template <typename T>
inline typename Internals::enable_if<
    Internals::is_same<typename Internals::remove_const<T>::type,
                       JsonObject>::value,
    T>::type
JsonVariant::as() const {
  return variantAsObject();
}
inline JsonArray JsonVariant::variantAsArray() const {
  if (_type == Internals::JSON_ARRAY) return _content.asArray;
  return JsonArray();
}
inline JsonObject JsonVariant::variantAsObject() const {
  if (_type == Internals::JSON_OBJECT) return _content.asObject;
  return JsonObject();
}
template <typename T>
inline T JsonVariant::variantAsInteger() const {
  using namespace Internals;
  switch (_type) {
    case JSON_UNDEFINED:
    case JSON_UNPARSED:
      return 0;
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return T(_content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return T(~_content.asInteger + 1);
    case JSON_STRING:
      return parseInteger<T>(_content.asString);
    default:
      return T(_content.asFloat);
  }
}
inline const char *JsonVariant::variantAsString() const {
  using namespace Internals;
  return _type == JSON_STRING ? _content.asString : NULL;
}
template <typename T>
inline T JsonVariant::variantAsFloat() const {
  using namespace Internals;
  switch (_type) {
    case JSON_UNDEFINED:
    case JSON_UNPARSED:
      return 0;
    case JSON_POSITIVE_INTEGER:
    case JSON_BOOLEAN:
      return static_cast<T>(_content.asInteger);
    case JSON_NEGATIVE_INTEGER:
      return -static_cast<T>(_content.asInteger);
    case JSON_STRING:
      return parseFloat<T>(_content.asString);
    default:
      return static_cast<T>(_content.asFloat);
  }
}
inline bool JsonVariant::variantIsBoolean() const {
  using namespace Internals;
  return _type == JSON_BOOLEAN;
}
inline bool JsonVariant::variantIsInteger() const {
  using namespace Internals;
  return _type == JSON_POSITIVE_INTEGER || _type == JSON_NEGATIVE_INTEGER;
}
inline bool JsonVariant::variantIsFloat() const {
  using namespace Internals;
  return _type == JSON_FLOAT || _type == JSON_POSITIVE_INTEGER ||
         _type == JSON_NEGATIVE_INTEGER;
}
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TJsonBuffer>
class StringCopier {
 public:
  StringCopier(TJsonBuffer& jb) : _jb(&jb) {}
  typedef typename TJsonBuffer::String String;
  String startString() {
    return _jb->startString();
  }
 private:
  TJsonBuffer* _jb;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TChar>
class StringMover {
 public:
  class String {
   public:
    String(TChar** ptr) : _writePtr(ptr), _startPtr(*ptr) {}
    void append(char c) {
      *(*_writePtr)++ = TChar(c);
    }
    const char* c_str() const {
      *(*_writePtr)++ = 0;
      return reinterpret_cast<const char*>(_startPtr);
    }
   private:
    TChar** _writePtr;
    TChar* _startPtr;
  };
  StringMover(TChar* buffer) : _ptr(buffer) {}
  String startString() {
    return String(&_ptr);
  }
 private:
  TChar* _ptr;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TJsonBuffer, typename TInput, typename Enable = void>
struct StringStorage {
  typedef StringCopier<TJsonBuffer> type;
  static type create(TJsonBuffer& jb, TInput&) {
    return type(jb);
  }
};
template <typename TJsonBuffer, typename TChar>
struct StringStorage<TJsonBuffer, TChar*,
                     typename enable_if<!is_const<TChar>::value>::type> {
  typedef StringMover<TChar> type;
  static type create(TJsonBuffer&, TChar* input) {
    return type(input);
  }
};
template <typename TJsonBuffer, typename TInput>
typename StringStorage<TJsonBuffer, TInput>::type makeStringStorage(
    TJsonBuffer& jb, TInput& input) {
  return StringStorage<TJsonBuffer, TInput>::create(jb, input);
}
template <typename TJsonBuffer, typename TChar>
typename StringStorage<TJsonBuffer, TChar*>::type makeStringStorage(
    TJsonBuffer& jb, TChar* input) {
  return StringStorage<TJsonBuffer, TChar*>::create(jb, input);
}
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_ARDUINO_STREAM
#include <Stream.h>
namespace ArduinoJson {
namespace Internals {
struct ArduinoStreamReader {
  Stream& _stream;
  char _current;
  bool _ended;
 public:
  explicit ArduinoStreamReader(Stream& stream)
      : _stream(stream), _current(0), _ended(false) {}
  char read() {
    char c = 0;
    _ended = _stream.readBytes(&c, 1) == 0;
    return c;
  }
  bool ended() const {
    return _ended;
  }
};
inline ArduinoStreamReader makeReader(Stream& input) {
  return ArduinoStreamReader(input);
}
}  // namespace Internals
}  // namespace ArduinoJson
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TChar>
class UnsafeCharPointerReader {
  const TChar* _ptr;
 public:
  explicit UnsafeCharPointerReader(const TChar* ptr)
      : _ptr(ptr ? ptr : reinterpret_cast<const TChar*>("")) {}
  char read() {
    return static_cast<char>(*_ptr++);
  }
  bool ended() const {
    return false;
  }
};
template <typename TChar>
class SafeCharPointerReader {
  const TChar* _ptr;
  const TChar* _end;
 public:
  explicit SafeCharPointerReader(const TChar* ptr, size_t len)
      : _ptr(ptr ? ptr : reinterpret_cast<const TChar*>("")),
        _end(_ptr + len) {}
  char read() {
    return static_cast<char>(*_ptr++);
  }
  bool ended() const {
    return _ptr == _end;
  }
};
template <typename TChar>
inline UnsafeCharPointerReader<TChar> makeReader(TChar* input) {
  return UnsafeCharPointerReader<TChar>(input);
}
template <typename TChar>
inline SafeCharPointerReader<TChar> makeReader(TChar* input, size_t n) {
  return SafeCharPointerReader<TChar>(input, n);
}
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
inline SafeCharPointerReader<char> makeReader(const String& input) {
  return SafeCharPointerReader<char>(input.c_str(), input.length());
}
#endif
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <ostream>
#endif
namespace ArduinoJson {
class DeserializationError {
 public:
  enum Code {
    Ok,
    IncompleteInput,
    InvalidInput,
    NoMemory,
    NotSupported,
    TooDeep
  };
  DeserializationError() {}
  DeserializationError(Code code) : _code(code) {}
  friend bool operator==(const DeserializationError& err, Code code) {
    return err._code == code;
  }
  friend bool operator==(Code code, const DeserializationError& err) {
    return err._code == code;
  }
  friend bool operator!=(const DeserializationError& err, Code code) {
    return err._code != code;
  }
  friend bool operator!=(Code code, const DeserializationError& err) {
    return err._code != code;
  }
  operator bool() const {
    return _code != Ok;
  }
  const char* c_str() const {
    switch (_code) {
      case Ok:
        return "Ok";
      case TooDeep:
        return "TooDeep";
      case NoMemory:
        return "NoMemory";
      case InvalidInput:
        return "InvalidInput";
      case IncompleteInput:
        return "IncompleteInput";
      case NotSupported:
        return "NotSupported";
      default:
        return "???";
    }
  }
 private:
  Code _code;
};
#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream& operator<<(std::ostream& s,
                                const DeserializationError& e) {
  s << e.c_str();
  return s;
}
inline std::ostream& operator<<(std::ostream& s, DeserializationError::Code c) {
  s << DeserializationError(c).c_str();
  return s;
}
#endif
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_PROGMEM
namespace ArduinoJson {
namespace Internals {
class UnsafeFlashStringReader {
  const char* _ptr;
 public:
  explicit UnsafeFlashStringReader(const __FlashStringHelper* ptr)
      : _ptr(reinterpret_cast<const char*>(ptr)) {}
  char read() {
    return pgm_read_byte_near(_ptr++);
  }
  bool ended() const {
    return false;
  }
};
class SafeFlashStringReader {
  const char* _ptr;
  const char* _end;
 public:
  explicit SafeFlashStringReader(const __FlashStringHelper* ptr, size_t size)
      : _ptr(reinterpret_cast<const char*>(ptr)), _end(_ptr + size) {}
  char read() {
    return pgm_read_byte_near(_ptr++);
  }
  bool ended() const {
    return _ptr == _end;
  }
};
inline UnsafeFlashStringReader makeReader(const __FlashStringHelper* input) {
  return UnsafeFlashStringReader(input);
}
inline SafeFlashStringReader makeReader(const __FlashStringHelper* input,
                                        size_t size) {
  return SafeFlashStringReader(input, size);
}
}  // namespace Internals
}  // namespace ArduinoJson
#endif
namespace ArduinoJson {
namespace Internals {
template <typename TIterator>
class IteratorReader {
  TIterator _ptr, _end;
 public:
  explicit IteratorReader(TIterator begin, TIterator end)
      : _ptr(begin), _end(end) {}
  bool ended() const {
    return _ptr == _end;
  }
  char read() {
    return char(*_ptr++);
  }
};
template <typename TInput>
inline IteratorReader<typename TInput::const_iterator> makeReader(
    const TInput& input) {
  return IteratorReader<typename TInput::const_iterator>(input.begin(),
                                                         input.end());
}
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <istream>
namespace ArduinoJson {
namespace Internals {
class StdStreamReader {
  std::istream& _stream;
  char _current;
 public:
  explicit StdStreamReader(std::istream& stream)
      : _stream(stream), _current(0) {}
  bool ended() const {
    return _stream.eof();
  }
  char read() {
    return static_cast<char>(_stream.get());
  }
 private:
  StdStreamReader& operator=(const StdStreamReader&);  // Visual Studio C4512
};
inline StdStreamReader makeReader(std::istream& input) {
  return StdStreamReader(input);
}
}  // namespace Internals
}  // namespace ArduinoJson
#endif
namespace ArduinoJson {
namespace Internals {
template <template <typename, typename> class TDeserializer,
          typename TJsonBuffer, typename TReader, typename TWriter>
TDeserializer<TReader, TWriter> makeDeserializer(TJsonBuffer *buffer,
                                                 TReader reader, TWriter writer,
                                                 uint8_t nestingLimit) {
  return TDeserializer<TReader, TWriter>(buffer, reader, writer, nestingLimit);
}
template <template <typename, typename> class TDeserializer, typename TDocument,
          typename TString>
typename Internals::enable_if<!Internals::is_array<TString>::value,
                              DeserializationError>::type
deserialize(TDocument &doc, const TString &input) {
  using namespace Internals;
  return makeDeserializer<TDeserializer>(&doc.buffer(), makeReader(input),
                                         makeStringStorage(doc.buffer(), input),
                                         doc.nestingLimit)
      .parse(doc.template to<JsonVariant>());
}
template <template <typename, typename> class TDeserializer, typename TDocument,
          typename TChar>
DeserializationError deserialize(TDocument &doc, TChar *input) {
  using namespace Internals;
  return makeDeserializer<TDeserializer>(&doc.buffer(), makeReader(input),
                                         makeStringStorage(doc.buffer(), input),
                                         doc.nestingLimit)
      .parse(doc.template to<JsonVariant>());
}
template <template <typename, typename> class TDeserializer, typename TDocument,
          typename TChar>
DeserializationError deserialize(TDocument &doc, TChar *input,
                                 size_t inputSize) {
  using namespace Internals;
  return makeDeserializer<TDeserializer>(
             &doc.buffer(), makeReader(input, inputSize),
             makeStringStorage(doc.buffer(), input), doc.nestingLimit)
      .parse(doc.template to<JsonVariant>());
}
template <template <typename, typename> class TDeserializer, typename TDocument,
          typename TStream>
DeserializationError deserialize(TDocument &doc, TStream &input) {
  using namespace Internals;
  return makeDeserializer<TDeserializer>(&doc.buffer(), makeReader(input),
                                         makeStringStorage(doc.buffer(), input),
                                         doc.nestingLimit)
      .parse(doc.template to<JsonVariant>());
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class EscapeSequence {
 public:
  static char escapeChar(char c) {
    const char *p = escapeTable(false);
    while (p[0] && p[1] != c) {
      p += 2;
    }
    return p[0];
  }
  static char unescapeChar(char c) {
    const char *p = escapeTable(true);
    for (;;) {
      if (p[0] == '\0') return c;
      if (p[0] == c) return p[1];
      p += 2;
    }
  }
 private:
  static const char *escapeTable(bool excludeIdenticals) {
    return &"\"\"\\\\b\bf\fn\nr\rt\t"[excludeIdenticals ? 4 : 0];
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TReader, typename TStringStorage>
class JsonDeserializer {
 public:
  JsonDeserializer(JsonBuffer *buffer, TReader reader,
                   TStringStorage stringStorage, uint8_t nestingLimit)
      : _buffer(buffer),
        _reader(reader),
        _stringStorage(stringStorage),
        _nestingLimit(nestingLimit),
        _loaded(false) {}
  DeserializationError parse(JsonVariant &variant) {
    DeserializationError err = skipSpacesAndComments();
    if (err) return err;
    switch (current()) {
      case '[':
        return parseArray(variant);
      case '{':
        return parseObject(variant);
      default:
        return parseValue(variant);
    }
  }
 private:
  JsonDeserializer &operator=(const JsonDeserializer &);  // non-copiable
  char current() {
    if (!_loaded) {
      if (_reader.ended())
        _current = 0;
      else
        _current = _reader.read();
      _loaded = true;
    }
    return _current;
  }
  void move() {
    _loaded = false;
  }
  FORCE_INLINE bool eat(char charToSkip) {
    if (current() != charToSkip) return false;
    move();
    return true;
  }
  DeserializationError parseArray(JsonVariant &variant) {
    if (_nestingLimit == 0) return DeserializationError::TooDeep;
    JsonArray array(_buffer);
    if (array.isNull()) return DeserializationError::NoMemory;
    variant = array;
    if (!eat('[')) return DeserializationError::InvalidInput;
    DeserializationError err = skipSpacesAndComments();
    if (err) return err;
    if (eat(']')) return DeserializationError::Ok;
    for (;;) {
      JsonVariant value;
      _nestingLimit--;
      err = parse(value);
      _nestingLimit++;
      if (err) return err;
      if (!array.add(value)) return DeserializationError::NoMemory;
      err = skipSpacesAndComments();
      if (err) return err;
      if (eat(']')) return DeserializationError::Ok;
      if (!eat(',')) return DeserializationError::InvalidInput;
    }
  }
  DeserializationError parseObject(JsonVariant &variant) {
    if (_nestingLimit == 0) return DeserializationError::TooDeep;
    JsonObject object(_buffer);
    if (object.isNull()) return DeserializationError::NoMemory;
    variant = object;
    if (!eat('{')) return DeserializationError::InvalidInput;
    DeserializationError err = skipSpacesAndComments();
    if (err) return err;
    if (eat('}')) return DeserializationError::Ok;
    for (;;) {
      const char *key;
      err = parseKey(&key);
      if (err) return err;
      err = skipSpacesAndComments();
      if (err) return err;  // Colon
      if (!eat(':')) return DeserializationError::InvalidInput;
      JsonVariant value;
      _nestingLimit--;
      err = parse(value);
      _nestingLimit++;
      if (err) return err;
      if (!object.set(key, value)) return DeserializationError::NoMemory;
      err = skipSpacesAndComments();
      if (err) return err;
      if (eat('}')) return DeserializationError::Ok;
      if (!eat(',')) return DeserializationError::InvalidInput;
      err = skipSpacesAndComments();
      if (err) return err;
    }
  }
  DeserializationError parseValue(JsonVariant &variant) {
    if (isQuote(current())) {
      return parseStringValue(variant);
    } else {
      return parseNumericValue(variant);
    }
  }
  DeserializationError parseKey(const char **key) {
    if (isQuote(current())) {
      return parseQuotedString(key);
    } else {
      return parseNonQuotedString(key);
    }
  }
  DeserializationError parseStringValue(JsonVariant &variant) {
    const char *value;
    DeserializationError err = parseQuotedString(&value);
    if (err) return err;
    variant = value;
    return DeserializationError::Ok;
  }
  DeserializationError parseQuotedString(const char **result) {
    typename remove_reference<TStringStorage>::type::String str =
        _stringStorage.startString();
    char stopChar = current();
    move();
    for (;;) {
      char c = current();
      move();
      if (c == stopChar) break;
      if (c == '\0') return DeserializationError::IncompleteInput;
      if (c == '\\') {
        c = current();
        if (c == '\0') return DeserializationError::IncompleteInput;
        if (c == 'u') return DeserializationError::NotSupported;
        c = EscapeSequence::unescapeChar(c);
        if (c == '\0') return DeserializationError::InvalidInput;
        move();
      }
      str.append(c);
    }
    *result = str.c_str();
    if (*result == NULL) return DeserializationError::NoMemory;
    return DeserializationError::Ok;
  }
  DeserializationError parseNonQuotedString(const char **result) {
    typename remove_reference<TStringStorage>::type::String str =
        _stringStorage.startString();
    char c = current();
    if (c == '\0') return DeserializationError::IncompleteInput;
    if (canBeInNonQuotedString(c)) {  // no quotes
      do {
        move();
        str.append(c);
        c = current();
      } while (canBeInNonQuotedString(c));
    } else {
      return DeserializationError::InvalidInput;
    }
    *result = str.c_str();
    if (*result == NULL) return DeserializationError::NoMemory;
    return DeserializationError::Ok;
  }
  DeserializationError parseNumericValue(JsonVariant &result) {
    char buffer[64];
    uint8_t n = 0;
    char c = current();
    while (canBeInNonQuotedString(c) && n < 63) {
      move();
      buffer[n++] = c;
      c = current();
    }
    buffer[n] = 0;
    if (isInteger(buffer)) {
      result = parseInteger<JsonInteger>(buffer);
    } else if (isFloat(buffer)) {
      result = parseFloat<JsonFloat>(buffer);
    } else if (!strcmp(buffer, "true")) {
      result = true;
    } else if (!strcmp(buffer, "false")) {
      result = false;
    } else if (!strcmp(buffer, "null")) {
      result = static_cast<const char *>(0);
    } else {
      return DeserializationError::InvalidInput;
    }
    return DeserializationError::Ok;
  }
  static inline bool isBetween(char c, char min, char max) {
    return min <= c && c <= max;
  }
  static inline bool canBeInNonQuotedString(char c) {
    return isBetween(c, '0', '9') || isBetween(c, '_', 'z') ||
           isBetween(c, 'A', 'Z') || c == '+' || c == '-' || c == '.';
  }
  static inline bool isQuote(char c) {
    return c == '\'' || c == '\"';
  }
  DeserializationError skipSpacesAndComments() {
    for (;;) {
      switch (current()) {
        case '\0':
          return DeserializationError::IncompleteInput;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          move();
          continue;
        case '/':
          move();  // skip '/'
          switch (current()) {
            case '*': {
              move();  // skip '*'
              bool wasStar = false;
              for (;;) {
                char c = current();
                if (c == '\0') return DeserializationError::IncompleteInput;
                if (c == '/' && wasStar) {
                  move();
                  break;
                }
                wasStar = c == '*';
                move();
              }
              break;
            }
            case '/':
              for (;;) {
                move();
                char c = current();
                if (c == '\0') return DeserializationError::IncompleteInput;
                if (c == '\n') break;
              }
              break;
            default:
              return DeserializationError::InvalidInput;
          }
          break;
        default:
          return DeserializationError::Ok;
      }
    }
  }
  JsonBuffer *_buffer;
  TReader _reader;
  TStringStorage _stringStorage;
  uint8_t _nestingLimit;
  char _current;
  bool _loaded;
};  // namespace Internals
}  // namespace Internals
template <typename TDocument, typename TInput>
DeserializationError deserializeJson(TDocument &doc, const TInput &input) {
  using namespace Internals;
  return deserialize<JsonDeserializer>(doc, input);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeJson(TDocument &doc, TInput *input) {
  using namespace Internals;
  return deserialize<JsonDeserializer>(doc, input);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeJson(TDocument &doc, TInput *input,
                                     size_t inputSize) {
  using namespace Internals;
  return deserialize<JsonDeserializer>(doc, input, inputSize);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeJson(TDocument &doc, TInput &input) {
  using namespace Internals;
  return deserialize<JsonDeserializer>(doc, input);
}
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class DummyWriter {
 public:
  size_t write(uint8_t) {
    return 1;
  }
  size_t write(const uint8_t*, size_t n) {
    return n;
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <template <typename> class TSerializer, typename TSource>
size_t measure(const TSource &source) {
  DummyWriter dp;
  TSerializer<DummyWriter> serializer(dp);
  source.visit(serializer);
  return serializer.bytesWritten();
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
class StaticStringWriter {
 public:
  StaticStringWriter(char *buf, size_t size) : end(buf + size - 1), p(buf) {
    *p = '\0';
  }
  size_t write(uint8_t c) {
    if (p >= end) return 0;
    *p++ = static_cast<char>(c);
    *p = '\0';
    return 1;
  }
  size_t write(const uint8_t *s, size_t n) {
    char *begin = p;
    while (p < end && n > 0) {
      *p++ = static_cast<char>(*s++);
      n--;
    }
    *p = '\0';
    return size_t(p - begin);
  }
 private:
  char *end;
  char *p;
};
}  // namespace Internals
}  // namespace ArduinoJson
#if ARDUINOJSON_ENABLE_STD_STREAM
#if ARDUINOJSON_ENABLE_STD_STREAM
namespace ArduinoJson {
namespace Internals {
class StreamWriter {
 public:
  explicit StreamWriter(std::ostream& os) : _os(os) {}
  size_t write(uint8_t c) {
    _os << c;
    return 1;
  }
  size_t write(const uint8_t* s, size_t n) {
    _os.write(reinterpret_cast<const char*>(s),
              static_cast<std::streamsize>(n));
    return n;
  }
 private:
  StreamWriter& operator=(const StreamWriter&);
  std::ostream& _os;
};
}  // namespace Internals
}  // namespace ArduinoJson
#endif  // ARDUINOJSON_ENABLE_STD_STREAM
#endif
namespace ArduinoJson {
namespace Internals {
template <template <typename> class TSerializer, typename TSource,
          typename TPrint>
typename enable_if<!IsWriteableString<TPrint>::value, size_t>::type serialize(
    const TSource &source, TPrint &destination) {
  TSerializer<TPrint> serializer(destination);
  source.visit(serializer);
  return serializer.bytesWritten();
}
#if ARDUINOJSON_ENABLE_STD_STREAM
template <template <typename> class TSerializer, typename TSource>
size_t serialize(const TSource &source, std::ostream &os) {
  StreamWriter writer(os);
  return serialize<TSerializer>(source, writer);
}
#endif
template <template <typename> class TSerializer, typename TSource>
size_t serialize(const TSource &source, char *buffer, size_t bufferSize) {
  StaticStringWriter writer(buffer, bufferSize);
  return serialize<TSerializer>(source, writer);
}
template <template <typename> class TSerializer, typename TSource, size_t N>
size_t serialize(const TSource &source, char (&buffer)[N]) {
  StaticStringWriter writer(buffer, N);
  return serialize<TSerializer>(source, writer);
}
template <template <typename> class TSerializer, typename TSource,
          typename TString>
typename enable_if<IsWriteableString<TString>::value, size_t>::type serialize(
    const TSource &source, TString &str) {
  DynamicStringWriter<TString> writer(str);
  return serialize<TSerializer>(source, writer);
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TFloat>
struct FloatParts {
  uint32_t integral;
  uint32_t decimal;
  int16_t exponent;
  int8_t decimalPlaces;
  FloatParts(TFloat value) {
    uint32_t maxDecimalPart = sizeof(TFloat) >= 8 ? 1000000000 : 1000000;
    decimalPlaces = sizeof(TFloat) >= 8 ? 9 : 6;
    exponent = normalize(value);
    integral = uint32_t(value);
    for (uint32_t tmp = integral; tmp >= 10; tmp /= 10) {
      maxDecimalPart /= 10;
      decimalPlaces--;
    }
    TFloat remainder = (value - TFloat(integral)) * TFloat(maxDecimalPart);
    decimal = uint32_t(remainder);
    remainder = remainder - TFloat(decimal);
    decimal += uint32_t(remainder * 2);
    if (decimal >= maxDecimalPart) {
      decimal = 0;
      integral++;
      if (exponent && integral >= 10) {
        exponent++;
        integral = 1;
      }
    }
    while (decimal % 10 == 0 && decimalPlaces > 0) {
      decimal /= 10;
      decimalPlaces--;
    }
  }
  static int16_t normalize(TFloat& value) {
    typedef FloatTraits<TFloat> traits;
    int16_t powersOf10 = 0;
    int8_t index = sizeof(TFloat) == 8 ? 8 : 5;
    int bit = 1 << index;
    if (value >= ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value >= traits::positiveBinaryPowerOfTen(index)) {
          value *= traits::negativeBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 + bit);
        }
        bit >>= 1;
      }
    }
    if (value > 0 && value <= ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value < traits::negativeBinaryPowerOfTenPlusOne(index)) {
          value *= traits::positiveBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 - bit);
        }
        bit >>= 1;
      }
    }
    return powersOf10;
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TWriter>
class JsonWriter {
 public:
  explicit JsonWriter(TWriter &writer) : _writer(writer), _length(0) {}
  size_t bytesWritten() const {
    return _length;
  }
  void beginArray() {
    writeRaw('[');
  }
  void endArray() {
    writeRaw(']');
  }
  void beginObject() {
    writeRaw('{');
  }
  void endObject() {
    writeRaw('}');
  }
  void writeColon() {
    writeRaw(':');
  }
  void writeComma() {
    writeRaw(',');
  }
  void writeBoolean(bool value) {
    if (value)
      writeRaw("true");
    else
      writeRaw("false");
  }
  void writeString(const char *value) {
    if (!value) {
      writeRaw("null");
    } else {
      writeRaw('\"');
      while (*value) writeChar(*value++);
      writeRaw('\"');
    }
  }
  void writeChar(char c) {
    char specialChar = EscapeSequence::escapeChar(c);
    if (specialChar) {
      writeRaw('\\');
      writeRaw(specialChar);
    } else {
      writeRaw(c);
    }
  }
  template <typename TFloat>
  void writeFloat(TFloat value) {
    if (isnan(value)) return writeRaw("NaN");
    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }
    if (isinf(value)) return writeRaw("Infinity");
    FloatParts<TFloat> parts(value);
    writeInteger(parts.integral);
    if (parts.decimalPlaces) writeDecimals(parts.decimal, parts.decimalPlaces);
    if (parts.exponent < 0) {
      writeRaw("e-");
      writeInteger(-parts.exponent);
    }
    if (parts.exponent > 0) {
      writeRaw('e');
      writeInteger(parts.exponent);
    }
  }
  template <typename UInt>
  void writeInteger(UInt value) {
    char buffer[22];
    char *end = buffer + sizeof(buffer);
    char *begin = end;
    do {
      *--begin = char(value % 10 + '0');
      value = UInt(value / 10);
    } while (value);
    writeRaw(begin, end);
  }
  void writeDecimals(uint32_t value, int8_t width) {
    char buffer[16];
    char *end = buffer + sizeof(buffer);
    char *begin = end;
    while (width--) {
      *--begin = char(value % 10 + '0');
      value /= 10;
    }
    *--begin = '.';
    writeRaw(begin, end);
  }
  void writeRaw(const char *s) {
    _length += _writer.write(reinterpret_cast<const uint8_t *>(s), strlen(s));
  }
  void writeRaw(const char *begin, const char *end) {
    _length += _writer.write(reinterpret_cast<const uint8_t *>(begin),
                             static_cast<size_t>(end - begin));
  }
  template <size_t N>
  void writeRaw(const char (&s)[N]) {
    _length += _writer.write(reinterpret_cast<const uint8_t *>(s), N - 1);
  }
  void writeRaw(char c) {
    _length += _writer.write(static_cast<uint8_t>(c));
  }
 protected:
  TWriter &_writer;
  size_t _length;
 private:
  JsonWriter &operator=(const JsonWriter &);  // cannot be assigned
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TWriter>
class JsonSerializer {
 public:
  JsonSerializer(TWriter &writer) : _writer(writer) {}
  void acceptFloat(JsonFloat value) {
    _writer.writeFloat(value);
  }
  void acceptArray(const JsonArray &array) {
    _writer.beginArray();
    JsonArray::const_iterator it = array.begin();
    while (it != array.end()) {
      it->visit(*this);
      ++it;
      if (it == array.end()) break;
      _writer.writeComma();
    }
    _writer.endArray();
  }
  void acceptObject(const JsonObject &object) {
    _writer.beginObject();
    JsonObject::const_iterator it = object.begin();
    while (it != object.end()) {
      _writer.writeString(it->key);
      _writer.writeColon();
      it->value.visit(*this);
      ++it;
      if (it == object.end()) break;
      _writer.writeComma();
    }
    _writer.endObject();
  }
  void acceptString(const char *value) {
    _writer.writeString(value);
  }
  void acceptRawJson(const char *data, size_t n) {
    for (size_t i = 0; i < n; i++) _writer.writeRaw(data[i]);
  }
  void acceptNegativeInteger(JsonUInt value) {
    _writer.writeRaw('-');
    _writer.writeInteger(value);
  }
  void acceptPositiveInteger(JsonUInt value) {
    _writer.writeInteger(value);
  }
  void acceptBoolean(bool value) {
    _writer.writeBoolean(value);
  }
  void acceptNull() {
    _writer.writeRaw("null");
  }
  size_t bytesWritten() const {
    return _writer.bytesWritten();
  }
 private:
  JsonWriter<TWriter> _writer;
};
}  // namespace Internals
template <typename TSource, typename TDestination>
size_t serializeJson(TSource &source, TDestination &destination) {
  using namespace Internals;
  return serialize<JsonSerializer>(source, destination);
}
template <typename TSource>
size_t serializeJson(const TSource &source, char *buffer, size_t bufferSize) {
  using namespace Internals;
  return serialize<JsonSerializer>(source, buffer, bufferSize);
}
template <typename TSource>
size_t measureJson(const TSource &source) {
  using namespace Internals;
  return measure<JsonSerializer>(source);
}
#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream &operator<<(std::ostream &os, const JsonArray &source) {
  serializeJson(source, os);
  return os;
}
inline std::ostream &operator<<(std::ostream &os, const JsonObject &source) {
  serializeJson(source, os);
  return os;
}
inline std::ostream &operator<<(std::ostream &os, const JsonVariant &source) {
  serializeJson(source, os);
  return os;
}
namespace Internals {
inline std::ostream &operator<<(std::ostream &os,
                                const JsonArraySubscript &source) {
  serializeJson(source, os);
  return os;
}
template <typename TKey>
inline std::ostream &operator<<(std::ostream &os,
                                const JsonObjectSubscript<TKey> &source) {
  serializeJson(source, os);
  return os;
}
}  // namespace Internals
#endif
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename Print>
class IndentedPrint {
 public:
  explicit IndentedPrint(Print &p) : sink(&p) {
    level = 0;
    tabSize = 2;
    isNewLine = true;
  }
  size_t write(uint8_t c) {
    size_t n = 0;
    if (isNewLine) n += writeTabs();
    n += sink->write(c);
    isNewLine = c == '\n';
    return n;
  }
  size_t write(const uint8_t *s, size_t n) {
    size_t bytesWritten = 0;
    while (n > 0) {
      bytesWritten += write(*s++);
      n--;
    }
    return bytesWritten;
  }
  void indent() {
    if (level < MAX_LEVEL) level++;
  }
  void unindent() {
    if (level > 0) level--;
  }
  void setTabSize(uint8_t n) {
    if (n < MAX_TAB_SIZE) tabSize = n & MAX_TAB_SIZE;
  }
 private:
  Print *sink;
  uint8_t level : 4;
  uint8_t tabSize : 3;
  bool isNewLine : 1;
  size_t writeTabs() {
    size_t n = 0;
    for (int i = 0; i < level * tabSize; i++) n += sink->write(' ');
    return n;
  }
  static const int MAX_LEVEL = 15;    // because it's only 4 bits
  static const int MAX_TAB_SIZE = 7;  // because it's only 3 bits
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TWriter>
class Prettyfier {
 public:
  explicit Prettyfier(IndentedPrint<TWriter>& p) : _sink(p) {
    _previousChar = 0;
    _inString = false;
  }
  size_t write(uint8_t c) {
    size_t n = _inString ? handleStringChar(c) : handleMarkupChar(char(c));
    _previousChar = char(c);
    return n;
  }
  size_t write(const uint8_t* s, size_t n) {
    size_t bytesWritten = 0;
    while (n > 0) {
      bytesWritten += write(*s++);
      n--;
    }
    return bytesWritten;
  }
 private:
  Prettyfier& operator=(const Prettyfier&);  // cannot be assigned
  bool inEmptyBlock() {
    return _previousChar == '{' || _previousChar == '[';
  }
  size_t handleStringChar(uint8_t c) {
    bool isQuote = c == '"' && _previousChar != '\\';
    if (isQuote) _inString = false;
    return _sink.write(c);
  }
  size_t handleMarkupChar(char c) {
    switch (c) {
      case '{':
      case '[':
        return writeBlockOpen(c);
      case '}':
      case ']':
        return writeBlockClose(c);
      case ':':
        return writeColon();
      case ',':
        return writeComma();
      case '"':
        return writeQuoteOpen();
      default:
        return writeNormalChar(c);
    }
  }
  size_t writeBlockClose(char c) {
    size_t n = 0;
    n += unindentIfNeeded();
    n += write(c);
    return n;
  }
  size_t writeBlockOpen(char c) {
    size_t n = 0;
    n += indentIfNeeded();
    n += write(c);
    return n;
  }
  size_t writeColon() {
    size_t n = 0;
    n += write(": ");
    return n;
  }
  size_t writeComma() {
    size_t n = 0;
    n += write(",\r\n");
    return n;
  }
  size_t writeQuoteOpen() {
    _inString = true;
    size_t n = 0;
    n += indentIfNeeded();
    n += write('"');
    return n;
  }
  size_t writeNormalChar(char c) {
    size_t n = 0;
    n += indentIfNeeded();
    n += write(c);
    return n;
  }
  size_t indentIfNeeded() {
    if (!inEmptyBlock()) return 0;
    _sink.indent();
    return write("\r\n");
  }
  size_t unindentIfNeeded() {
    if (inEmptyBlock()) return 0;
    _sink.unindent();
    return write("\r\n");
  }
  size_t write(char c) {
    return _sink.write(static_cast<uint8_t>(c));
  }
  template <size_t N>
  size_t write(const char (&s)[N]) {
    return _sink.write(reinterpret_cast<const uint8_t*>(s), N - 1);
  }
  char _previousChar;
  IndentedPrint<TWriter>& _sink;
  bool _inString;
};
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TPrint>
class PrettyJsonSerializer_Base {
 public:
  PrettyJsonSerializer_Base(TPrint &output)
      : _indentedPrint(output), _prettyfier(_indentedPrint) {}
 protected:
  IndentedPrint<TPrint> _indentedPrint;
  Prettyfier<TPrint> _prettyfier;
};
template <typename TPrint>
class PrettyJsonSerializer : PrettyJsonSerializer_Base<TPrint>,
                             public JsonSerializer<Prettyfier<TPrint> > {
 public:
  PrettyJsonSerializer(TPrint &output)
      : PrettyJsonSerializer_Base<TPrint>(output),
        JsonSerializer<Prettyfier<TPrint> >(
            PrettyJsonSerializer_Base<TPrint>::_prettyfier) {}
};
}  // namespace Internals
template <typename TSource, typename TDestination>
size_t serializeJsonPretty(TSource &source, TDestination &destination) {
  using namespace Internals;
  return serialize<PrettyJsonSerializer>(source, destination);
}
template <typename TSource>
size_t serializeJsonPretty(const TSource &source, char *buffer,
                           size_t bufferSize) {
  using namespace Internals;
  return serialize<PrettyJsonSerializer>(source, buffer, bufferSize);
}
template <typename TSource>
size_t measureJsonPretty(const TSource &source) {
  using namespace Internals;
  return measure<PrettyJsonSerializer>(source);
}
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename T>
inline void swap(T& a, T& b) {
  T t(a);
  a = b;
  b = t;
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 8>) {
  swap(p[0], p[7]);
  swap(p[1], p[6]);
  swap(p[2], p[5]);
  swap(p[3], p[4]);
}
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 4>) {
  swap(p[0], p[3]);
  swap(p[1], p[2]);
}
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 2>) {
  swap(p[0], p[1]);
}
inline void fixEndianess(uint8_t*, integral_constant<size_t, 1>) {}
template <typename T>
inline void fixEndianess(T& value) {
#if ARDUINOJSON_LITTLE_ENDIAN
  fixEndianess(reinterpret_cast<uint8_t*>(&value),
               integral_constant<size_t, sizeof(T)>());
#endif
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
inline void doubleToFloat(const uint8_t d[8], uint8_t f[4]) {
  f[0] = uint8_t((d[0] & 0xC0) | (d[0] << 3 & 0x3f) | (d[1] >> 5));
  f[1] = uint8_t((d[1] << 3) | (d[2] >> 5));
  f[2] = uint8_t((d[2] << 3) | (d[3] >> 5));
  f[3] = uint8_t((d[3] << 3) | (d[4] >> 5));
}
}  // namespace Internals
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TReader, typename TStringStorage>
class MsgPackDeserializer {
 public:
  MsgPackDeserializer(JsonBuffer *buffer, TReader reader,
                      TStringStorage stringStorage, uint8_t nestingLimit)
      : _buffer(buffer),
        _reader(reader),
        _stringStorage(stringStorage),
        _nestingLimit(nestingLimit) {}
  DeserializationError parse(JsonVariant &variant) {
    uint8_t code;
    if (!readByte(code)) return DeserializationError::IncompleteInput;
    if ((code & 0x80) == 0) {
      variant = code;
      return DeserializationError::Ok;
    }
    if ((code & 0xe0) == 0xe0) {
      variant = static_cast<int8_t>(code);
      return DeserializationError::Ok;
    }
    if ((code & 0xe0) == 0xa0) {
      return readString(variant, code & 0x1f);
    }
    if ((code & 0xf0) == 0x90) return readArray(variant, code & 0x0F);
    if ((code & 0xf0) == 0x80) return readObject(variant, code & 0x0F);
    switch (code) {
      case 0xc0:
        variant = static_cast<char *>(0);
        return DeserializationError::Ok;
      case 0xc2:
        variant = false;
        return DeserializationError::Ok;
      case 0xc3:
        variant = true;
        return DeserializationError::Ok;
      case 0xcc:
        return readInteger<uint8_t>(variant);
      case 0xcd:
        return readInteger<uint16_t>(variant);
      case 0xce:
        return readInteger<uint32_t>(variant);
      case 0xcf:
#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
        return readInteger<uint64_t>(variant);
#else
        readInteger<uint32_t>();
        return readInteger<uint32_t>(variant);
#endif
      case 0xd0:
        return readInteger<int8_t>(variant);
      case 0xd1:
        return readInteger<int16_t>(variant);
      case 0xd2:
        return readInteger<int32_t>(variant);
      case 0xd3:
#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
        return readInteger<int64_t>(variant);
#else
        if (!skip(4)) return DeserializationError::IncompleteInput;
        return readInteger<int32_t>(variant);
#endif
      case 0xca:
        return readFloat<float>(variant);
      case 0xcb:
        return readDouble<double>(variant);
      case 0xd9:
        return readString<uint8_t>(variant);
      case 0xda:
        return readString<uint16_t>(variant);
      case 0xdb:
        return readString<uint32_t>(variant);
      case 0xdc:
        return readArray<uint16_t>(variant);
      case 0xdd:
        return readArray<uint32_t>(variant);
      case 0xde:
        return readObject<uint16_t>(variant);
      case 0xdf:
        return readObject<uint32_t>(variant);
      default:
        return DeserializationError::NotSupported;
    }
  }
 private:
  MsgPackDeserializer &operator=(const MsgPackDeserializer &);
  bool skip(uint8_t n) {
    while (n--) {
      if (_reader.ended()) return false;
      _reader.read();
    }
    return true;
  }
  bool readByte(uint8_t &value) {
    if (_reader.ended()) return false;
    value = static_cast<uint8_t>(_reader.read());
    return true;
  }
  bool readBytes(uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i++) {
      if (!readByte(p[i])) return false;
    }
    return true;
  }
  template <typename T>
  bool readBytes(T &value) {
    return readBytes(reinterpret_cast<uint8_t *>(&value), sizeof(value));
  }
  template <typename T>
  T readInteger() {
    T value;
    readBytes(value);
    fixEndianess(value);
    return value;
  }
  template <typename T>
  bool readInteger(T &value) {
    if (!readBytes(value)) return false;
    fixEndianess(value);
    return true;
  }
  template <typename T>
  DeserializationError readInteger(JsonVariant &variant) {
    T value;
    if (!readInteger(value)) return DeserializationError::IncompleteInput;
    variant = value;
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 4, DeserializationError>::type readFloat(
      JsonVariant &variant) {
    T value;
    if (!readBytes(value)) return DeserializationError::IncompleteInput;
    fixEndianess(value);
    variant = value;
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 8, DeserializationError>::type readDouble(
      JsonVariant &variant) {
    T value;
    if (!readBytes(value)) return DeserializationError::IncompleteInput;
    fixEndianess(value);
    variant = value;
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 4, DeserializationError>::type readDouble(
      JsonVariant &variant) {
    uint8_t i[8];  // input is 8 bytes
    T value;       // output is 4 bytes
    uint8_t *o = reinterpret_cast<uint8_t *>(&value);
    if (!readBytes(i, 8)) return DeserializationError::IncompleteInput;
    doubleToFloat(i, o);
    fixEndianess(value);
    variant = value;
    return DeserializationError::Ok;
  }
  template <typename T>
  DeserializationError readString(JsonVariant &variant) {
    T size;
    if (!readInteger(size)) return DeserializationError::IncompleteInput;
    return readString(variant, size);
  }
  DeserializationError readString(JsonVariant &variant, size_t n) {
    typename remove_reference<TStringStorage>::type::String str =
        _stringStorage.startString();
    for (; n; --n) {
      uint8_t c;
      if (!readBytes(c)) return DeserializationError::IncompleteInput;
      str.append(static_cast<char>(c));
    }
    const char *s = str.c_str();
    if (s == NULL) return DeserializationError::NoMemory;
    variant = s;
    return DeserializationError::Ok;
  }
  template <typename TSize>
  DeserializationError readArray(JsonVariant &variant) {
    TSize size;
    if (!readInteger(size)) return DeserializationError::IncompleteInput;
    return readArray(variant, size);
  }
  DeserializationError readArray(JsonVariant &variant, size_t n) {
    JsonArray array(_buffer);
    if (array.isNull()) return DeserializationError::NoMemory;
    variant = array;
    return readArray(array, n);
  }
  DeserializationError readArray(JsonArray array, size_t n) {
    if (_nestingLimit == 0) return DeserializationError::TooDeep;
    --_nestingLimit;
    for (; n; --n) {
      JsonVariant variant;
      DeserializationError err = parse(variant);
      if (err) return err;
      if (!array.add(variant)) return DeserializationError::NoMemory;
    }
    ++_nestingLimit;
    return DeserializationError::Ok;
  }
  template <typename TSize>
  DeserializationError readObject(JsonVariant &variant) {
    TSize size;
    if (!readInteger(size)) return DeserializationError::IncompleteInput;
    return readObject(variant, size);
  }
  DeserializationError readObject(JsonVariant &variant, size_t n) {
    JsonObject object(_buffer);
    if (object.isNull()) return DeserializationError::NoMemory;
    variant = object;
    return readObject(object, n);
  }
  DeserializationError readObject(JsonObject object, size_t n) {
    if (_nestingLimit == 0) return DeserializationError::TooDeep;
    --_nestingLimit;
    for (; n; --n) {
      DeserializationError err;
      JsonVariant variant;
      err = parse(variant);
      if (err) return err;
      const char *key = variant.as<char *>();
      if (!key) return DeserializationError::NotSupported;
      err = parse(variant);
      if (err) return err;
      if (!object.set(key, variant)) return DeserializationError::NoMemory;
    }
    ++_nestingLimit;
    return DeserializationError::Ok;
  }
  JsonBuffer *_buffer;
  TReader _reader;
  TStringStorage _stringStorage;
  uint8_t _nestingLimit;
};
}  // namespace Internals
template <typename TDocument, typename TInput>
DeserializationError deserializeMsgPack(TDocument &doc, const TInput &input) {
  using namespace Internals;
  return deserialize<MsgPackDeserializer>(doc, input);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeMsgPack(TDocument &doc, TInput *input) {
  using namespace Internals;
  return deserialize<MsgPackDeserializer>(doc, input);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeMsgPack(TDocument &doc, TInput *input,
                                        size_t inputSize) {
  using namespace Internals;
  return deserialize<MsgPackDeserializer>(doc, input, inputSize);
}
template <typename TDocument, typename TInput>
DeserializationError deserializeMsgPack(TDocument &doc, TInput &input) {
  using namespace Internals;
  return deserialize<MsgPackDeserializer>(doc, input);
}
}  // namespace ArduinoJson
namespace ArduinoJson {
namespace Internals {
template <typename TWriter>
class MsgPackSerializer {
 public:
  MsgPackSerializer(TWriter& writer) : _writer(&writer), _bytesWritten(0) {}
  template <typename T>
  typename enable_if<sizeof(T) == 4>::type acceptFloat(T value32) {
    writeByte(0xCA);
    writeInteger(value32);
  }
  template <typename T>
  typename enable_if<sizeof(T) == 8>::type acceptFloat(T value64) {
    float value32 = float(value64);
    if (value32 == value64) {
      writeByte(0xCA);
      writeInteger(value32);
    } else {
      writeByte(0xCB);
      writeInteger(value64);
    }
  }
  void acceptArray(const JsonArray& array) {
    size_t n = array.size();
    if (n < 0x10) {
      writeByte(uint8_t(0x90 + array.size()));
    } else if (n < 0x10000) {
      writeByte(0xDC);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDD);
      writeInteger(uint32_t(n));
    }
    for (JsonArray::const_iterator it = array.begin(); it != array.end();
         ++it) {
      it->visit(*this);
    }
  }
  void acceptObject(const JsonObject& object) {
    size_t n = object.size();
    if (n < 0x10) {
      writeByte(uint8_t(0x80 + n));
    } else if (n < 0x10000) {
      writeByte(0xDE);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDF);
      writeInteger(uint32_t(n));
    }
    for (JsonObject::const_iterator it = object.begin(); it != object.end();
         ++it) {
      acceptString(it->key);
      it->value.visit(*this);
    }
  }
  void acceptString(const char* value) {
    if (!value) return writeByte(0xC0);  // nil
    size_t n = strlen(value);
    if (n < 0x20) {
      writeByte(uint8_t(0xA0 + n));
    } else if (n < 0x100) {
      writeByte(0xD9);
      writeInteger(uint8_t(n));
    } else if (n < 0x10000) {
      writeByte(0xDA);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDB);
      writeInteger(uint32_t(n));
    }
    writeBytes(reinterpret_cast<const uint8_t*>(value), n);
  }
  void acceptRawJson(const char* data, size_t size) {
    writeBytes(reinterpret_cast<const uint8_t*>(data), size);
  }
  void acceptNegativeInteger(JsonUInt value) {
    JsonUInt negated = JsonUInt(~value + 1);
    if (value <= 0x20) {
      writeInteger(int8_t(negated));
    } else if (value <= 0x80) {
      writeByte(0xD0);
      writeInteger(int8_t(negated));
    } else if (value <= 0x8000) {
      writeByte(0xD1);
      writeInteger(int16_t(negated));
    } else if (value <= 0x80000000) {
      writeByte(0xD2);
      writeInteger(int32_t(negated));
    }
#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
    else {
      writeByte(0xD3);
      writeInteger(int64_t(negated));
    }
#endif
  }
  void acceptPositiveInteger(JsonUInt value) {
    if (value <= 0x7F) {
      writeInteger(uint8_t(value));
    } else if (value <= 0xFF) {
      writeByte(0xCC);
      writeInteger(uint8_t(value));
    } else if (value <= 0xFFFF) {
      writeByte(0xCD);
      writeInteger(uint16_t(value));
    } else if (value <= 0xFFFFFFFF) {
      writeByte(0xCE);
      writeInteger(uint32_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
    else {
      writeByte(0xCF);
      writeInteger(uint64_t(value));
    }
#endif
  }
  void acceptBoolean(bool value) {
    writeByte(value ? 0xC3 : 0xC2);
  }
  void acceptNull() {
    writeByte(0xC0);
  }
  size_t bytesWritten() const {
    return _bytesWritten;
  }
 private:
  void writeByte(uint8_t c) {
    _bytesWritten += _writer->write(c);
  }
  void writeBytes(const uint8_t* p, size_t n) {
    _bytesWritten += _writer->write(p, n);
  }
  template <typename T>
  void writeInteger(T value) {
    fixEndianess(value);
    writeBytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));
  }
  TWriter* _writer;
  size_t _bytesWritten;
};
}  // namespace Internals
template <typename TSource, typename TDestination>
inline size_t serializeMsgPack(const TSource& source, TDestination& output) {
  using namespace Internals;
  return serialize<MsgPackSerializer>(source, output);
}
template <typename TSource, typename TDestination>
inline size_t serializeMsgPack(const TSource& source, TDestination* output,
                               size_t size) {
  using namespace Internals;
  return serialize<MsgPackSerializer>(source, output, size);
}
template <typename TSource>
inline size_t measureMsgPack(const TSource& source) {
  using namespace Internals;
  return measure<MsgPackSerializer>(source);
}
}  // namespace ArduinoJson

using namespace ArduinoJson;

#else

#error ArduinoJson requires a C++ compiler, please change file extension to .cc or .cpp

#endif
