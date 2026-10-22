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

#اگر_غیر_تعریف_شدہ CHARSTRING_H
#تعریف_کرو CHARSTRING_H

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uobject.h"
#شامل "cmemory.h"

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
class U_COMMON_API_CLASS CharString : عوامی UMemory {
عوامی:
    U_COMMON_API CharString() : len(0) { بفر[0]=0; }
    U_COMMON_API CharString(StringPiece s, UError_رمز &error_رمز) : len(0) {
        بفر[0]=0;
        append(s, error_رمز);
    }
    U_COMMON_API CharString(ثابت CharString &s, UError_رمز &error_رمز) : len(0) {
        بفر[0]=0;
        append(s, error_رمز);
    }
    U_COMMON_API CharString(ثابت حرف *s, int32_t s_لمبائی, UError_رمز &error_رمز) : len(0) {
        بفر[0]=0;
        append(s, s_لمبائی, error_رمز);
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
    U_COMMON_API CharString &عامل=(CharString &&src) noexcept;

    /**
     * Replaces this string's contents with the other string's contents.
     * CharString does not support the standard copy constructor nor
     * the assignment operator, to make copies explicit and to
     * use a UErrorCode where memory allocations might be needed.
     */
    U_COMMON_API CharString &copyFrom(ثابت CharString &other, UError_رمز &error_رمز);
    U_COMMON_API CharString &copyFrom(StringPiece s, UError_رمز &error_رمز);

    U_COMMON_API UBool is_خالی_خانے() ثابت { واپس len==0; }
    U_COMMON_API int32_t لمبائی() ثابت { واپس len; }
    U_COMMON_API حرف عامل[](int32_t اشاریہ) ثابت { واپس بفر[اشاریہ]; }
    U_COMMON_API StringPiece مقصد_String_Piece() ثابت { واپس StringPiece(بفر.getAlias(), len); }

    U_COMMON_API ثابت حرف *ڈیٹا() ثابت { واپس بفر.getAlias(); }
    U_COMMON_API حرف *ڈیٹا() { واپس بفر.getAlias(); }
    /**
     * Allocates length()+1 chars and copies the NUL-terminated data().
     * The caller must uprv_free() the result.
     */
    U_COMMON_API حرف *clone_ڈیٹا(UError_رمز &error_رمز) ثابت;
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
    U_COMMON_API int32_t نکالو(حرف *dest, int32_t استعداد, UError_رمز &error_رمز) ثابت;

    U_COMMON_API bool عامل==(ثابت CharString& other) ثابت {
        واپس len == other.لمبائی() && (len == 0 || uprv_memcmp(ڈیٹا(), other.ڈیٹا(), len) == 0);
    }
    U_COMMON_API bool عامل!=(ثابت CharString& other) ثابت {
        واپس !عامل==(other);
    }

    U_COMMON_API bool عامل==(StringPiece other) ثابت {
        واپس len == other.لمبائی() && (len == 0 || uprv_memcmp(ڈیٹا(), other.ڈیٹا(), len) == 0);
    }
    U_COMMON_API bool عامل!=(StringPiece other) ثابت {
        واپس !عامل==(other);
    }

    /** @return last index of c, or -1 if c is not in this string */
    U_COMMON_API int32_t آخری_اشاریہ_کا(حرف c) ثابت;

    U_COMMON_API bool شامل_ہے(StringPiece s) ثابت;

    U_COMMON_API CharString &clear() { len=0; بفر[0]=0; واپس *this; }
    U_COMMON_API CharString &truncate(int32_t new_لمبائی);

    U_COMMON_API CharString &append(حرف c, UError_رمز &error_رمز);
    U_COMMON_API CharString &append(StringPiece s, UError_رمز &error_رمز) {
        واپس append(s.ڈیٹا(), s.لمبائی(), error_رمز);
    }
    U_COMMON_API CharString &append(ثابت CharString &s, UError_رمز &error_رمز) {
        واپس append(s.ڈیٹا(), s.لمبائی(), error_رمز);
    }
    U_COMMON_API CharString &append(ثابت حرف *s, int32_t s_لمبائی, UError_رمز &حالت_2);

    U_COMMON_API CharString &append_نمبر(int64_t نمبر, UError_رمز &حالت_2);

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
    U_COMMON_API حرف *get_Append_بفر(int32_t min_استعداد,
                                       int32_t desired_استعداد_Hint,
                                       int32_t &نتیجہ_استعداد,
                                       UError_رمز &error_رمز);

    U_COMMON_API CharString &appendInvariantChars(ثابت UnicodeString &s, UError_رمز &error_رمز);
    U_COMMON_API CharString &appendInvariantChars(ثابت char16_t* uchars,
                                                  int32_t ucharsLen,
                                                  UError_رمز& error_رمز);

    /**
     * Appends a filename/path part, e.g., a directory name.
     * First appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if necessary.
     * Does nothing if s is empty.
     */
    U_COMMON_API CharString &append_راستہ_Part(StringPiece s, UError_رمز &error_رمز);

    /**
     * Appends a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR if this string is not empty
     * and does not already end with a U_FILE_SEP_CHAR or U_FILE_ALT_SEP_CHAR.
     */
    U_COMMON_API CharString &ensure_سرے_With_File_Separator(UError_رمز &error_رمز);

نجی:
    Maybe_ڈھیر_Array<حرف, 40> بفر;
    int32_t len;

    UBool ensure_استعداد(int32_t استعداد, int32_t desired_استعداد_Hint, UError_رمز &error_رمز);

    CharString(ثابت CharString &other) = delete; // forbid copying of this class
    CharString &عامل=(ثابت CharString &other) = delete; // forbid copying of this class

    /**
     * Returns U_FILE_ALT_SEP_CHAR if found in string, and U_FILE_SEP_CHAR is not found.
     * Otherwise returns U_FILE_SEP_CHAR.
     */
    حرف getDirSepChar() ثابت;
};

U_NAMESPACE_انتہا

#اگر_ختم
//eof
