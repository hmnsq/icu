// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#شامل <type_traits>

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "unicode/edits.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : عمومی ByteSink {
عمومی:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ثابت CharStringByteSink&) = delete;
    CharStringByteSink& operator=(ثابت CharStringByteSink&) = delete;

    تهی Append(ثابت نویسه* bytes, int32_t n) override;

    نویسه* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          نویسه* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

خصوصی:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : عمومی CharStringByteSink {
 عمومی:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
عمومی:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ایستا UBool appendChange(int32_t length,
                              ثابت char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ایستا UBool appendChange(ثابت uint8_t *s, ثابت uint8_t *limit,
                              ثابت char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    ایستا تهی appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ایستا درونخطی تهی appendCodePoint(ثابت uint8_t *src, ثابت uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ایستا تهی appendTwoBytes(UChar32 c, ByteSink &sink);

    ایستا UBool appendUnchanged(ثابت uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت false; }
        اگر (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        بازگشت true;
    }

    ایستا UBool appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *limit,
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
                  std::is_invocable_r_v<تهی, F, ByteSink&, UErrorCode&>>>
    ایستا int32_t viaByteSinkToTerminatedChars(نویسه* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        اگر (U_FAILURE(status)) { بازگشت 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        اگر (U_FAILURE(status)) { بازگشت 0; }

        int32_t reslen = sink.عدد_Of_Bytes_Appended();

        اگر (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            بازگشت reslen;
        }

        بازگشت u_terminateChars(buffer, capacity, reslen, &status);
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
                  std::is_invocable_r_v<تهی, F, ByteSink&, UErrorCode&>>>
    ایستا CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        اگر (U_FAILURE(status)) { بازگشت {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        بازگشت result;
    }

خصوصی:
    ایستا تهی appendNonEmptyUnchanged(ثابت uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
