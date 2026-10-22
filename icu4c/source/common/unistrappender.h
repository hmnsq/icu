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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UNISTRAPPENDER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UNISTRAPPENDER_H__

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

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
class UnicodeStringAppender : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(char16_t x) {
        ಆದರೆ (fIdx == UPRV_LENGTHOF(f_ಬಫರ್)) {
            fDest->append(f_ಬಫರ್, 0, fIdx);
            fIdx = 0;
        }
        f_ಬಫರ್[fIdx++] = x;
    }

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(UChar32 x) {
        ಆದರೆ (fIdx >= UPRV_LENGTHOF(f_ಬಫರ್) - 1) {
            fDest->append(f_ಬಫರ್, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_ಬಫರ್, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ flush() {
        ಆದರೆ (fIdx) {
            fDest->append(f_ಬಫರ್, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
ಖಾಸಗಿ:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_ಬಫರ್[32];
    UnicodeStringAppender(ಸ್ಥಿರಾಂಕ UnicodeStringAppender &other);
    UnicodeStringAppender &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UnicodeStringAppender &other);
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
