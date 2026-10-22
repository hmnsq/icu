// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, ध्रुव char16_t *s16, int32_t s16_दीर्घता,
                           ByteSink &sink, Edits *edits, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    वर्ण scratch[200];
    int32_t s8_दीर्घता = 0;
    प्रति (int32_t i = 0; i < s16_दीर्घता;) {
        int32_t capacity;
        int32_t desired_क्षमता = s16_दीर्घता - i;
        यदि (desired_क्षमता < (INT32_MAX / 3)) {
            desired_क्षमता *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } अन्यथा यदि (desired_क्षमता < (INT32_MAX / 2)) {
            desired_क्षमता *= 2;
        } अन्यथा {
            desired_क्षमता = INT32_MAX;
        }
        वर्ण *बफर = sink.Get_Append_बफर(U8_MAX_दीर्घता, desired_क्षमता,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_दीर्घता - 1;
        int32_t j = 0;
        प्रति (; i < s16_दीर्घता && j < capacity;) {
            UChar32 c;
            U16_अग्र_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(बफर, j, c);
        }
        यदि (j > (INT32_MAX - s8_दीर्घता)) {
            error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम असत्य;
        }
        sink.Append(बफर, j);
        s8_दीर्घता += j;
    }
    यदि (edits != शून्यसूचकः) {
        edits->सङ्कलय_Replace(length, s8_दीर्घता);
    }
    प्रत्यागम सत्य;
}

UBool
ByteSinkUtil::appendChange(ध्रुव uint8_t *s, ध्रुव uint8_t *सीमा,
                           ध्रुव char16_t *s16, int32_t s16_दीर्घता,
                           ByteSink &sink, Edits *edits, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    यदि ((सीमा - s) > INT32_MAX) {
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    }
    प्रत्यागम appendChange(static_cast<int32_t>(सीमा - s), s16, s16_दीर्घता, sink, edits, error_संहिता);
}

शून्य
ByteSinkUtil::append_संहिता_Point(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    वर्ण s8[U8_MAX_दीर्घता];
    int32_t s8_दीर्घता = 0;
    U8_APPEND_UNSAFE(s8, s8_दीर्घता, c);
    यदि (edits != शून्यसूचकः) {
        edits->सङ्कलय_Replace(length, s8_दीर्घता);
    }
    sink.Append(s8, s8_दीर्घता);
}

नामाकाश {

// See unicode/utf8.h U8_APPEND_UNSAFE().
अन्तःस्थ uint8_t getTwoByteLead(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c >> 6) | 0xc0); }
अन्तःस्थ uint8_t getTwoByteTrail(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

शून्य
ByteSinkUtil::append_Two_अष्टकानि(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    वर्ण s8[2] = {static_cast<वर्ण>(getTwoByteLead(c)), static_cast<वर्ण>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

शून्य
ByteSinkUtil::append_Non_रिक्तानि_Unchanged(ध्रुव uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    यदि (edits != शून्यसूचकः) {
        edits->सङ्कलय_Unchanged(length);
    }
    यदि ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<ध्रुव वर्ण *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(ध्रुव uint8_t *s, ध्रुव uint8_t *सीमा,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    यदि ((सीमा - s) > INT32_MAX) {
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम असत्य;
    }
    int32_t length = static_cast<int32_t>(सीमा - s);
    यदि (length > 0) {
        append_Non_रिक्तानि_Unchanged(s, length, sink, options, edits);
    }
    प्रत्यागम सत्य;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = स्वभाव;

शून्य
CharStringByteSink::Append(ध्रुव वर्ण* अष्टकानि, int32_t n) {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    dest_.append(अष्टकानि, n, स्थितिः);
    // Any errors are silently ignored.
}

वर्ण*
CharStringByteSink::Get_Append_बफर(int32_t min_क्षमता_2,
                                    int32_t desired_क्षमता_hint,
                                    वर्ण* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_क्षमता_2,
                                    int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND {
    यदि (min_क्षमता_2 < 1 || scratch_क्षमता_2 < min_क्षमता_2) {
        *परिणाम_क्षमता_2 = 0;
        प्रत्यागम शून्यसूचकः;
    }

    UError_संहिता स्थितिः = U_ZERO_ERROR;
    वर्ण* परिणाम = dest_.get_Append_बफर(
            min_क्षमता_2,
            desired_क्षमता_hint,
            *परिणाम_क्षमता_2,
            स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        प्रत्यागम परिणाम;
    }

    *परिणाम_क्षमता_2 = scratch_क्षमता_2;
    प्रत्यागम scratch;
}

U_NAMESPACE_अन्तः
