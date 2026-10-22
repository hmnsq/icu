// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/stringoptions.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, અચલ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    અક્ષર scratch[200];
    int32_t s8Length = 0;
    માટે (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        જો (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } નહીંતર જો (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } નહીંતર {
            desiredCapacity = INT32_MAX;
        }
        અક્ષર *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        માટે (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        જો (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા ખોટું;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    જો (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    પાછા સાચું;
}

UBool
ByteSinkUtil::appendChange(અચલ uint8_t *s, અચલ uint8_t *limit,
                           અચલ char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    જો ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા ખોટું;
    }
    પાછા appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

શૂન્ય
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    અક્ષર s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    જો (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

નામાકાશ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
અંતઃરેખ uint8_t getTwoByteLead(UChar32 c) { પાછા static_cast<uint8_t>((c >> 6) | 0xc0); }
અંતઃરેખ uint8_t getTwoByteTrail(UChar32 c) { પાછા static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

શૂન્ય
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    અક્ષર s8[2] = {static_cast<અક્ષર>(getTwoByteLead(c)), static_cast<અક્ષર>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

શૂન્ય
ByteSinkUtil::appendNonEmptyUnchanged(અચલ uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    જો (edits != nullptr) {
        edits->addUnchanged(length);
    }
    જો ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<અચલ અક્ષર *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(અચલ uint8_t *s, અચલ uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    જો ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા ખોટું;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    જો (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    પાછા સાચું;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = મૂળભૂત;

શૂન્ય
CharStringByteSink::Append(અચલ અક્ષર* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

અક્ષર*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    અક્ષર* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    જો (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        પાછા nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    અક્ષર* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    જો (U_SUCCESS(status)) {
        પાછા result;
    }

    *result_capacity = scratch_capacity;
    પાછા scratch;
}

U_NAMESPACE_END
