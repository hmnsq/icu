// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#অন্তর্ভাব <type_traits>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : সর্বজনীন ByteSink {
সর্বজনীন:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ধ্রুবক CharStringByteSink&) = delete;
    CharStringByteSink& operator=(ধ্রুবক CharStringByteSink&) = delete;

    শূন্য Append(ধ্রুবক অক্ষর* bytes, int32_t n) override;

    অক্ষর* Get_Append_বাফার(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          অক্ষর* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* ফলাফল_capacity) U_LIFETIME_BOUND override;

ব্যক্তিগত:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : সর্বজনীন CharStringByteSink {
 সর্বজনীন:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
সর্বজনীন:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    স্থির UBool appendChange(int32_t দৈর্ঘ্য,
                              ধ্রুবক char16_t *s16, int32_t s16_দৈর্ঘ্য,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    স্থির UBool appendChange(ধ্রুবক uint8_t *s, ধ্রুবক uint8_t *limit,
                              ধ্রুবক char16_t *s16, int32_t s16_দৈর্ঘ্য,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    স্থির শূন্য appendCodePoint(int32_t দৈর্ঘ্য, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    স্থির অন্তঃরেখ শূন্য appendCodePoint(ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *পরবর্তী_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(পরবর্তী_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    স্থির শূন্য appendTwoBytes(UChar32 c, ByteSink &sink);

    স্থির UBool appendUnchanged(ধ্রুবক uint8_t *s, int32_t দৈর্ঘ্য,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত false; }
        যদি (দৈর্ঘ্য > 0) { appendNonEmptyUnchanged(s, দৈর্ঘ্য, sink, options, edits); }
        ফেরত true;
    }

    স্থির UBool appendUnchanged(ধ্রুবক uint8_t *s, ধ্রুবক uint8_t *limit,
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
                  std::is_invocable_r_v<শূন্য, F, ByteSink&, UErrorCode&>>>
    স্থির int32_t viaByteSinkToTerminatedChars(অক্ষর* বাফার, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        যদি (U_FAILURE(status)) { ফেরত 0; }
        CheckedArrayByteSink sink(বাফার, capacity);
        lambda(sink, status);
        যদি (U_FAILURE(status)) { ফেরত 0; }

        int32_t reslen = sink.সংখ্যা_Of_Bytes_Appended();

        যদি (sink.Overflowed()) {
            status = U_বাফার_OVERFLOW_ERROR;
            ফেরত reslen;
        }

        ফেরত u_terminateChars(বাফার, capacity, reslen, &status);
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
                  std::is_invocable_r_v<শূন্য, F, ByteSink&, UErrorCode&>>>
    স্থির CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        যদি (U_FAILURE(status)) { ফেরত {}; }
        CharString ফলাফল;
        CharStringByteSink sink(&ফলাফল);
        lambda(sink, status);
        ফেরত ফলাফল;
    }

ব্যক্তিগত:
    স্থির শূন্য appendNonEmptyUnchanged(ধ্রুবক uint8_t *s, int32_t দৈর্ঘ্য,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
