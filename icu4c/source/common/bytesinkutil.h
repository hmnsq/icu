// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.h
// created: 2017sep14 Markus W. Scherer

#నిర్వచితం_కాకపోతే BYTESINKUTIL_H
#నిర్వచించు BYTESINKUTIL_H

#అన్తర్భావ <type_traits>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

class ByteSink;
class Edits;

class U_COMMON_API CharStringByteSink : బహిరంగ ByteSink {
బహిరంగ:
    CharStringByteSink(CharString* dest);
    ~CharStringByteSink() override;

    CharStringByteSink() = delete;
    CharStringByteSink(స్థిరరాశి CharStringByteSink&) = delete;
    CharStringByteSink& క్రియాకారకం=(స్థిరరాశి CharStringByteSink&) = delete;

    శూన్య Append(స్థిరరాశి అక్షరం* బైట్లు, int32_t n) override;

    అక్షరం* Get_Append_బఫరు(int32_t min_సామర్థ్యం_2,
                          int32_t desired_సామర్థ్యం_hint,
                          అక్షరం* scratch U_LIFETIME_BOUND,
                          int32_t scratch_సామర్థ్యం_2,
                          int32_t* ఫలితం_సామర్థ్యం_2) U_LIFETIME_BOUND override;

అంతరంగ:
    CharString& dest_;
};

// CharString doesn't provide the public API that StringByteSink requires a
// string class to have so this template specialization replaces the default
// implementation of StringByteSink<CharString> with CharStringByteSink.
మూస<>
class StringByteSink<CharString> : బహిరంగ CharStringByteSink {
 బహిరంగ:
  StringByteSink(CharString* dest) : CharStringByteSink(dest) { }
  StringByteSink(CharString* dest, int32_t /*initialAppendCapacity*/) : CharStringByteSink(dest) { }
};

class U_COMMON_API ByteSinkUtil {
బహిరంగ:
    ByteSinkUtil() = delete;  // all static

    /** (length) bytes were mapped to valid (s16, s16Length). */
    స్థిర UBool appendChange(int32_t నిడివి,
                              స్థిరరాశి char16_t *s16, int32_t s16_నిడివి,
                              ByteSink &sink, Edits *edits, UError_సంకేతం &error_సంకేతం);

    /** The bytes at [s, limit[ were mapped to valid (s16, s16Length). */
    స్థిర UBool appendChange(స్థిరరాశి uint8_t *s, స్థిరరాశి uint8_t *హద్దు,
                              స్థిరరాశి char16_t *s16, int32_t s16_నిడివి,
                              ByteSink &sink, Edits *edits, UError_సంకేతం &error_సంకేతం);

    /** (length) bytes were mapped/changed to valid code point c. */
    స్థిర శూన్య append_సంకేతం_బిందువు(int32_t నిడివి, UChar32 c, ByteSink &sink, Edits *edits = nullptr);

    /** The few bytes at [src, nextSrc[ were mapped/changed to valid code point c. */
    స్థిర అంతఃవరుస శూన్య append_సంకేతం_బిందువు(స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *తదుపరి_Src, UChar32 c,
                                       ByteSink &sink, Edits *edits = nullptr) {
        append_సంకేతం_బిందువు(static_cast<int32_t>(తదుపరి_Src - src), c, sink, edits);
    }

    /** Append the two-byte character (U+0080..U+07FF). */
    స్థిర శూన్య append_Two_బైట్లు(UChar32 c, ByteSink &sink);

    స్థిర UBool appendUnchanged(స్థిరరాశి uint8_t *s, int32_t నిడివి,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి false; }
        ఒకవేళ (నిడివి > 0) { append_Non_ఖాళీలు_Unchanged(s, నిడివి, sink, options, edits); }
        తిరిగి true;
    }

    స్థిర UBool appendUnchanged(స్థిరరాశి uint8_t *s, స్థిరరాశి uint8_t *హద్దు,
                                 ByteSink &sink, uint32_t options, Edits *edits,
                                 UError_సంకేతం &error_సంకేతం);

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
    మూస <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<శూన్య, F, ByteSink&, UError_సంకేతం&>>>
    స్థిర int32_t ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(అక్షరం* బఫరు, int32_t సామర్థ్యం,
                                                F&& lambda,
                                                UError_సంకేతం& స్థితి_2) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }
        CheckedArrayByteSink sink(బఫరు, సామర్థ్యం);
        lambda(sink, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }

        int32_t reslen = sink.సంఖ్య_యొక్క_బైట్లు_Appended();

        ఒకవేళ (sink.Overflowed()) {
            స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి reslen;
        }

        తిరిగి u_terminateChars(బఫరు, సామర్థ్యం, reslen, &స్థితి_2);
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
    మూస <typename F,
              typename = std::enable_if_t<
                  std::is_invocable_r_v<శూన్య, F, ByteSink&, UError_సంకేతం&>>>
    స్థిర CharString ద్వారా_Byte_Sink_గమ్యం_Char_String(F&& lambda, UError_సంకేతం& స్థితి_2) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి {}; }
        CharString ఫలితం;
        CharStringByteSink sink(&ఫలితం);
        lambda(sink, స్థితి_2);
        తిరిగి ఫలితం;
    }

అంతరంగ:
    స్థిర శూన్య append_Non_ఖాళీలు_Unchanged(స్థిరరాశి uint8_t *s, int32_t నిడివి,
                                        ByteSink &sink, uint32_t options, Edits *edits);
};

U_NAMESPACE_చివర

#అయితే_ముగింపు //BYTESINKUTIL_H
