// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#अगर_अघोषित BYTESINKUTIL_H
#घोषणा BYTESINKUTIL_H

#समावेश <type_traits>

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestream.h"
#समावेश "unicode/edits.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : खुला ByteSink {
खुला:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(स्थिर CharStringByteSink&) = delete;
    CharStringByteSink& चालक=(स्थिर CharStringByteSink&) = delete;

    व्योम Append(स्थिर अक्षर* bytes, int32_t n) override;

    अक्षर* Get_Append_बफर(int32_t min_capacity,
                          int32_t desired_capacity_hint,
                          अक्षर* scratch U_LIFETIME_BOUND,
                          int32_t scratch_capacity,
                          int32_t* परिणाम_capacity) U_LIFETIME_BOUND override;

निजी:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
ढाँचा<>
class StringByteSink<CharString> : खुला CharStringByteSink {
 खुला:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
खुला:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    अटल UBool appendChange(int32_t लंबाई,
                              स्थिर char16_t *s16, int32_t s16_लंबाई,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    अटल UBool appendChange(स्थिर uint8_t *s, स्थिर uint8_t *limit,
                              स्थिर char16_t *s16, int32_t s16_लंबाई,
                              ByteSink &sink, Edits *edits, UErrorCode &errorCode);

    /** (length) bytes were mapped/changed to valid code point c. */
    अटल व्योम appendCodePoint(int32_t लंबाई, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    अटल अंतरभूत व्योम appendCodePoint(स्थिर uint8_t *src, स्थिर uint8_t *अग्र_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        appendCodePoint(static_cast<int32_t>(अग्र_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    अटल व्योम appendTwoBytes(UChar32 c, ByteSink &sink);

    अटल UBool appendUnchanged(स्थिर uint8_t *s, int32_t लंबाई,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस false; }
        अगर (लंबाई > 0) { appendNonEmptyUnchanged(s, लंबाई, sink, options, edits); }
        वापस true;
    }

    अटल UBool appendUnchanged(स्थिर uint8_t *s, स्थिर uint8_t *limit,
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
    ढाँचा <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<व्योम, F, ByteSink&, UErrorCode&>>>
    अटल int32_t viaByteSinkToTerminatedChars(अक्षर* बफर, int32_t capacity,
                                                F&& lambda,
                                                UErrorCode& status) {
        अगर (U_FAILURE(status)) { वापस 0; }
        CheckedArrayByteSink sink(बफर, capacity);
        lambda(sink, status);
        अगर (U_FAILURE(status)) { वापस 0; }

        int32_t reslen = sink.संख्या_Of_Bytes_Appended();

        अगर (sink.Overflowed()) {
            status = U_बफर_OVERFLOW_ERROR;
            वापस reslen;
        }

        वापस u_terminateChars(बफर, capacity, reslen, &status);
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
    ढाँचा <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<व्योम, F, ByteSink&, UErrorCode&>>>
    अटल CharString viaByteSinkToCharString(F&& lambda, UErrorCode& status) {
        अगर (U_FAILURE(status)) { वापस {}; }
        CharString परिणाम;
        CharStringByteSink sink(&परिणाम);
        lambda(sink, status);
        वापस परिणाम;
    }

निजी:
    अटल व्योम appendNonEmptyUnchanged(स्थिर uint8_t *s, int32_t लंबाई,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_END

#पूर्ण_अगर //BYTESINKUTIL_H
