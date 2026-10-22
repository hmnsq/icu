// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#اگر_تعریف_نشده BYTESINKUTIL_H
#تعریف_کن BYTESINKUTIL_H

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
    CharStringByteSink& عملگر=(ثابت CharStringByteSink&) = delete;

    فراغ Append(ثابت حرف* بایتها, int32_t n) override;

    حرف* Get_Append_میانگیر(int32_t min_ظرفیت_2,
                          int32_t desired_ظرفیت_hint,
                          حرف* scratch U_LIFETIME_BOUND,
                          int32_t scratch_ظرفیت_2,
                          int32_t* نتیجه_ظرفیت_2) U_LIFETIME_BOUND override;

خاص:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
قالب<>
class StringByteSink<CharString> : عام CharStringByteSink {
 عام:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
عام:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ساكن UBool appendChange(int32_t درازی,
                              ثابت char16_t *s16, int32_t s16_درازی,
                              ByteSink &sink, Edits *edits, UError_کد &error_کد);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ساكن UBool appendChange(ثابت uint8_t *s, ثابت uint8_t *حد,
                              ثابت char16_t *s16, int32_t s16_درازی,
                              ByteSink &sink, Edits *edits, UError_کد &error_کد);

    /** (length) bytes were mapped/changed to valid code point c. */
    ساكن فراغ append_کد_نقطه(int32_t درازی, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ساكن مضمن فراغ append_کد_نقطه(ثابت uint8_t *src, ثابت uint8_t *بعدی_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_کد_نقطه(static_cast<int32_t>(بعدی_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ساكن فراغ append_Two_بایتها(UChar32 c, ByteSink &sink);

    ساكن UBool appendUnchanged(ثابت uint8_t *s, int32_t درازی,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع false; }
        إذا (درازی > 0) { append_Non_خالی_Unchanged(s, درازی, sink, options, edits); }
        أرجع true;
    }

    ساكن UBool appendUnchanged(ثابت uint8_t *s, ثابت uint8_t *حد,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_کد &error_کد);

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
    قالب <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<فراغ, F, ByteSink&, UError_کد&>>>
    ساكن int32_t از_طریق_Byte_Sink_مقصد_Terminated_Chars(حرف* میانگیر, int32_t ظرفیت,
                                                F&& lambda,
                                                UError_کد& وضعیت) {
        إذا (U_FAILURE(وضعیت)) { أرجع 0; }
        CheckedArrayByteSink sink(میانگیر, ظرفیت);
        lambda(sink, وضعیت);
        إذا (U_FAILURE(وضعیت)) { أرجع 0; }

        int32_t reslen = sink.عدد_از_بایتها_Appended();

        إذا (sink.Overflowed()) {
            وضعیت = U_میانگیر_سرریز_ERROR;
            أرجع reslen;
        }

        أرجع u_terminateChars(میانگیر, ظرفیت, reslen, &وضعیت);
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
    قالب <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<فراغ, F, ByteSink&, UError_کد&>>>
    ساكن CharString از_طریق_Byte_Sink_مقصد_Char_String(F&& lambda, UError_کد& وضعیت) {
        إذا (U_FAILURE(وضعیت)) { أرجع {}; }
        CharString نتیجه;
        CharStringByteSink sink(&نتیجه);
        lambda(sink, وضعیت);
        أرجع نتیجه;
    }

خاص:
    ساكن فراغ append_Non_خالی_Unchanged(ثابت uint8_t *s, int32_t درازی,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_انتها

#پایان_اگر //BYTESINKUTIL_H
