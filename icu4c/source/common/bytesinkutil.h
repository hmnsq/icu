// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#અન્તર્ભાવ <type_traits>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : જાહેર ByteSink {
જાહેર:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(અચલ CharStringByteSink&) = delete;
    CharStringByteSink& operator=(અચલ CharStringByteSink&) = delete;

    શૂન્ય Append(અચલ અક્ષર* bytes, int32_t n) override;

    અક્ષર* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          અક્ષર* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

ખાનગી:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : જાહેર CharStringByteSink {
 જાહેર:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
જાહેર:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    સ્થિર UBool appendChange(int32_t length,
                              અચલ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    સ્થિર UBool appendChange(અચલ uint8_t *s, અચલ uint8_t *limit,
                              અચલ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    સ્થિર શૂન્ય appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    સ્થિર અંતઃરેખ શૂન્ય appendCodePoint(અચલ uint8_t *src, અચલ uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    સ્થિર શૂન્ય appendTwoBytes(UChar32 c, ByteSink &sink);

    સ્થિર UBool appendUnchanged(અચલ uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) { પાછા false; }
        જો (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        પાછા true;
    }

    સ્થિર UBool appendUnchanged(અચલ uint8_t *s, અચલ uint8_t *limit,
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
                  std::is_invocable_r_v<શૂન્ય, F, ByteSink&, UErrorCode&>>>
    સ્થિર int32_t viaByteSinkToTerminatedChars(અક્ષર* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        જો (U_FAILURE(status)) { પાછા 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        જો (U_FAILURE(status)) { પાછા 0; }

        int32_t reslen = sink.સંખ્યા_Of_Bytes_Appended();

        જો (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            પાછા reslen;
        }

        પાછા u_terminateChars(buffer, capacity, reslen, &status);
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
                  std::is_invocable_r_v<શૂન્ય, F, ByteSink&, UErrorCode&>>>
    સ્થિર CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        જો (U_FAILURE(status)) { પાછા {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        પાછા result;
    }

ખાનગી:
    સ્થિર શૂન્ય appendNonEmptyUnchanged(અચલ uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
