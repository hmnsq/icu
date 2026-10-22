// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2001-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/19/2001  aliu        Creation.
*   05/19/2010  markus      Rewritten from scratch
**********************************************************************
*/

#ifndef CHARSTRING_H
#define CHARSTRING_H

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_BEGIN

/**
 * ICU-internal char * string class.
 * This class does not assume or enforce any particular character encoding.
 * Raw bytes can be stored. The string object owns its characters.
 * A terminating NUL is stored, but the class does not prevent embedded NUL characters.
 *
 * This class wants to be convenient but is also deliberately minimalist.
 * Please do not add methods if they only add minor convenience.
 * For example:
 *   cs.data()[5]='a';  // no need for setCharAt(5, 'a')
 */
class U_COMMON_API_CLASS CharString : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    U_COMMON_API CharString() : len(0) { buffer[0]=0; }
    U_COMMON_API CharString(StringPiece s, UErrorCode &errorCode) : len(0) {
        buffer[0]=0;
        append(s, errorCode);
    }
    U_COMMON_API CharString(ଅଚଳ CharString &s, UErrorCode &errorCode) : len(0) {
        buffer[0]=0;
        append(s, errorCode);
    }
    U_COMMON_API CharString(ଅଚଳ ଅକ୍ଷର *s, int32_t sLength, UErrorCode &errorCode) : len(0) {
        buffer[0]=0;
        append(s, sLength, errorCode);
    }
    U_COMMON_API ~CharString() {}

    /**
     * Move constructor; might leave src in an undefined state.
     * This string will have the same contents and state that the source string had.
     */
    U_COMMON_API CharString(CharString &&src) noexcept;
    /**
     * Move assignment operator; might leave src in an undefined state.
     * This string will have the same contents and state that the source string had.
     * The behavior is undefined if *this and src are the same object.
     */
    U_COMMON_API CharString &operator=(CharString &&src) noexcept;

    /**
     * Replaces this string's contents with the other string's contents.
     * CharString does not support the standard copy constructor nor
     * the assignment operator, to make copies explicit and to
     * use a UErrorCode where memory allocations might be needed.
     */
    U_COMMON_API CharString &copyFrom(ଅଚଳ CharString &other, UErrorCode &errorCode);
    U_COMMON_API CharString &copyFrom(StringPiece s, UErrorCode &errorCode);

    U_COMMON_API UBool isEmpty() ଅଚଳ { ଫେରନ୍ତୁ len==0; }
    U_COMMON_API int32_t length() ଅଚଳ { ଫେରନ୍ତୁ len; }
    U_COMMON_API ଅକ୍ଷର operator[](int32_t index) ଅଚଳ { ଫେରନ୍ତୁ buffer[index]; }
    U_COMMON_API StringPiece toStringPiece() ଅଚଳ { ଫେରନ୍ତୁ StringPiece(buffer.getAlias(), len); }

    U_COMMON_API ଅଚଳ ଅକ୍ଷର *data() ଅଚଳ { ଫେରନ୍ତୁ buffer.getAlias(); }
    U_COMMON_API ଅକ୍ଷର *data() { ଫେରନ୍ତୁ buffer.getAlias(); }
    /**
     * Allocates length()+1 chars and copies the NUL-terminated data().
     * The caller must uprv_free() the result.
     */
    U_COMMON_API ଅକ୍ଷର *cloneData(UErrorCode &errorCode) ଅଚଳ;
    /**
     * Copies the contents of the string into dest.
     * Checks if there is enough space in dest, extracts the entire string if possible,
     * and NUL-terminates dest if possible.
     *
     * If the string fits into dest but cannot be NUL-terminated (length()==capacity),
     * then the error code is set to U_STRING_NOT_TERMINATED_WARNING.
     * If the string itself does not fit into dest (length()>capacity),
     * then the error code is set to U_BUFFER_OVERFLOW_ERROR.
     *
     * @param dest Destination string buffer.
     * @param capacity Size of the dest buffer (number of chars).
     * @param errorCode ICU error code.
     * @return length()
     */
    U_COMMON_API int32_t extract(ଅକ୍ଷର *dest, int32_t capacity, UErrorCode &errorCode) ଅଚଳ;

    U_COMMON_API bool operator==(ଅଚଳ CharString& other) ଅଚଳ {
        ଫେରନ୍ତୁ len == other.length() && (len == 0 || uprv_memcmp(data(), other.data(), len) == 0);
    }
    U_COMMON_API bool operator!=(ଅଚଳ CharString& other) ଅଚଳ {
        ଫେରନ୍ତୁ !operator==(other);
    }

    U_COMMON_API bool operator==(StringPiece other) ଅଚଳ {
        ଫେରନ୍ତୁ len == other.length() && (len == 0 || uprv_memcmp(data(), other.data(), len) == 0);
    }
    U_COMMON_API bool operator!=(StringPiece other) ଅଚଳ {
        ଫେରନ୍ତୁ !operator==(other);
    }

    /** @return last index of c, or -1 if c is not in this string */
    U_COMMON_API int32_t lastIndexOf(ଅକ୍ଷର c) ଅଚଳ;

    U_COMMON_API bool contains(StringPiece s) ଅଚଳ;

    U_COMMON_API CharString &clear() { len=0; buffer[0]=0; ଫେରନ୍ତୁ *this; }
    U_COMMON_API CharString &truncate(int32_t newLength);

    U_COMMON_API CharString &append(ଅକ୍ଷର c, UErrorCode &errorCode);
    U_COMMON_API CharString &append(StringPiece s, UErrorCode &errorCode) {
        ଫେରନ୍ତୁ append(s.data(), s.length(), errorCode);
    }
    U_COMMON_API CharString &append(ଅଚଳ CharString &s, UErrorCode &errorCode) {
        ଫେରନ୍ତୁ append(s.data(), s.length(), errorCode);
    }
    U_COMMON_API CharString &append(ଅଚଳ ଅକ୍ଷର *s, int32_t sLength, UErrorCode &status);

    U_COMMON_API CharString &append_ସଂଖ୍ୟା(int64_t ସଂଖ୍ୟା, UErrorCode &status);

    /**
     * Returns a writable buffer for appending and writes the buffer's capacity to
     * resultCapacity. Guarantees resultCapacity>=minCapacity if U_SUCCESS().
     * There will additionally be space for a terminating NUL right at resultCapacity.
     * (This function is similar to ByteSink.GetAppendBuffer().)
     *
     * The returned buffer is only valid until the next write operation
     * on this string.
     *
     * After writing at most resultCapacity bytes, call append() with the
     * pointer returned from this function and the number of bytes written.
     *
     * @param minCapacity required minimum capacity of the returned buffer;
     *                    must be non-negative
     * @param desiredCapacityHint desired capacity of the returned buffer;
     *                            must be non-negative
     * @param resultCapacity will be set to the capacity of the returned buffer
     * @param errorCode in/out error code
     * @return a buffer with resultCapacity>=min_capacity
     */
    U_COMMON_API ଅକ୍ଷର *getAppendBuffer(int32_t minCapacity,
                                       int32_t desiredCapacityHint,
                                       int32_t &resultCapacity,
                                       UErrorCode &errorCode);

    U_COMMON_API CharString &appendInvariantChars(ଅଚଳ UnicodeString &s, UErrorCode &errorCode);
    U_COMMON_API CharString &appendInvariantChars(ଅଚଳ char16_t* uchars,
                                                  int32_t ucharsLen,
                                                  UErrorCode& errorCode);

    /**
     * Appends a filename/path part, e.g., a directory name.
     * First appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if necessary.
     * Does nothing if s is empty.
     */
    U_COMMON_API CharString &appendPathPart(StringPiece s, UErrorCode &errorCode);

    /**
     * Appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if this string is not empty
     * and does not already end with a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR.
     */
    U_COMMON_API CharString &ensureEndsWithFileSeparator(UErrorCode &errorCode);

ବ୍ୟକ୍ତିଗତ:
    Maybe_ଗଦା_Array<ଅକ୍ଷର, 40> buffer;
    int32_t len;

    UBool ensureCapacity(int32_t capacity, int32_t desiredCapacityHint, UErrorCode &errorCode);

    CharString(ଅଚଳ CharString &other) = delete; // forbid copying of this class
    CharString &operator=(ଅଚଳ CharString &other) = delete; // forbid copying of this class

    /**
     * Returns U_FILE_ALT_SEP_CHAR if found in string, and U_FILE_SEP_CHAR is not found.
     * Otherwise returns U_FILE_SEP_CHAR.
     */
    ଅକ୍ଷର getDirSepChar() ଅଚଳ;
};

U_NAMESPACE_END

#endif
//eof
