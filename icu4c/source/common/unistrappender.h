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

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"

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
class UnicodeStringAppender : പൊതു UMemory {
പൊതു:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    വരിയിൽ ശൂന്യം append(char16_t x) {
        എങ്കിൽ (fIdx == UPRV_LENGTHOF(fBuffer)) {
            fDest->append(fBuffer, 0, fIdx);
            fIdx = 0;
        }
        fBuffer[fIdx++] = x;
    }

    വരിയിൽ ശൂന്യം append(UChar32 x) {
        എങ്കിൽ (fIdx >= UPRV_LENGTHOF(fBuffer) - 1) {
            fDest->append(fBuffer, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(fBuffer, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    വരിയിൽ ശൂന്യം flush() {
        എങ്കിൽ (fIdx) {
            fDest->append(fBuffer, 0, fIdx);
        }
        fIdx = 0;
    }

    /**
     * flush the buffer when we go out of scope.
     */
    ~UnicodeStringAppender() {
        flush();
    }
സ്വകാര്യ:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t fBuffer[32];
    UnicodeStringAppender(സ്ഥിരം UnicodeStringAppender &other);
    UnicodeStringAppender &operator=(സ്ഥിരം UnicodeStringAppender &other);
};

U_NAMESPACE_END

#endif
