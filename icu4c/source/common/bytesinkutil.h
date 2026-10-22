// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#ifndef BYTESINKUTIL_H
#define BYTESINKUTIL_H

#ଅନ୍ତର୍ଭାଵ <type_traits>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : ସର୍ବସାଧାରଣ ByteSink {
ସର୍ବସାଧାରଣ:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ଅଚଳ CharStringByteSink&) = delete;
    CharStringByteSink& operator=(ଅଚଳ CharStringByteSink&) = delete;

    ଶୂନ୍ୟ Append(ଅଚଳ ଅକ୍ଷର* bytes, int32_t n) override;

    ଅକ୍ଷର* GetAppendBuffer(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          ଅକ୍ଷର* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* result_capacity) U_LIFETIME_BOUND override;

ବ୍ୟକ୍ତିଗତ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
template<>
class StringByteSink<CharString> : ସର୍ବସାଧାରଣ CharStringByteSink {
 ସର୍ବସାଧାରଣ:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
ସର୍ବସାଧାରଣ:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    ସ୍ଥିର UBool appendChange(int32_t length,
                              ଅଚଳ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    ସ୍ଥିର UBool appendChange(ଅଚଳ uint8_t *s, ଅଚଳ uint8_t *limit,
                              ଅଚଳ char16_t *s16, int32_t s16Length,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    ସ୍ଥିର ଶୂନ୍ୟ appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ appendCodePoint(ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *nextSrc, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(nextSrc - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    ସ୍ଥିର ଶୂନ୍ୟ appendTwoBytes(UChar32 c, ByteSink &sink);

    ସ୍ଥିର UBool appendUnchanged(ଅଚଳ uint8_t *s, int32_t length,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ false; }
        ଯଦି (length > 0) { appendNonEmptyUnchanged(s, length, sink, options, edits); }
        ଫେରନ୍ତୁ true;
    }

    ସ୍ଥିର UBool appendUnchanged(ଅଚଳ uint8_t *s, ଅଚଳ uint8_t *limit,
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
                  std::is_invocable_r_v<ଶୂନ୍ୟ, F, ByteSink&, UErrorCode&>>>
    ସ୍ଥିର int32_t viaByteSinkToTerminatedChars(ଅକ୍ଷର* buffer, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }
        CheckedArrayByteSink sink(buffer, capacity);
        lambda(sink, status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }

        int32_t reslen = sink.ସଂଖ୍ୟା_Of_Bytes_Appended();

        ଯଦି (sink.Overflowed()) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ଫେରନ୍ତୁ reslen;
        }

        ଫେରନ୍ତୁ u_terminateChars(buffer, capacity, reslen, &status);
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
                  std::is_invocable_r_v<ଶୂନ୍ୟ, F, ByteSink&, UErrorCode&>>>
    ସ୍ଥିର CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ {}; }
        CharString result;
        CharStringByteSink sink(&result);
        lambda(sink, status);
        ଫେରନ୍ତୁ result;
    }

ବ୍ୟକ୍ତିଗତ:
    ସ୍ଥିର ଶୂନ୍ୟ appendNonEmptyUnchanged(ଅଚଳ uint8_t *s, int32_t length,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#endif //BYTESINKUTIL_H
