// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, స్థిరరాశి char16_t *s16, int32_t s16_నిడివి,
                           ByteSink &sink, Edits *edits, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    అక్షరం scratch[200];
    int32_t s8_నిడివి = 0;
    కొరకు (int32_t i = 0; i < s16_నిడివి;) {
        int32_t capacity;
        int32_t desired_సామర్థ్యం = s16_నిడివి - i;
        ఒకవేళ (desired_సామర్థ్యం < (INT32_MAX / 3)) {
            desired_సామర్థ్యం *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } లేకపోతే ఒకవేళ (desired_సామర్థ్యం < (INT32_MAX / 2)) {
            desired_సామర్థ్యం *= 2;
        } లేకపోతే {
            desired_సామర్థ్యం = INT32_MAX;
        }
        అక్షరం *బఫరు = sink.Get_Append_బఫరు(U8_MAX_నిడివి, desired_సామర్థ్యం,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_నిడివి - 1;
        int32_t j = 0;
        కొరకు (; i < s16_నిడివి && j < capacity;) {
            UChar32 c;
            U16_తదుపరి_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(బఫరు, j, c);
        }
        ఒకవేళ (j > (INT32_MAX - s8_నిడివి)) {
            error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి అబద్ధం;
        }
        sink.Append(బఫరు, j);
        s8_నిడివి += j;
    }
    ఒకవేళ (edits != శూన్యసూచిక) {
        edits->కూడు_Replace(length, s8_నిడివి);
    }
    తిరిగి నిజం;
}

UBool
ByteSinkUtil::appendChange(స్థిరరాశి uint8_t *s, స్థిరరాశి uint8_t *హద్దు,
                           స్థిరరాశి char16_t *s16, int32_t s16_నిడివి,
                           ByteSink &sink, Edits *edits, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    ఒకవేళ ((హద్దు - s) > INT32_MAX) {
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి అబద్ధం;
    }
    తిరిగి appendChange(static_cast<int32_t>(హద్దు - s), s16, s16_నిడివి, sink, edits, error_సంకేతం);
}

శూన్య
ByteSinkUtil::append_సంకేతం_బిందువు(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    అక్షరం s8[U8_MAX_నిడివి];
    int32_t s8_నిడివి = 0;
    U8_APPEND_UNSAFE(s8, s8_నిడివి, c);
    ఒకవేళ (edits != శూన్యసూచిక) {
        edits->కూడు_Replace(length, s8_నిడివి);
    }
    sink.Append(s8, s8_నిడివి);
}

నామాకాశ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
అంతఃవరుస uint8_t getTwoByteLead(UChar32 c) { తిరిగి static_cast<uint8_t>((c >> 6) | 0xc0); }
అంతఃవరుస uint8_t getTwoByteTrail(UChar32 c) { తిరిగి static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

శూన్య
ByteSinkUtil::append_Two_బైట్లు(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    అక్షరం s8[2] = {static_cast<అక్షరం>(getTwoByteLead(c)), static_cast<అక్షరం>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

శూన్య
ByteSinkUtil::append_Non_ఖాళీలు_Unchanged(స్థిరరాశి uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    ఒకవేళ (edits != శూన్యసూచిక) {
        edits->కూడు_Unchanged(length);
    }
    ఒకవేళ ((options & U_OMIT_UNCHANGED_పాఠ్యం) == 0) {
        sink.Append(reinterpret_cast<స్థిరరాశి అక్షరం *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(స్థిరరాశి uint8_t *s, స్థిరరాశి uint8_t *హద్దు,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    ఒకవేళ ((హద్దు - s) > INT32_MAX) {
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి అబద్ధం;
    }
    int32_t length = static_cast<int32_t>(హద్దు - s);
    ఒకవేళ (length > 0) {
        append_Non_ఖాళీలు_Unchanged(s, length, sink, options, edits);
    }
    తిరిగి నిజం;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = అప్రమేయ;

శూన్య
CharStringByteSink::Append(స్థిరరాశి అక్షరం* బైట్లు, int32_t n) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    dest_.append(బైట్లు, n, స్థితి_2);
    // Any errors are silently ignored.
}

అక్షరం*
CharStringByteSink::Get_Append_బఫరు(int32_t min_సామర్థ్యం_2,
                                    int32_t desired_సామర్థ్యం_hint,
                                    అక్షరం* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_సామర్థ్యం_2,
                                    int32_t* ఫలితం_సామర్థ్యం_2) U_LIFETIME_BOUND {
    ఒకవేళ (min_సామర్థ్యం_2 < 1 || scratch_సామర్థ్యం_2 < min_సామర్థ్యం_2) {
        *ఫలితం_సామర్థ్యం_2 = 0;
        తిరిగి శూన్యసూచిక;
    }

    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    అక్షరం* ఫలితం = dest_.get_Append_బఫరు(
            min_సామర్థ్యం_2,
            desired_సామర్థ్యం_hint,
            *ఫలితం_సామర్థ్యం_2,
            స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        తిరిగి ఫలితం;
    }

    *ఫలితం_సామర్థ్యం_2 = scratch_సామర్థ్యం_2;
    తిరిగి scratch;
}

U_NAMESPACE_చివర
