// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#यदि_अपरिभाषितम् BYTESINKUTIL_H
#परिभाषय BYTESINKUTIL_H

#अन्तर्भाव <type_traits>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : सार्वजनिक ByteSink {
सार्वजनिक:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(ध्रुव CharStringByteSink&) = delete;
    CharStringByteSink& संक्रियकः=(ध्रुव CharStringByteSink&) = delete;

    शून्य Append(ध्रुव वर्ण* अष्टकानि, int32_t n) override;

    वर्ण* Get_Append_बफर(int32_t min_क्षमता_2,
                          int32_t desired_क्षमता_hint,
                          वर्ण* scratch U_LIFETIME_BOUND,
                          int32_t scratch_क्षमता_2,
                          int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND override;

गुप्त:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
प्रतिरूपम्<>
class StringByteSink<CharString> : सार्वजनिक CharStringByteSink {
 सार्वजनिक:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
सार्वजनिक:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    स्थिर UBool appendChange(int32_t दीर्घता,
                              ध्रुव char16_t *s16, int32_t s16_दीर्घता,
                              ByteSink &sink, Edits *edits, UError_संहिता &error_संहिता);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    स्थिर UBool appendChange(ध्रुव uint8_t *s, ध्रुव uint8_t *सीमा,
                              ध्रुव char16_t *s16, int32_t s16_दीर्घता,
                              ByteSink &sink, Edits *edits, UError_संहिता &error_संहिता);

    /** (length) bytes were mapped/changed to valid code point c. */
    स्थिर शून्य append_संहिता_Point(int32_t दीर्घता, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    स्थिर अन्तःस्थ शून्य append_संहिता_Point(ध्रुव uint8_t *src, ध्रुव uint8_t *अग्र_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_संहिता_Point(static_cast<int32_t>(अग्र_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    स्थिर शून्य append_Two_अष्टकानि(UChar32 c, ByteSink &sink);

    स्थिर UBool appendUnchanged(ध्रुव uint8_t *s, int32_t दीर्घता,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम false; }
        यदि (दीर्घता > 0) { append_Non_रिक्तानि_Unchanged(s, दीर्घता, sink, options, edits); }
        प्रत्यागम true;
    }

    स्थिर UBool appendUnchanged(ध्रुव uint8_t *s, ध्रुव uint8_t *सीमा,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_संहिता &error_संहिता);

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
    प्रतिरूपम् <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<शून्य, F, ByteSink&, UError_संहिता&>>>
    स्थिर int32_t द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(वर्ण* बफर, int32_t क्षमता,
                                                F&& lambda,
                                                UError_संहिता& स्थितिः) {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }
        CheckedArrayByteSink sink(बफर, क्षमता);
        lambda(sink, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }

        int32_t reslen = sink.सङ्ख्या_सम्बन्धि_अष्टकानि_Appended();

        यदि (sink.Overflowed()) {
            स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम reslen;
        }

        प्रत्यागम u_terminateChars(बफर, क्षमता, reslen, &स्थितिः);
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
    प्रतिरूपम् <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<शून्य, F, ByteSink&, UError_संहिता&>>>
    स्थिर CharString द्वारा_Byte_Sink_गन्तव्यम्_Char_String(F&& lambda, UError_संहिता& स्थितिः) {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम {}; }
        CharString परिणाम;
        CharStringByteSink sink(&परिणाम);
        lambda(sink, स्थितिः);
        प्रत्यागम परिणाम;
    }

गुप्त:
    स्थिर शून्य append_Non_रिक्तानि_Unchanged(ध्रुव uint8_t *s, int32_t दीर्घता,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_अन्तः

#यदि_अन्तः //BYTESINKUTIL_H
