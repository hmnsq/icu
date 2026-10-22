// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BYTESINKUTIL_H
#ವ್ಯಾಖ್ಯಾನಿಸು BYTESINKUTIL_H

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
    CharStringByteSink(ಸ್ಥಿರಾಂಕ CharStringByteSink&) = delete;
    CharStringByteSink& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ CharStringByteSink&) = delete;

    ಶೂನ್ಯ Append(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬೈಟುಗಳು, int32_t n) override;

    ಅಕ್ಷರ* Get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ_2,
                          int32_t desired_ಸಾಮರ್ಥ್ಯ_hint,
                          ಅಕ್ಷರ* scratch U_LIFETIME_BOUND,
                          int32_t scratch_ಸಾಮರ್ಥ್ಯ_2,
                          int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2) U_LIFETIME_BOUND override;

ಖಾಸಗಿ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
ಮಾದರಿ<>
class StringByteSink<CharString> : ಸಾರ್ವಜನಿಕ CharStringByteSink {
 ಸಾರ್ವಜನಿಕ:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
ಸಾರ್ವಜನಿಕ:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ಸ್ಥಿರ UBool appendChange(int32_t ಉದ್ದಳತೆ,
                              ಸ್ಥಿರಾಂಕ char16_t *s16, int32_t s16_ಉದ್ದಳತೆ,
                              ByteSink &sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ಸ್ಥಿರ UBool appendChange(ಸ್ಥಿರಾಂಕ uint8_t *s, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                              ಸ್ಥಿರಾಂಕ char16_t *s16, int32_t s16_ಉದ್ದಳತೆ,
                              ByteSink &sink, Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /** (length) bytes were mapped/changed to valid code point c. */
    ಸ್ಥಿರ ಶೂನ್ಯ append_ಸಂಕೇತ_ಬಿಂದು(int32_t ಉದ್ದಳತೆ, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ append_ಸಂಕೇತ_ಬಿಂದು(ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಮುಂದಿನ_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_ಸಂಕೇತ_ಬಿಂದು(static_cast<int32_t>(ಮುಂದಿನ_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ಸ್ಥಿರ ಶೂನ್ಯ append_Two_ಬೈಟುಗಳು(UChar32 c, ByteSink &sink);

    ಸ್ಥಿರ UBool appendUnchanged(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಉದ್ದಳತೆ,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ false; }
        ಆದರೆ (ಉದ್ದಳತೆ > 0) { append_Non_ಖಾಲಿಗಳು_Unchanged(s, ಉದ್ದಳತೆ, sink, options, edits); }
        ಹಿಂತಿರುಗಿ true;
    }

    ಸ್ಥಿರ UBool appendUnchanged(ಸ್ಥಿರಾಂಕ uint8_t *s, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

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
    ಮಾದರಿ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<ಶೂನ್ಯ, F, ByteSink&, UError_ಸಂಕೇತ&>>>
    ಸ್ಥಿರ int32_t ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(ಅಕ್ಷರ* ಬಫರ್, int32_t ಸಾಮರ್ಥ್ಯ,
                                                F&& lambda,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
        CheckedArrayByteSink sink(ಬಫರ್, ಸಾಮರ್ಥ್ಯ);
        lambda(sink, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }

        int32_t reslen = sink.ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು_Appended();

        ಆದರೆ (sink.Overflowed()) {
            ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ reslen;
        }

        ಹಿಂತಿರುಗಿ u_terminateChars(ಬಫರ್, ಸಾಮರ್ಥ್ಯ, reslen, &ಸ್ಥಿತಿ_2);
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
    ಮಾದರಿ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<ಶೂನ್ಯ, F, ByteSink&, UError_ಸಂಕೇತ&>>>
    ಸ್ಥಿರ CharString ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(F&& lambda, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ {}; }
        CharString ಫಲಿತಾಂಶ;
        CharStringByteSink sink(&ಫಲಿತಾಂಶ);
        lambda(sink, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರ ಶೂನ್ಯ append_Non_ಖಾಲಿಗಳು_Unchanged(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಉದ್ದಳತೆ,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ //BYTESINKUTIL_H
