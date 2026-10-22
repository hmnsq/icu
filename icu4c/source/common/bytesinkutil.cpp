// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// bytesinkutil.cpp
// created: 2017sep14 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN

UBool
ByteSinkUtil::appendChange(int32_t length, സ്ഥിരം char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    അക്ഷരം scratch[200];
    int32_t s8Length = 0;
    വേണ്ടി (int32_t i = 0; i < s16Length;) {
        int32_t capacity;
        int32_t desiredCapacity = s16Length - i;
        എങ്കിൽ (desiredCapacity < (INT32_MAX / 3)) {
            desiredCapacity *= 3;  // max 3 UTF-8 bytes per UTF-16 code unit
        } അല്ലെങ്കിൽ എങ്കിൽ (desiredCapacity < (INT32_MAX / 2)) {
            desiredCapacity *= 2;
        } അല്ലെങ്കിൽ {
            desiredCapacity = INT32_MAX;
        }
        അക്ഷരം *buffer = sink.GetAppendBuffer(U8_MAX_LENGTH, desiredCapacity,
                                            scratch, UPRV_LENGTHOF(scratch), &capacity);
        capacity -= U8_MAX_LENGTH - 1;
        int32_t j = 0;
        വേണ്ടി (; i < s16Length && j < capacity;) {
            UChar32 c;
            U16_NEXT_UNSAFE(s16, i, c);
            U8_APPEND_UNSAFE(buffer, j, c);
        }
        എങ്കിൽ (j > (INT32_MAX - s8Length)) {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ അസത്യം;
        }
        sink.Append(buffer, j);
        s8Length += j;
    }
    എങ്കിൽ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    തിരികെ സത്യം;
}

UBool
ByteSinkUtil::appendChange(സ്ഥിരം uint8_t *s, സ്ഥിരം uint8_t *limit,
                           സ്ഥിരം char16_t *s16, int32_t s16Length,
                           ByteSink &sink, Edits *edits, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    എങ്കിൽ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ അസത്യം;
    }
    തിരികെ appendChange(static_cast<int32_t>(limit - s), s16, s16Length, sink, edits, errorCode);
}

ശൂന്യം
ByteSinkUtil::appendCodePoint(int32_t length, UChar32 c, ByteSink &sink, Edits *edits) {
    അക്ഷരം s8[U8_MAX_LENGTH];
    int32_t s8Length = 0;
    U8_APPEND_UNSAFE(s8, s8Length, c);
    എങ്കിൽ (edits != nullptr) {
        edits->addReplace(length, s8Length);
    }
    sink.Append(s8, s8Length);
}

നാമാകാശ {

// See unicode/utf8.h U8_APPEND_UNSAFE().
വരിയിൽ uint8_t getTwoByteLead(UChar32 c) { തിരികെ static_cast<uint8_t>((c >> 6) | 0xc0); }
വരിയിൽ uint8_t getTwoByteTrail(UChar32 c) { തിരികെ static_cast<uint8_t>((c & 0x3f) | 0x80); }

}  // namespace

ശൂന്യം
ByteSinkUtil::appendTwoBytes(UChar32 c, ByteSink &sink) {
    U_ASSERT(0x80 <= c && c <= 0x7ff);  // 2-byte UTF-8
    അക്ഷരം s8[2] = {static_cast<അക്ഷരം>(getTwoByteLead(c)), static_cast<അക്ഷരം>(getTwoByteTrail(c))};
    sink.Append(s8, 2);
}

ശൂന്യം
ByteSinkUtil::appendNonEmptyUnchanged(സ്ഥിരം uint8_t *s, int32_t length,
                                      ByteSink &sink, uint32_t options, Edits *edits) {
    U_ASSERT(length > 0);
    എങ്കിൽ (edits != nullptr) {
        edits->addUnchanged(length);
    }
    എങ്കിൽ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
        sink.Append(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(s), length);
    }
}

UBool
ByteSinkUtil::appendUnchanged(സ്ഥിരം uint8_t *s, സ്ഥിരം uint8_t *limit,
                              ByteSink &sink, uint32_t options, Edits *edits,
                              UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    എങ്കിൽ ((limit - s) > INT32_MAX) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ അസത്യം;
    }
    int32_t length = static_cast<int32_t>(limit - s);
    എങ്കിൽ (length > 0) {
        appendNonEmptyUnchanged(s, length, sink, options, edits);
    }
    തിരികെ സത്യം;
}

CharStringByteSink::CharStringByteSink(CharString* dest) : dest_(*dest) {
}

CharStringByteSink::~CharStringByteSink() = സ്ഥിരസ്ഥിതി;

ശൂന്യം
CharStringByteSink::Append(സ്ഥിരം അക്ഷരം* bytes, int32_t n) {
    UErrorCode status = U_ZERO_ERROR;
    dest_.append(bytes, n, status);
    // Any errors are silently ignored.
}

അക്ഷരം*
CharStringByteSink::GetAppendBuffer(int32_t min_capacity,
                                    int32_t desired_capacity_hint,
                                    അക്ഷരം* scratch U_LIFETIME_BOUND,
                                    int32_t scratch_capacity,
                                    int32_t* result_capacity) U_LIFETIME_BOUND {
    എങ്കിൽ (min_capacity < 1 || scratch_capacity < min_capacity) {
        *result_capacity = 0;
        തിരികെ nullptr;
    }

    UErrorCode status = U_ZERO_ERROR;
    അക്ഷരം* result = dest_.getAppendBuffer(
            min_capacity,
            desired_capacity_hint,
            *result_capacity,
            status);
    എങ്കിൽ (U_SUCCESS(status)) {
        തിരികെ result;
    }

    *result_capacity = scratch_capacity;
    തിരികെ scratch;
}

U_NAMESPACE_END
