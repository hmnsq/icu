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

#अगर_अघोषित __UNISTRAPPENDER_H__
#घोषणा __UNISTRAPPENDER_H__

#समावेश "unicode/unistr.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/utf16.h"
#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"

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
class UnicodeStringAppender : खुला UMemory {
खुला:
    
    /**
     * dest is the UnicodeString being appended to. It must always
     * exist while this instance exists.
     */
    UnicodeStringAppender(UnicodeString &dest) : fDest(&dest), fIdx(0) { }

    अंतरभूत व्योम append(char16_t x) {
        अगर (fIdx == UPRV_LENGTHOF(f_बफर)) {
            fDest->append(f_बफर, 0, fIdx);
            fIdx = 0;
        }
        f_बफर[fIdx++] = x;
    }

    अंतरभूत व्योम append(UChar32 x) {
        अगर (fIdx >= UPRV_LENGTHOF(f_बफर) - 1) {
            fDest->append(f_बफर, 0, fIdx);
            fIdx = 0;
        }
        U16_APPEND_UNSAFE(f_बफर, fIdx, x);
    }

    /**
     * Ensures that all appended characters have been written out to dest.
     */
    अंतरभूत व्योम flush() {
        अगर (fIdx) {
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
निजी:
    UnicodeString *fDest;
    int32_t fIdx;
    char16_t f_बफर[32];
    UnicodeStringAppender(स्थिर UnicodeStringAppender &other);
    UnicodeStringAppender &चालक=(स्थिर UnicodeStringAppender &other);
};

U_NAMESPACE_END

#पूर्ण_अगर
