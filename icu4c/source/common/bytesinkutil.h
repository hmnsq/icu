// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : ಸಾರ್ವಜನಿಕ ByteSink {
ಸಾರ್ವಜನಿಕ:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ಧ್ರುವ CharStringByteSink&) = delete;
    CharStringByteSink& operator=(ಧ್ರುವ CharStringByteSink&) = delete;

    ಶೂನ್ಯ Append(ಧ್ರುವ ವರ್ಣ* bytes, int32_t n) override;

    ವರ್ಣ* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          ವರ್ಣ* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

ಗುಪ್ತ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : ಸಾರ್ವಜನಿಕ CharStringByteSink {
 ಸಾರ್ವಜನಿಕ:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
ಸಾರ್ವಜನಿಕ:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ಸ್ಥಿರ UBool appendChange(int32_t length,
                              ಧ್ರುವ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ಸ್ಥಿರ UBool appendChange(ಧ್ರುವ uint8_t *s, ಧ್ರುವ uint8_t *limit,
                              ಧ್ರುವ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    ಸ್ಥಿರ ಶೂನ್ಯ appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ appendCodePoint(ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ಸ್ಥಿರ ಶೂನ್ಯ appendTwoBytes(UChar32 c, ByteSink &sink);

    ಸ್ಥಿರ UBool appendUnchanged(ಧ್ರುವ uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ false; }
        ಯದಿ (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        ಪ್ರತ್ಯಾಗಮ true;
    }

    ಸ್ಥಿರ UBool appendUnchanged(ಧ್ರುವ uint8_t *s, ಧ್ರುವ uint8_t *limit,
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
                  std::is_invocable_r_v<ಶೂನ್ಯ, F, ByteSink&, UErrorCode&>>>
    ಸ್ಥಿರ int32_t viaByteSinkToTerminatedChars(ವರ್ಣ* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }

        int32_t reslen = sink.ಸಂಖ್ಯೆ_Of_Bytes_Appended();

        ಯದಿ (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ಪ್ರತ್ಯಾಗಮ reslen;
        }

        ಪ್ರತ್ಯಾಗಮ u_terminateChars(buffer, capacity, reslen, &status);
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
                  std::is_invocable_r_v<ಶೂನ್ಯ, F, ByteSink&, UErrorCode&>>>
    ಸ್ಥಿರ CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        ಪ್ರತ್ಯಾಗಮ result;
    }

ಗುಪ್ತ:
    ಸ್ಥಿರ ಶೂನ್ಯ appendNonEmptyUnchanged(ಧ್ರುವ uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
