// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#جے_بنا_تعریف BYTESINKUTIL_H
#تعریف_کرو BYTESINKUTIL_H

#ضمن <type_traits>

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestream.h"
#ضمن "unicode/edits.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : عام ByteSink {
عام:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ثابت CharStringByteSink&) = delete;
    CharStringByteSink& عامل=(ثابت CharStringByteSink&) = delete;

    فراغ Append(ثابت حرف* بائٹاں, int32_t n) override;

    حرف* Get_Append_بفر(int32_t min_سمرتھا_2,
                          int32_t desired_سمرتھا_hint,
                          حرف* scratch U_LIFETIME_BOUND,
                          int32_t scratch_سمرتھا_2,
                          int32_t* نتیجہ_سمرتھا_2) U_LIFETIME_BOUND override;

خاص:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
سانچا<>
class StringByteSink<CharString> : عام CharStringByteSink {
 عام:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
عام:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ساكن UBool appendChange(int32_t لمائی,
                              ثابت char16_t *s16, int32_t s16_لمائی,
                              ByteSink &sink, Edits *edits, UError_کوڈ &error_کوڈ);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ساكن UBool appendChange(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                              ثابت char16_t *s16, int32_t s16_لمائی,
                              ByteSink &sink, Edits *edits, UError_کوڈ &error_کوڈ);

    /** (length) bytes were mapped/changed to valid code point c. */
    ساكن فراغ append_کوڈ_نقطہ(int32_t لمائی, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ساكن مضمن فراغ append_کوڈ_نقطہ(ثابت uint8_t *src, ثابت uint8_t *اگلا_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_کوڈ_نقطہ(static_cast<int32_t>(اگلا_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ساكن فراغ append_Two_بائٹاں(UChar32 c, ByteSink &sink);

    ساكن UBool appendUnchanged(ثابت uint8_t *s, int32_t لمائی,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع false; }
        إذا (لمائی > 0) { append_Non_خالی_تھاواں_Unchanged(s, لمائی, sink, options, edits); }
        أرجع true;
    }

    ساكن UBool appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_کوڈ &error_کوڈ);

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
    سانچا <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<فراغ, F, ByteSink&, UError_کوڈ&>>>
    ساكن int32_t راہیں_Byte_Sink_منزل_Terminated_Chars(حرف* بفر, int32_t سمرتھا,
                                                F&& lambda,
                                                UError_کوڈ& حالت) {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        CheckedArrayByteSink sink(بفر, سمرتھا);
        lambda(sink, حالت);
        إذا (U_FAILURE(حالت)) { أرجع 0; }

        int32_t reslen = sink.نمبر_دا_بائٹاں_Appended();

        إذا (sink.Overflowed()) {
            حالت = U_بفر_حد_توں_ودھ_ERROR;
            أرجع reslen;
        }

        أرجع u_terminateChars(بفر, سمرتھا, reslen, &حالت);
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
    سانچا <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<فراغ, F, ByteSink&, UError_کوڈ&>>>
    ساكن CharString راہیں_Byte_Sink_منزل_Char_String(F&& lambda, UError_کوڈ& حالت) {
        إذا (U_FAILURE(حالت)) { أرجع {}; }
        CharString نتیجہ;
        CharStringByteSink sink(&نتیجہ);
        lambda(sink, حالت);
        أرجع نتیجہ;
    }

خاص:
    ساكن فراغ append_Non_خالی_تھاواں_Unchanged(ثابت uint8_t *s, int32_t لمائی,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_سرا

#جے_انت //BYTESINKUTIL_H
