// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BYTESINKUTIL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BYTESINKUTIL_H

#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : ਜਨਤਕ ByteSink {
ਜਨਤਕ:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ਅਚਲ CharStringByteSink&) = delete;
    CharStringByteSink& ਸੰਚਾਲਕ=(ਅਚਲ CharStringByteSink&) = delete;

    ਖਾਲੀ Append(ਅਚਲ ਅੱਖਰ* ਬਾਈਟਾਂ, int32_t n) override;

    ਅੱਖਰ* Get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ_2,
                          int32_t desired_ਸਮਰੱਥਾ_hint,
                          ਅੱਖਰ* scratch U_LIFETIME_BOUND,
                          int32_t scratch_ਸਮਰੱਥਾ_2,
                          int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ_2) U_LIFETIME_BOUND override;

ਨਿਜੀ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
ਸਾਂਚਾ<>
class StringByteSink<CharString> : ਜਨਤਕ CharStringByteSink {
 ਜਨਤਕ:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
ਜਨਤਕ:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ਸਥਿਰ UBool appendChange(int32_t ਲੰਮਾਈ,
                              ਅਚਲ char16_t *s16, int32_t s16_ਲੰਮਾਈ,
                              ByteSink &sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ਸਥਿਰ UBool appendChange(ਅਚਲ uint8_t *s, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                              ਅਚਲ char16_t *s16, int32_t s16_ਲੰਮਾਈ,
                              ByteSink &sink, Edits *edits, UError_ਕੋਡ &error_ਕੋਡ);

    /** (length) bytes were mapped/changed to valid code point c. */
    ਸਥਿਰ ਖਾਲੀ append_ਕੋਡ_ਬਿੰਦੂ(int32_t ਲੰਮਾਈ, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ append_ਕੋਡ_ਬਿੰਦੂ(ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਅਗਲਾ_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_ਕੋਡ_ਬਿੰਦੂ(static_cast<int32_t>(ਅਗਲਾ_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ਸਥਿਰ ਖਾਲੀ append_Two_ਬਾਈਟਾਂ(UChar32 c, ByteSink &sink);

    ਸਥਿਰ UBool appendUnchanged(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ false; }
        ਜੇ (ਲੰਮਾਈ > 0) { append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(s, ਲੰਮਾਈ, sink, options, edits); }
        ਵਾਪਸ true;
    }

    ਸਥਿਰ UBool appendUnchanged(ਅਚਲ uint8_t *s, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_ਕੋਡ &error_ਕੋਡ);

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
    ਸਾਂਚਾ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<ਖਾਲੀ, F, ByteSink&, UError_ਕੋਡ&>>>
    ਸਥਿਰ int32_t ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(ਅੱਖਰ* ਬਫਰ, int32_t ਸਮਰੱਥਾ,
                                                F&& lambda,
                                                UError_ਕੋਡ& ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
        CheckedArrayByteSink sink(ਬਫਰ, ਸਮਰੱਥਾ);
        lambda(sink, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }

        int32_t reslen = sink.ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ_Appended();

        ਜੇ (sink.Overflowed()) {
            ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ reslen;
        }

        ਵਾਪਸ u_terminateChars(ਬਫਰ, ਸਮਰੱਥਾ, reslen, &ਸਥਿਤੀ);
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
    ਸਾਂਚਾ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<ਖਾਲੀ, F, ByteSink&, UError_ਕੋਡ&>>>
    ਸਥਿਰ CharString ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(F&& lambda, UError_ਕੋਡ& ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ {}; }
        CharString ਨਤੀਜਾ;
        CharStringByteSink sink(&ਨਤੀਜਾ);
        lambda(sink, ਸਥਿਤੀ);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    ਸਥਿਰ ਖਾਲੀ append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ //BYTESINKUTIL_H
