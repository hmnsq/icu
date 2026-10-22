// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File unistrappender.h
******************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UNISTRAPPENDER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UNISTRAPPENDER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * An optimization for the slowness of calling UnicodeString::append()
 * one character at a time in a loop. It stores appends in a buffer while
 * never actually calling append on the unicode string unless the buffer
 * fills up or is flushed.
 * 
 * proper usage:
 * {
 *     UnicodeStringAppender appender(astring);
 *     for (int32_t i = 0; i < 100; ++i) {
 *        appender.append((char16_t) i);
 *     }
 *     // appender flushed automatically when it goes out of scope.
 * }
 */
class UnicodeStringAppender : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    ਸਤਰਅੰਦਰ ਖਾਲੀ append(char16_t x) {
        ਜੇ (fIdx == UPRV_LENGTHOF(f_ਬਫਰ)) {
            fDest->append(f_ਬਫਰ, 0, fIdx);
            fIdx = 0;
        }
        f_ਬਫਰ[fIdx++] = x;
    }

    ਸਤਰਅੰਦਰ ਖਾਲੀ append(UChar32 x) {
        ਜੇ (fIdx >= UPRV_LENGTHOF(f_ਬਫਰ) - 1) {
            fDest->append(f_ਬਫਰ, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_ਬਫਰ, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    ਸਤਰਅੰਦਰ ਖਾਲੀ flush() {
        ਜੇ (fIdx) {
            fDest->append(f_ਬਫਰ, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
ਨਿਜੀ:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_ਬਫਰ[32];
    UnicodeStringAppender(ਅਚਲ UnicodeStringAppender &other);
    UnicodeStringAppender &ਸੰਚਾਲਕ=(ਅਚਲ UnicodeStringAppender &other);
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
