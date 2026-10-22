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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CHARSTRING_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHARSTRING_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

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
class U_COMMON_API_CLASS CharString : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    U_COMMON_API CharString() : len(0) { ਬਫਰ[0]=0; }
    U_COMMON_API CharString(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ) : len(0) {
        ਬਫਰ[0]=0;
        append(s, error_ਕੋਡ);
    }
    U_COMMON_API CharString(ਅਚਲ CharString &s, UError_ਕੋਡ &error_ਕੋਡ) : len(0) {
        ਬਫਰ[0]=0;
        append(s, error_ਕੋਡ);
    }
    U_COMMON_API CharString(ਅਚਲ ਅੱਖਰ *s, int32_t s_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) : len(0) {
        ਬਫਰ[0]=0;
        append(s, s_ਲੰਮਾਈ, error_ਕੋਡ);
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
    U_COMMON_API CharString &ਸੰਚਾਲਕ=(CharString &&src) noexcept;

    /**
     * Replaces this string's contents with the other string's contents.
     * CharString does not support the standard copy constructor nor
     * the assignment operator, to make copies explicit and to
     * use a UErrorCode where memory allocations might be needed.
     */
    U_COMMON_API CharString &copyFrom(ਅਚਲ CharString &other, UError_ਕੋਡ &error_ਕੋਡ);
    U_COMMON_API CharString &copyFrom(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ);

    U_COMMON_API UBool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ len==0; }
    U_COMMON_API int32_t ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ len; }
    U_COMMON_API ਅੱਖਰ ਸੰਚਾਲਕ[](int32_t ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ ਬਫਰ[ਸੂਚਕਾਂਕ]; }
    U_COMMON_API StringPiece ਮੰਜ਼ਿਲ_String_Piece() ਅਚਲ { ਵਾਪਸ StringPiece(ਬਫਰ.getAlias(), len); }

    U_COMMON_API ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ() ਅਚਲ { ਵਾਪਸ ਬਫਰ.getAlias(); }
    U_COMMON_API ਅੱਖਰ *ਅੰਕੜੇ() { ਵਾਪਸ ਬਫਰ.getAlias(); }
    /**
     * Allocates length()+1 chars and copies the NUL-terminated data().
     * The caller must uprv_free() the result.
     */
    U_COMMON_API ਅੱਖਰ *clone_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;
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
    U_COMMON_API int32_t ਕੱਢੋ(ਅੱਖਰ *dest, int32_t ਸਮਰੱਥਾ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    U_COMMON_API bool ਸੰਚਾਲਕ==(ਅਚਲ CharString& other) ਅਚਲ {
        ਵਾਪਸ len == other.ਲੰਮਾਈ() && (len == 0 || uprv_memcmp(ਅੰਕੜੇ(), other.ਅੰਕੜੇ(), len) == 0);
    }
    U_COMMON_API bool ਸੰਚਾਲਕ!=(ਅਚਲ CharString& other) ਅਚਲ {
        ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
    }

    U_COMMON_API bool ਸੰਚਾਲਕ==(StringPiece other) ਅਚਲ {
        ਵਾਪਸ len == other.ਲੰਮਾਈ() && (len == 0 || uprv_memcmp(ਅੰਕੜੇ(), other.ਅੰਕੜੇ(), len) == 0);
    }
    U_COMMON_API bool ਸੰਚਾਲਕ!=(StringPiece other) ਅਚਲ {
        ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
    }

    /** @return last index of c, or -1 if c is not in this string */
    U_COMMON_API int32_t ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(ਅੱਖਰ c) ਅਚਲ;

    U_COMMON_API bool ਸ਼ਾਮਲ_ਹੈ(StringPiece s) ਅਚਲ;

    U_COMMON_API CharString &clear() { len=0; ਬਫਰ[0]=0; ਵਾਪਸ *this; }
    U_COMMON_API CharString &truncate(int32_t new_ਲੰਮਾਈ);

    U_COMMON_API CharString &append(ਅੱਖਰ c, UError_ਕੋਡ &error_ਕੋਡ);
    U_COMMON_API CharString &append(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ) {
        ਵਾਪਸ append(s.ਅੰਕੜੇ(), s.ਲੰਮਾਈ(), error_ਕੋਡ);
    }
    U_COMMON_API CharString &append(ਅਚਲ CharString &s, UError_ਕੋਡ &error_ਕੋਡ) {
        ਵਾਪਸ append(s.ਅੰਕੜੇ(), s.ਲੰਮਾਈ(), error_ਕੋਡ);
    }
    U_COMMON_API CharString &append(ਅਚਲ ਅੱਖਰ *s, int32_t s_ਲੰਮਾਈ, UError_ਕੋਡ &ਸਥਿਤੀ);

    U_COMMON_API CharString &append_ਸੰਖਿਆ(int64_t ਸੰਖਿਆ, UError_ਕੋਡ &ਸਥਿਤੀ);

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
    U_COMMON_API ਅੱਖਰ *get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ,
                                       int32_t desired_ਸਮਰੱਥਾ_Hint,
                                       int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ,
                                       UError_ਕੋਡ &error_ਕੋਡ);

    U_COMMON_API CharString &appendInvariantChars(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ);
    U_COMMON_API CharString &appendInvariantChars(ਅਚਲ char16_t* uchars,
                                                  int32_t ucharsLen,
                                                  UError_ਕੋਡ& error_ਕੋਡ);

    /**
     * Appends a filename/path part, e.g., a directory name.
     * First appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if necessary.
     * Does nothing if s is empty.
     */
    U_COMMON_API CharString &append_ਰਸਤਾ_Part(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if this string is not empty
     * and does not already end with a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR.
     */
    U_COMMON_API CharString &ensure_ਸਿਰੇ_With_File_Separator(UError_ਕੋਡ &error_ਕੋਡ);

ਨਿਜੀ:
    Maybe_ਢੇਰ_Array<ਅੱਖਰ, 40> ਬਫਰ;
    int32_t len;

    UBool ensure_ਸਮਰੱਥਾ(int32_t ਸਮਰੱਥਾ, int32_t desired_ਸਮਰੱਥਾ_Hint, UError_ਕੋਡ &error_ਕੋਡ);

    CharString(ਅਚਲ CharString &other) = delete; // forbid copying of this class
    CharString &ਸੰਚਾਲਕ=(ਅਚਲ CharString &other) = delete; // forbid copying of this class

    /**
     * Returns U_FILE_ALT_SEP_CHAR if found in string, and U_FILE_SEP_CHAR is not found.
     * Otherwise returns U_FILE_SEP_CHAR.
     */
    ਅੱਖਰ getDirSepChar() ਅਚਲ;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
//eof
