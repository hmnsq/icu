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

#నిర్వచితం_కాకపోతే CHARSTRING_H
#నిర్వచించు CHARSTRING_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cmemory.h"

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
class U_COMMON_API_CLASS CharString : బహిరంగ UMemory {
బహిరంగ:
    U_COMMON_API CharString() : len(0) { బఫరు[0]=0; }
    U_COMMON_API CharString(StringPiece s, UError_సంకేతం &error_సంకేతం) : len(0) {
        బఫరు[0]=0;
        append(s, error_సంకేతం);
    }
    U_COMMON_API CharString(స్థిరరాశి CharString &s, UError_సంకేతం &error_సంకేతం) : len(0) {
        బఫరు[0]=0;
        append(s, error_సంకేతం);
    }
    U_COMMON_API CharString(స్థిరరాశి అక్షరం *s, int32_t s_నిడివి, UError_సంకేతం &error_సంకేతం) : len(0) {
        బఫరు[0]=0;
        append(s, s_నిడివి, error_సంకేతం);
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
    U_COMMON_API CharString &క్రియాకారకం=(CharString &&src) noexcept;

    /**
     * Replaces this string's contents with the other string's contents.
     * CharString does not support the standard copy constructor nor
     * the assignment operator, to make copies explicit and to
     * use a UErrorCode where memory allocations might be needed.
     */
    U_COMMON_API CharString &copyFrom(స్థిరరాశి CharString &other, UError_సంకేతం &error_సంకేతం);
    U_COMMON_API CharString &copyFrom(StringPiece s, UError_సంకేతం &error_సంకేతం);

    U_COMMON_API UBool is_ఖాళీలు() స్థిరరాశి { తిరిగి len==0; }
    U_COMMON_API int32_t నిడివి() స్థిరరాశి { తిరిగి len; }
    U_COMMON_API అక్షరం క్రియాకారకం[](int32_t సూచీ) స్థిరరాశి { తిరిగి బఫరు[సూచీ]; }
    U_COMMON_API StringPiece గమ్యం_String_Piece() స్థిరరాశి { తిరిగి StringPiece(బఫరు.getAlias(), len); }

    U_COMMON_API స్థిరరాశి అక్షరం *దత్తాంశం() స్థిరరాశి { తిరిగి బఫరు.getAlias(); }
    U_COMMON_API అక్షరం *దత్తాంశం() { తిరిగి బఫరు.getAlias(); }
    /**
     * Allocates length()+1 chars and copies the NUL-terminated data().
     * The caller must uprv_free() the result.
     */
    U_COMMON_API అక్షరం *clone_దత్తాంశం(UError_సంకేతం &error_సంకేతం) స్థిరరాశి;
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
    U_COMMON_API int32_t వెలికితీయి(అక్షరం *dest, int32_t సామర్థ్యం, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    U_COMMON_API bool క్రియాకారకం==(స్థిరరాశి CharString& other) స్థిరరాశి {
        తిరిగి len == other.నిడివి() && (len == 0 || uprv_memcmp(దత్తాంశం(), other.దత్తాంశం(), len) == 0);
    }
    U_COMMON_API bool క్రియాకారకం!=(స్థిరరాశి CharString& other) స్థిరరాశి {
        తిరిగి !క్రియాకారకం==(other);
    }

    U_COMMON_API bool క్రియాకారకం==(StringPiece other) స్థిరరాశి {
        తిరిగి len == other.నిడివి() && (len == 0 || uprv_memcmp(దత్తాంశం(), other.దత్తాంశం(), len) == 0);
    }
    U_COMMON_API bool క్రియాకారకం!=(StringPiece other) స్థిరరాశి {
        తిరిగి !క్రియాకారకం==(other);
    }

    /** @return last index of c, or -1 if c is not in this string */
    U_COMMON_API int32_t చివరి_సూచీ_యొక్క(అక్షరం c) స్థిరరాశి;

    U_COMMON_API bool కలిగియుంది(StringPiece s) స్థిరరాశి;

    U_COMMON_API CharString &clear() { len=0; బఫరు[0]=0; తిరిగి *this; }
    U_COMMON_API CharString &truncate(int32_t new_నిడివి);

    U_COMMON_API CharString &append(అక్షరం c, UError_సంకేతం &error_సంకేతం);
    U_COMMON_API CharString &append(StringPiece s, UError_సంకేతం &error_సంకేతం) {
        తిరిగి append(s.దత్తాంశం(), s.నిడివి(), error_సంకేతం);
    }
    U_COMMON_API CharString &append(స్థిరరాశి CharString &s, UError_సంకేతం &error_సంకేతం) {
        తిరిగి append(s.దత్తాంశం(), s.నిడివి(), error_సంకేతం);
    }
    U_COMMON_API CharString &append(స్థిరరాశి అక్షరం *s, int32_t s_నిడివి, UError_సంకేతం &స్థితి_2);

    U_COMMON_API CharString &append_సంఖ్య(int64_t సంఖ్య, UError_సంకేతం &స్థితి_2);

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
    U_COMMON_API అక్షరం *get_Append_బఫరు(int32_t min_సామర్థ్యం,
                                       int32_t desired_సామర్థ్యం_Hint,
                                       int32_t &ఫలితం_సామర్థ్యం,
                                       UError_సంకేతం &error_సంకేతం);

    U_COMMON_API CharString &appendInvariantChars(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం);
    U_COMMON_API CharString &appendInvariantChars(స్థిరరాశి char16_t* uchars,
                                                  int32_t ucharsLen,
                                                  UError_సంకేతం& error_సంకేతం);

    /**
     * Appends a filename/path part, e.g., a directory name.
     * First appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if necessary.
     * Does nothing if s is empty.
     */
    U_COMMON_API CharString &append_మార్గం_Part(StringPiece s, UError_సంకేతం &error_సంకేతం);

    /**
     * Appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if this string is not empty
     * and does not already end with a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR.
     */
    U_COMMON_API CharString &ensure_చివర్లు_With_File_Separator(UError_సంకేతం &error_సంకేతం);

అంతరంగ:
    Maybe_దొంతర_Array<అక్షరం, 40> బఫరు;
    int32_t len;

    UBool ensure_సామర్థ్యం(int32_t సామర్థ్యం, int32_t desired_సామర్థ్యం_Hint, UError_సంకేతం &error_సంకేతం);

    CharString(స్థిరరాశి CharString &other) = delete; // forbid copying of this class
    CharString &క్రియాకారకం=(స్థిరరాశి CharString &other) = delete; // forbid copying of this class

    /**
     * Returns U_FILE_ALT_SEP_CHAR if found in string, and U_FILE_SEP_CHAR is not found.
     * Otherwise returns U_FILE_SEP_CHAR.
     */
    అక్షరం getDirSepChar() స్థిరరాశి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
//eof
