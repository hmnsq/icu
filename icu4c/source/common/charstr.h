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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CHARSTRING_H
#ವ್ಯಾಖ್ಯಾನಿಸು CHARSTRING_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

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
class U_COMMON_API_CLASS CharString : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    U_COMMON_API CharString() : len(0) { ಬಫರ್[0]=0; }
    U_COMMON_API CharString(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : len(0) {
        ಬಫರ್[0]=0;
        append(s, error_ಸಂಕೇತ);
    }
    U_COMMON_API CharString(ಸ್ಥಿರಾಂಕ CharString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : len(0) {
        ಬಫರ್[0]=0;
        append(s, error_ಸಂಕೇತ);
    }
    U_COMMON_API CharString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t s_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : len(0) {
        ಬಫರ್[0]=0;
        append(s, s_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
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
    U_COMMON_API CharString &ಕ್ರಿಯಾಕಾರಕ=(CharString &&src) noexcept;

    /**
     * Replaces this string's contents with the other string's contents.
     * CharString does not support the standard copy constructor nor
     * the assignment operator, to make copies explicit and to
     * use a UErrorCode where memory allocations might be needed.
     */
    U_COMMON_API CharString &copyFrom(ಸ್ಥಿರಾಂಕ CharString &other, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    U_COMMON_API CharString &copyFrom(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    U_COMMON_API UBool is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ len==0; }
    U_COMMON_API int32_t ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ len; }
    U_COMMON_API ಅಕ್ಷರ ಕ್ರಿಯಾಕಾರಕ[](int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಫರ್[ಸೂಚ್ಯಂಕ]; }
    U_COMMON_API StringPiece ಗಮ್ಯ_String_Piece() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ StringPiece(ಬಫರ್.getAlias(), len); }

    U_COMMON_API ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಫರ್.getAlias(); }
    U_COMMON_API ಅಕ್ಷರ *ದತ್ತಾಂಶ() { ಹಿಂತಿರುಗಿ ಬಫರ್.getAlias(); }
    /**
     * Allocates length()+1 chars and copies the NUL-terminated data().
     * The caller must uprv_free() the result.
     */
    U_COMMON_API ಅಕ್ಷರ *clone_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;
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
    U_COMMON_API int32_t ಹೊರತೆಗೆ(ಅಕ್ಷರ *dest, int32_t ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    U_COMMON_API bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ CharString& other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ len == other.ಉದ್ದಳತೆ() && (len == 0 || uprv_memcmp(ದತ್ತಾಂಶ(), other.ದತ್ತಾಂಶ(), len) == 0);
    }
    U_COMMON_API bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ CharString& other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
    }

    U_COMMON_API bool ಕ್ರಿಯಾಕಾರಕ==(StringPiece other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ len == other.ಉದ್ದಳತೆ() && (len == 0 || uprv_memcmp(ದತ್ತಾಂಶ(), other.ದತ್ತಾಂಶ(), len) == 0);
    }
    U_COMMON_API bool ಕ್ರಿಯಾಕಾರಕ!=(StringPiece other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
    }

    /** @return last index of c, or -1 if c is not in this string */
    U_COMMON_API int32_t ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(ಅಕ್ಷರ c) ಸ್ಥಿರಾಂಕ;

    U_COMMON_API bool ಒಳಗೊಂಡಿದೆ(StringPiece s) ಸ್ಥಿರಾಂಕ;

    U_COMMON_API CharString &clear() { len=0; ಬಫರ್[0]=0; ಹಿಂತಿರುಗಿ *this; }
    U_COMMON_API CharString &truncate(int32_t new_ಉದ್ದಳತೆ);

    U_COMMON_API CharString &append(ಅಕ್ಷರ c, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    U_COMMON_API CharString &append(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಹಿಂತಿರುಗಿ append(s.ದತ್ತಾಂಶ(), s.ಉದ್ದಳತೆ(), error_ಸಂಕೇತ);
    }
    U_COMMON_API CharString &append(ಸ್ಥಿರಾಂಕ CharString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಹಿಂತಿರುಗಿ append(s.ದತ್ತಾಂಶ(), s.ಉದ್ದಳತೆ(), error_ಸಂಕೇತ);
    }
    U_COMMON_API CharString &append(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t s_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    U_COMMON_API CharString &append_ಸಂಖ್ಯೆ(int64_t ಸಂಖ್ಯೆ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

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
    U_COMMON_API ಅಕ್ಷರ *get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ,
                                       int32_t desired_ಸಾಮರ್ಥ್ಯ_Hint,
                                       int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
                                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    U_COMMON_API CharString &appendInvariantChars(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    U_COMMON_API CharString &appendInvariantChars(ಸ್ಥಿರಾಂಕ char16_t* uchars,
                                                  int32_t ucharsLen,
                                                  UError_ಸಂಕೇತ& error_ಸಂಕೇತ);

    /**
     * Appends a filename/path part, e.g., a directory name.
     * First appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if necessary.
     * Does nothing if s is empty.
     */
    U_COMMON_API CharString &append_ಮಾರ್ಗ_Part(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /**
     * Appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if this string is not empty
     * and does not already end with a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR.
     */
    U_COMMON_API CharString &ensure_ತುದಿಗಳು_With_File_Separator(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

ಖಾಸಗಿ:
    Maybe_ರಾಶಿ_Array<ಅಕ್ಷರ, 40> ಬಫರ್;
    int32_t len;

    UBool ensure_ಸಾಮರ್ಥ್ಯ(int32_t ಸಾಮರ್ಥ್ಯ, int32_t desired_ಸಾಮರ್ಥ್ಯ_Hint, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    CharString(ಸ್ಥಿರಾಂಕ CharString &other) = delete; // forbid copying of this class
    CharString &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ CharString &other) = delete; // forbid copying of this class

    /**
     * Returns U_FILE_ALT_SEP_CHAR if found in string, and U_FILE_SEP_CHAR is not found.
     * Otherwise returns U_FILE_SEP_CHAR.
     */
    ಅಕ್ಷರ getDirSepChar() ಸ್ಥಿರಾಂಕ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
//eof
