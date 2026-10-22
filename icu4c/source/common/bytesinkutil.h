// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#അന്തര്ഭാവ <type_traits>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : പൊതു ByteSink {
പൊതു:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(സ്ഥിരം CharStringByteSink&) = delete;
    CharStringByteSink& operator=(സ്ഥിരം CharStringByteSink&) = delete;

    ശൂന്യം Append(സ്ഥിരം അക്ഷരം* bytes, int32_t n) override;

    അക്ഷരം* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          അക്ഷരം* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

സ്വകാര്യ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : പൊതു CharStringByteSink {
 പൊതു:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
പൊതു:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    സ്ഥിതം UBool appendChange(int32_t length,
                              സ്ഥിരം char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    സ്ഥിതം UBool appendChange(സ്ഥിരം uint8_t *s, സ്ഥിരം uint8_t *limit,
                              സ്ഥിരം char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    സ്ഥിതം ശൂന്യം appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    സ്ഥിതം വരിയിൽ ശൂന്യം appendCodePoint(സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    സ്ഥിതം ശൂന്യം appendTwoBytes(UChar32 c, ByteSink &sink);

    സ്ഥിതം UBool appendUnchanged(സ്ഥിരം uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ false; }
        എങ്കിൽ (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        തിരികെ true;
    }

    സ്ഥിതം UBool appendUnchanged(സ്ഥിരം uint8_t *s, സ്ഥിരം uint8_t *limit,
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
                  std::is_invocable_r_v<ശൂന്യം, F, ByteSink&, UErrorCode&>>>
    സ്ഥിതം int32_t viaByteSinkToTerminatedChars(അക്ഷരം* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ 0; }

        int32_t reslen = sink.സംഖ്യ_Of_Bytes_Appended();

        എങ്കിൽ (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            തിരികെ reslen;
        }

        തിരികെ u_terminateChars(buffer, capacity, reslen, &status);
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
                  std::is_invocable_r_v<ശൂന്യം, F, ByteSink&, UErrorCode&>>>
    സ്ഥിതം CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        തിരികെ result;
    }

സ്വകാര്യ:
    സ്ഥിതം ശൂന്യം appendNonEmptyUnchanged(സ്ഥിരം uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
