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

#ifndef __UNISTRAPPENDER_H__
#define __UNISTRAPPENDER_H__

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"

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
class UnicodeStringAppender : সর্বজনীন UMemory {
সর্বজনীন:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    অন্তঃরেখ শূন্য append(char16_t x) {
        যদি (fIdx == UPRV_LENGTHOF(f_বাফার)) {
            fDest->append(f_বাফার, 0, fIdx);
            fIdx = 0;
        }
        f_বাফার[fIdx++] = x;
    }

    অন্তঃরেখ শূন্য append(UChar32 x) {
        যদি (fIdx >= UPRV_LENGTHOF(f_বাফার) - 1) {
            fDest->append(f_বাফার, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_বাফার, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    অন্তঃরেখ শূন্য flush() {
        যদি (fIdx) {
            fDest->append(f_বাফার, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
ব্যক্তিগত:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_বাফার[32];
    UnicodeStringAppender(ধ্রুবক UnicodeStringAppender &other);
    UnicodeStringAppender &operator=(ধ্রুবক UnicodeStringAppender &other);
};

U_NAMESPACE_END

#endif
