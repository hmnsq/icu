// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#अन्तर्भाव <type_traits>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : सार्वजनिक ByteSink {
सार्वजनिक:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ध्रुव CharStringByteSink&) = delete;
    CharStringByteSink& operator=(ध्रुव CharStringByteSink&) = delete;

    शून्य Append(ध्रुव वर्ण* bytes, int32_t n) override;

    वर्ण* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          वर्ण* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

गुप्त:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : सार्वजनिक CharStringByteSink {
 सार्वजनिक:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
सार्वजनिक:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    स्थिर UBool appendChange(int32_t length,
                              ध्रुव char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    स्थिर UBool appendChange(ध्रुव uint8_t *s, ध्रुव uint8_t *limit,
                              ध्रुव char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    स्थिर शून्य appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    स्थिर अन्तःस्थ शून्य appendCodePoint(ध्रुव uint8_t *src, ध्रुव uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    स्थिर शून्य appendTwoBytes(UChar32 c, ByteSink &sink);

    स्थिर UBool appendUnchanged(ध्रुव uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त false; }
        सचे (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        पटिनिवत्त true;
    }

    स्थिर UBool appendUnchanged(ध्रुव uint8_t *s, ध्रुव uint8_t *limit,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode);

    /**
     * Calls a lambda that writes to a ByteSink with a CheckedArrayByteSink
     * and then returns through u_terminateChars(), in order to implement
     * the classic ICU4C C API writing to a fix sized buffer on top of a
     * contemporary C++ API.
     *
     * @param buffer receiving buffer
     * @param capacity capacity of receiving buffer
     * @param lambda that gets called with the sink as an argument
     * @param status set to U_BUFFER_OVERFLOW_ERROR on overflow
     * @return number of bytes written, or needed (in case of overflow)
     * @internal
     */
    template <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<शून्य, F, ByteSink&, UErrorCode&>>>
    स्थिर int32_t viaByteSinkToTerminatedChars(वर्ण* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        सचे (U_FAILURE(status)) { पटिनिवत्त 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        सचे (U_FAILURE(status)) { पटिनिवत्त 0; }

        int32_t reslen = sink.सङ्ख्या_Of_Bytes_Appended();

        सचे (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            पटिनिवत्त reslen;
        }

        पटिनिवत्त u_terminateChars(buffer, capacity, reslen, &status);
    }

    /**
     * Calls a lambda that writes to a ByteSink with a CharStringByteSink and
     * then returns a CharString, in order to implement a contemporary C++ API
     * on top of a C/C++ compatibility ByteSink API.
     *
     * @param lambda that gets called with the sink as an argument
     * @param status to check and report
     * @return the resulting string, or an empty string (in case of error)
     * @internal
     */
    template <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<शून्य, F, ByteSink&, UErrorCode&>>>
    स्थिर CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        सचे (U_FAILURE(status)) { पटिनिवत्त {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        पटिनिवत्त result;
    }

गुप्त:
    स्थिर शून्य appendNonEmptyUnchanged(ध्रुव uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
