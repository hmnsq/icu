// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#اگر_غیر_تعریف_شدہ BYTESINKUTIL_H
#تعریف_کرو BYTESINKUTIL_H

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

class U_COMMON_API CharStringByteSink : عوامی ByteSink {
عوامی:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ثابت CharStringByteSink&) = delete;
    CharStringByteSink& عامل=(ثابت CharStringByteSink&) = delete;

    خالی Append(ثابت حرف* بائٹیں, int32_t n) override;

    حرف* Get_Append_بفر(int32_t min_استعداد_2,
                          int32_t desired_استعداد_hint,
                          حرف* scratch U_LIFETIME_BOUND,
                          int32_t scratch_استعداد_2,
                          int32_t* نتیجہ_استعداد_2) U_LIFETIME_BOUND override;

نجی:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
سانچہ<>
class StringByteSink<CharString> : عوامی CharStringByteSink {
 عوامی:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
عوامی:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ساکن UBool appendChange(int32_t لمبائی,
                              ثابت char16_t *s16, int32_t s16_لمبائی,
                              ByteSink &sink, Edits *edits, UError_رمز &error_رمز);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ساکن UBool appendChange(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                              ثابت char16_t *s16, int32_t s16_لمبائی,
                              ByteSink &sink, Edits *edits, UError_رمز &error_رمز);

    /** (length) bytes were mapped/changed to valid code point c. */
    ساکن خالی append_رمز_نقطہ(int32_t لمبائی, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ساکن اندرونی خالی append_رمز_نقطہ(ثابت uint8_t *src, ثابت uint8_t *اگلا_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_رمز_نقطہ(static_cast<int32_t>(اگلا_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ساکن خالی append_Two_بائٹیں(UChar32 c, ByteSink &sink);

    ساکن UBool appendUnchanged(ثابت uint8_t *s, int32_t لمبائی,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس false; }
        اگر (لمبائی > 0) { append_Non_خالی_خانے_Unchanged(s, لمبائی, sink, options, edits); }
        واپس true;
    }

    ساکن UBool appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد_بندی,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_رمز &error_رمز);

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
    سانچہ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<خالی, F, ByteSink&, UError_رمز&>>>
    ساکن int32_t بذریعہ_Byte_Sink_مقصد_Terminated_Chars(حرف* بفر, int32_t استعداد,
                                                F&& lambda,
                                                UError_رمز& حالت_2) {
        اگر (U_FAILURE(حالت_2)) { واپس 0; }
        CheckedArrayByteSink sink(بفر, استعداد);
        lambda(sink, حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس 0; }

        int32_t reslen = sink.نمبر_کا_بائٹیں_Appended();

        اگر (sink.Overflowed()) {
            حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
            واپس reslen;
        }

        واپس u_terminateChars(بفر, استعداد, reslen, &حالت_2);
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
    سانچہ <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<خالی, F, ByteSink&, UError_رمز&>>>
    ساکن CharString بذریعہ_Byte_Sink_مقصد_Char_String(F&& lambda, UError_رمز& حالت_2) {
        اگر (U_FAILURE(حالت_2)) { واپس {}; }
        CharString نتیجہ;
        CharStringByteSink sink(&نتیجہ);
        lambda(sink, حالت_2);
        واپس نتیجہ;
    }

نجی:
    ساکن خالی append_Non_خالی_خانے_Unchanged(ثابت uint8_t *s, int32_t لمبائی,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_انتہا

#اگر_ختم //BYTESINKUTIL_H
