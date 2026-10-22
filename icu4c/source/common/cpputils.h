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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CPPUTILS_H
#ವ್ಯಾಖ್ಯಾನಿಸು CPPUTILS_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ* src, ದ್ವಿಗುಣ* dst, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ* src, int32_t src_ಆರಂಭ,
              ದ್ವಿಗುಣ* dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int8_t* src, int8_t* dst, int32_t ಎಣಿಕೆ_2)
    { uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int8_t* src, int32_t src_ಆರಂಭ,
              int8_t* dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int16_t* src, int16_t* dst, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int16_t* src, int32_t src_ಆರಂಭ,
              int16_t* dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int32_t* src, int32_t* dst, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ uprv_arrayCopy(ಸ್ಥಿರಾಂಕ int32_t* src, int32_t src_ಆರಂಭ,
              int32_t* dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ
uprv_arrayCopy(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಆರಂಭ,
        char16_t *dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_memcpy(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)ಎಣಿಕೆ_2 * ಗಾತ್ರ(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
uprv_arrayCopy(ಸ್ಥಿರಾಂಕ icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ಎಣಿಕೆ_2)
{ ಆಗ(ಎಣಿಕೆ_2-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
uprv_arrayCopy(ಸ್ಥಿರಾಂಕ icu::UnicodeString *src, int32_t src_ಆರಂಭ,
               icu::UnicodeString *dst, int32_t dst_ಆರಂಭ, int32_t ಎಣಿಕೆ_2)
{ uprv_arrayCopy(src+src_ಆರಂಭ, dst+dst_ಆರಂಭ, ಎಣಿಕೆ_2); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
ಸಾಲೊಳಗೆ ಶೂನ್ಯ
uprv_check_Can_Get_ಬಫರ್(ಸ್ಥಿರಾಂಕ icu::UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && s.isBogus()) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* _CPPUTILS */
