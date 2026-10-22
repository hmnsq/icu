// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2012, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)
//
// Abstract interface that consumes a sequence of bytes (ByteSink).
//
// Used so that we can write a single piece of code that can operate
// on a variety of output string types.
//
// Various implementations of this interface are provided:
//   ByteSink:
//      CheckedArrayByteSink    Write to a flat array, with bounds checking
//      StringByteSink          Write to an STL string

// This code is a contribution of Google code, and the style used here is
// a compromise between the original Google code and the ICU coding guidelines.
// For example, data types are ICU-ified (size_t,int->int32_t),
// and API comments doxygen-ified, but function names and behavior are
// as in the original, if possible.
// Assertion-style error handling, not available in ICU, was changed to
// parameter "pinning" similar to UnicodeString.
//
// In addition, this is only a partial port of the original Google code,
// limited to what was needed so far. The (nearly) complete original code
// is in the ICU svn repository at icuhtml/trunk/design/strings/contrib
// (see ICU ticket 6765, r25517).

#यदि_अपरिभाषितम् __BYTESTREAM_H__
#परिभाषय __BYTESTREAM_H__

/**
 * \file
 * \brief C++ API: Interface for writing bytes, and implementation classes.
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव <type_traits>

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/std_string.h"

U_NAMESPACE_BEGIN

/**
 * A ByteSink can be filled with bytes.
 * @stable ICU 4.2
 */
class U_COMMON_API ByteSink : सार्वजनिक UMemory {
सार्वजनिक:
  /**
   * Default constructor.
   * @stable ICU 4.2
   */
  ByteSink() { }
  /**
   * Virtual destructor.
   * @stable ICU 4.2
   */
  आभास ~ByteSink();

  /**
   * Append "bytes[0,n-1]" to this.
   * @param bytes the pointer to the bytes
   * @param n the number of bytes; must be non-negative
   * @stable ICU 4.2
   */
  आभास शून्य Append(ध्रुव वर्ण* अष्टकानि, int32_t n) = 0;

  /**
   * Appends n bytes to this. Same as Append().
   * Call AppendU8() with u8"string literals" which are const char * in C++11
   * but const char8_t * in C++20.
   * If the compiler does support char8_t as a distinct type,
   * then an AppendU8() overload for that is defined and will be chosen.
   *
   * @param bytes the pointer to the bytes
   * @param n the number of bytes; must be non-negative
   * @stable ICU 67
   */
  अन्तःस्थ शून्य AppendU8(ध्रुव वर्ण* अष्टकानि, int32_t n) {
    Append(अष्टकानि, n);
  }

#यदि defined(__cpp_char8_t) || defined(U_अन्तर्_DOXYGEN)
  /**
   * Appends n bytes to this. Same as Append() but for a const char8_t * pointer.
   * Call AppendU8() with u8"string literals" which are const char * in C++11
   * but const char8_t * in C++20.
   * If the compiler does support char8_t as a distinct type,
   * then this AppendU8() overload for that is defined and will be chosen.
   *
   * @param bytes the pointer to the bytes
   * @param n the number of bytes; must be non-negative
   * @stable ICU 67
   */
  अन्तःस्थ शून्य AppendU8(ध्रुव char8_t* अष्टकानि, int32_t n) {
    Append(reinterpret_cast<ध्रुव वर्ण*>(अष्टकानि), n);
  }
#यदि_अन्तः

