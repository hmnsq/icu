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

#నిర్వచితం_కాకపోతే __UNISTRAPPENDER_H__
#నిర్వచించు __UNISTRAPPENDER_H__

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"

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
class UnicodeStringAppender : బహిరంగ UMemory {
బహిరంగ:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    అంతఃవరుస శూన్య append(char16_t x) {
        ఒకవేళ (fIdx == UPRV_LENGTHOF(f_బఫరు)) {
            fDest->append(f_బఫరు, 0, fIdx);
            fIdx = 0;
        }
        f_బఫరు[fIdx++] = x;
    }

    అంతఃవరుస శూన్య append(UChar32 x) {
        ఒకవేళ (fIdx >= UPRV_LENGTHOF(f_బఫరు) - 1) {
            fDest->append(f_బఫరు, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_బఫరు, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    అంతఃవరుస శూన్య flush() {
        ఒకవేళ (fIdx) {
            fDest->append(f_బఫరు, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
అంతరంగ:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_బఫరు[32];
    UnicodeStringAppender(స్థిరరాశి UnicodeStringAppender &other);
    UnicodeStringAppender &క్రియాకారకం=(స్థిరరాశి UnicodeStringAppender &other);
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
