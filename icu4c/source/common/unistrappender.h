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

#यदि_अपरिभाषितम् __UNISTRAPPENDER_H__
#परिभाषय __UNISTRAPPENDER_H__

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"

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
class UnicodeStringAppender : सार्वजनिक UMemory {
सार्वजनिक:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    अन्तःस्थ शून्य append(char16_t x) {
        यदि (fIdx == UPRV_LENGTHOF(f_बफर)) {
            fDest->append(f_बफर, 0, fIdx);
            fIdx = 0;
        }
        f_बफर[fIdx++] = x;
    }

    अन्तःस्थ शून्य append(UChar32 x) {
        यदि (fIdx >= UPRV_LENGTHOF(f_बफर) - 1) {
            fDest->append(f_बफर, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_बफर, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    अन्तःस्थ शून्य flush() {
        यदि (fIdx) {
            fDest->append(f_बफर, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
गुप्त:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_बफर[32];
    UnicodeStringAppender(ध्रुव UnicodeStringAppender &other);
    UnicodeStringAppender &संक्रियकः=(ध्रुव UnicodeStringAppender &other);
};

U_NAMESPACE_अन्तः

#यदि_अन्तः