  /**
   * Returns a writable buffer for appending and writes the buffer's capacity to
   * *result_capacity. Guarantees *result_capacity>=min_capacity.
   * May return a pointer to the caller-owned scratch buffer which must have
   * scratch_capacity>=min_capacity.
   * The returned buffer is only valid until the next operation
   * on this ByteSink.
   *
   * After writing at most *result_capacity bytes, call Append() with the
   * pointer returned from this function and the number of bytes written.
   * Many Append() implementations will avoid copying bytes if this function
   * returned an internal buffer.
   *
   * Partial usage example:
   *  int32_t capacity;
   *  char* buffer = sink->GetAppendBuffer(..., &capacity);
   *  ... Write n bytes into buffer, with n <= capacity.
   *  sink->Append(buffer, n);
   * In many implementations, that call to Append will avoid copying bytes.
   *
   * If the ByteSink allocates or reallocates an internal buffer, it should use
   * the desired_capacity_hint if appropriate.
   * If a caller cannot provide a reasonable guess at the desired capacity,
   * it should pass desired_capacity_hint=0.
   *
   * If a non-scratch buffer is returned, the caller may only pass
   * a prefix to it to Append().
   * That is, it is not correct to pass an interior pointer to Append().
   *
   * The default implementation always returns the scratch buffer.
   *
   * @param min_capacity required minimum capacity of the returned buffer;
   *                     must be non-negative
   * @param desired_capacity_hint desired capacity of the returned buffer;
   *                              must be non-negative
   * @param scratch default caller-owned buffer
   * @param scratch_capacity capacity of the scratch buffer
   * @param result_capacity pointer to an integer which will be set to the
   *                        capacity of the returned buffer
   * @return a buffer with *result_capacity>=min_capacity
   * @stable ICU 4.2
   */
  आभास वर्ण* Get_Append_बफर(int32_t min_क्षमता_2,
                                int32_t desired_क्षमता_hint,
                                वर्ण* scratch U_LIFETIME_BOUND,
                                int32_t scratch_क्षमता_2,
                                int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND;

  /**
   * Flush internal buffers.
   * Some byte sinks use internal buffers or provide buffering
   * and require calling Flush() at the end of the stream.
   * The ByteSink should be ready for further Append() calls after Flush().
   * The default implementation of Flush() does nothing.
   * @stable ICU 4.2
   */
  आभास शून्य Flush();

गुप्त:
  ByteSink(ध्रुव ByteSink &) = delete;
  ByteSink &संक्रियकः=(ध्रुव ByteSink &) = delete;
};

// -------------------------------------------------------------
// Some standard implementations

/** 
 * Implementation of ByteSink that writes to a flat byte array,
 * with bounds-checking:
 * This sink will not write more than capacity bytes to outbuf.
 * If more than capacity bytes are Append()ed, then excess bytes are ignored,
 * and Overflowed() will return true.
 * Overflow does not cause a runtime error.
 * @stable ICU 4.2
 */
class U_COMMON_API CheckedArrayByteSink : सार्वजनिक ByteSink {
सार्वजनिक:
  /**
   * Constructs a ByteSink that will write to outbuf[0..capacity-1].
   * @param outbuf buffer to write to
   * @param capacity size of the buffer
   * @stable ICU 4.2
   */
  CheckedArrayByteSink(वर्ण* outbuf, int32_t क्षमता);
  /**
   * Destructor.
   * @stable ICU 4.2
   */
  आभास ~CheckedArrayByteSink();
  /**
   * Returns the sink to its original state, without modifying the buffer.
   * Useful for reusing both the buffer and the sink for multiple streams.
   * Resets the state to NumberOfBytesWritten()=NumberOfBytesAppended()=0
   * and Overflowed()=false.
   * @return *this
   * @stable ICU 4.6
   */
  आभास CheckedArrayByteSink& Reset() U_LIFETIME_BOUND;
  /**
   * Append "bytes[0,n-1]" to this.
   * @param bytes the pointer to the bytes
   * @param n the number of bytes; must be non-negative
   * @stable ICU 4.2
   */
  आभास शून्य Append(ध्रुव वर्ण* अष्टकानि, int32_t n) override;
  /**
   * Returns a writable buffer for appending and writes the buffer's capacity to
   * *result_capacity. For details see the base class documentation.
   * @param min_capacity required minimum capacity of the returned buffer;
   *                     must be non-negative
   * @param desired_capacity_hint desired capacity of the returned buffer;
   *                              must be non-negative
   * @param scratch default caller-owned buffer
   * @param scratch_capacity capacity of the scratch buffer
   * @param result_capacity pointer to an integer which will be set to the
   *                        capacity of the returned buffer
   * @return a buffer with *result_capacity>=min_capacity
   * @stable ICU 4.2
   */
  आभास वर्ण* Get_Append_बफर(int32_t min_क्षमता_2,
                                int32_t desired_क्षमता_hint,
                                वर्ण* scratch U_LIFETIME_BOUND,
                                int32_t scratch_क्षमता_2,
                                int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND override;
  /**
   * Returns the number of bytes actually written to the sink.
   * @return number of bytes written to the buffer
   * @stable ICU 4.2
   */
  int32_t सङ्ख्या_सम्बन्धि_अष्टकानि_Written() ध्रुव { प्रत्यागम आकार; }
  /**
   * Returns true if any bytes were discarded, i.e., if there was an
   * attempt to write more than 'capacity' bytes.
   * @return true if more than 'capacity' bytes were Append()ed
   * @stable ICU 4.2
   */
  UBool Overflowed() ध्रुव { प्रत्यागम overflowed_; }
  /**
   * Returns the number of bytes appended to the sink.
   * If Overflowed() then NumberOfBytesAppended()>NumberOfBytesWritten()
   * else they return the same number.
   * @return number of bytes written to the buffer
   * @stable ICU 4.6
   */
  int32_t सङ्ख्या_सम्बन्धि_अष्टकानि_Appended() ध्रुव { प्रत्यागम appended_; }
गुप्त:
  वर्ण* outbuf_;
  ध्रुव int32_t क्षमता_2;
  int32_t आकार;
  int32_t appended_;
  UBool overflowed_;

  CheckedArrayByteSink() = delete;
  CheckedArrayByteSink(ध्रुव CheckedArrayByteSink &) = delete;
  CheckedArrayByteSink &संक्रियकः=(ध्रुव CheckedArrayByteSink &) = delete;
};

namespace prv {
/** @internal */
प्रतिरूपम्<typename StringClass, typename = शून्य>
संरचना मानम्_type_or_char {
  /** @internal */
  using type = वर्ण;
};
/** @internal */
प्रतिरूपम्<typename StringClass>
संरचना मानम्_type_or_char<StringClass, std::void_t<typename StringClass::मानम्_type>> {
  /** @internal */
  using type = typename StringClass::मानम्_type;
};
/** @internal */
प्रतिरूपम्<typename StringClass>
using मानम्_type_or_char_t = typename मानम्_type_or_char<StringClass>::type;
}

/** 
 * Implementation of ByteSink that writes to a "string".
 * The StringClass is usually instantiated with a std::string or a std::u8string.
 * StringClass must have public member functions reserve(integer type), capacity(), length(), and
 * append(value type, integer type) with the same semantics as those of std::basic_string, and must
 * have an 8-bit value type.  If the value type is not char, it must be a public member type
 * StringClass::value_type.
 * @stable ICU 4.2
 */
प्रतिरूपम्<typename StringClass>
class StringByteSink : सार्वजनिक ByteSink {
  using Unit = typename prv::मानम्_type_or_char_t<StringClass>;
 सार्वजनिक:
  /**
   * Constructs a ByteSink that will append bytes to the dest string.
   * @param dest pointer to string object to append to
   * @stable ICU 4.2
   */
  StringByteSink(StringClass* dest) : dest_(dest) { }
  /**
   * Constructs a ByteSink that reserves append capacity and will append bytes to the dest string.
   * 
   * @param dest pointer to string object to append to
   * @param initialAppendCapacity capacity beyond dest->length() to be reserve()d
   * @stable ICU 60
   */
  StringByteSink(StringClass* dest, int32_t initial_Append_क्षमता) : dest_(dest) {
    यदि (initial_Append_क्षमता > 0 &&
        static_cast<uint32_t>(initial_Append_क्षमता) > dest->क्षमता() - dest->दीर्घता()) {
      dest->reserve(dest->दीर्घता() + initial_Append_क्षमता);
    }
  }
  /**
   * Append "bytes[0,n-1]" to this.
   * @param data the pointer to the bytes
   * @param n the number of bytes; must be non-negative
   * @stable ICU 4.2
   */
  आभास शून्य Append(ध्रुव वर्ण* दत्त, int32_t n) override {
    यदि constexpr (std::is_same_v<Unit, वर्ण>) {
      dest_->append(दत्त, n);
    } अन्यथा {
      dest_->append(reinterpret_cast<ध्रुव Unit*>(दत्त), n);
    }
  }
 गुप्त:
  StringClass* dest_;

  StringByteSink() = delete;
  StringByteSink(ध्रुव StringByteSink &) = delete;
  StringByteSink &संक्रियकः=(ध्रुव StringByteSink &) = delete;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __BYTESTREAM_H__
