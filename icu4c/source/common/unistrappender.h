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

#جے_بنا_تعریف __UNISTRAPPENDER_H__
#تعریف_کرو __UNISTRAPPENDER_H__

#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utf16.h"
#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"

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
class UnicodeStringAppender : عام UMemory {
عام:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    مضمن فراغ append(char16_t x) {
        إذا (fIdx == UPRV_LENGTHOF(f_بفر)) {
            fDest->append(f_بفر, 0, fIdx);
            fIdx = 0;
        }
        f_بفر[fIdx++] = x;
    }

    مضمن فراغ append(UChar32 x) {
        إذا (fIdx >= UPRV_LENGTHOF(f_بفر) - 1) {
            fDest->append(f_بفر, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_بفر, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    مضمن فراغ flush() {
        إذا (fIdx) {
            fDest->append(f_بفر, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
خاص:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_بفر[32];
    UnicodeStringAppender(ثابت UnicodeStringAppender &other);
    UnicodeStringAppender &عامل=(ثابت UnicodeStringAppender &other);
};

U_NAMESPACE_سرا

#جے_انت
