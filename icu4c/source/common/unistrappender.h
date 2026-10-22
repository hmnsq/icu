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

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

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
class UnicodeStringAppender : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(char16_t x) {
        ଯଦି (fIdx == UPRV_LENGTHOF(fBuffer)) {
            fDest->append(fBuffer, 0, fIdx);
            fIdx = 0;
        }
        fBuffer[fIdx++] = x;
    }

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(UChar32 x) {
        ଯଦି (fIdx >= UPRV_LENGTHOF(fBuffer) - 1) {
            fDest->append(fBuffer, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(fBuffer, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ flush() {
        ଯଦି (fIdx) {
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
ବ୍ୟକ୍ତିଗତ:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t fBuffer[32];
    UnicodeStringAppender(ଅଚଳ UnicodeStringAppender &other);
    UnicodeStringAppender &operator=(ଅଚଳ UnicodeStringAppender &other);
};

U_NAMESPACE_END

#endif
