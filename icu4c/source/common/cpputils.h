// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cpputils.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CPPUTILS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPPUTILS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ ਦੁੱਗਣਾ* src, ਦੁੱਗਣਾ* dst, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst, src, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ ਦੁੱਗਣਾ* src, int32_t src_ਸ਼ੁਰੂ,
              ਦੁੱਗਣਾ* dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int8_t* src, int8_t* dst, int32_t ਗਿਣਤੀ_2)
    { uprv_memcpy(dst, src, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int8_t* src, int32_t src_ਸ਼ੁਰੂ,
              int8_t* dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int16_t* src, int16_t* dst, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst, src, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int16_t* src, int32_t src_ਸ਼ੁਰੂ,
              int16_t* dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int32_t* src, int32_t* dst, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst, src, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ uprv_arrayCopy(ਅਚਲ int32_t* src, int32_t src_ਸ਼ੁਰੂ,
              int32_t* dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ
uprv_arrayCopy(ਅਚਲ char16_t *src, int32_t src_ਸ਼ੁਰੂ,
        char16_t *dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_memcpy(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)ਗਿਣਤੀ_2 * ਆਕਾਰ(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
uprv_arrayCopy(ਅਚਲ icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ਗਿਣਤੀ_2)
{ ਜਦੋਂ(ਗਿਣਤੀ_2-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
uprv_arrayCopy(ਅਚਲ icu::UnicodeString *src, int32_t src_ਸ਼ੁਰੂ,
               icu::UnicodeString *dst, int32_t dst_ਸ਼ੁਰੂ, int32_t ਗਿਣਤੀ_2)
{ uprv_arrayCopy(src+src_ਸ਼ੁਰੂ, dst+dst_ਸ਼ੁਰੂ, ਗਿਣਤੀ_2); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
ਸਤਰਅੰਦਰ ਖਾਲੀ
uprv_check_Can_Get_ਬਫਰ(ਅਚਲ icu::UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && s.isBogus()) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

#ਜੇ_ਅੰਤ /* _CPPUTILS */
