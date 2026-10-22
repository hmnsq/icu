// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistr_cnv.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004aug19
*   created by: Markus W. Scherer
*
*   Character conversion functions moved here from unistr.cpp
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#ಆದರೆ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಆದರೆ(codepage_ದತ್ತಾಂಶ != 0) {
        doCodepageCreate(codepage_ದತ್ತಾಂಶ, (int32_t)uprv_strlen(codepage_ದತ್ತಾಂಶ), 0);
    }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ,
                             int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಆದರೆ(codepage_ದತ್ತಾಂಶ != 0) {
        doCodepageCreate(codepage_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, 0);
    }
}

// else see unistr.cpp
#ಆದರೆ_ಅಂತ್ಯ

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಆದರೆ (codepage_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        doCodepageCreate(codepage_ದತ್ತಾಂಶ, static_cast<int32_t>(uprv_strlen(codepage_ದತ್ತಾಂಶ)), codepage);
    }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ,
                             int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಆದರೆ (codepage_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        doCodepageCreate(codepage_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, codepage);
    }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                             UConverter *cnv,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        // check arguments
        ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ) {
            // treat as an empty string, do nothing more
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(src_ಉದ್ದಳತೆ<-1) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // get input length
            ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
                src_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(src));
            }
            ಆದರೆ(src_ಉದ್ದಳತೆ>0) {
                ಆದರೆ (cnv != ಶೂನ್ಯಸೂಚಕ) {
                    // use the provided converter
                    ucnv_reset_ಗಮ್ಯ_Unicode(cnv);
                    doCodepageCreate(src, src_ಉದ್ದಳತೆ, cnv, error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_ಸಂಕೇತ);
                    doCodepageCreate(src, src_ಉದ್ದಳತೆ, cnv, error_ಸಂಕೇತ);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#ಆದರೆ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ಆರಂಭ,
                       int32_t length,
                       ಅಕ್ಷರ *ಗುರಿ,
                       uint32_t dst_ಗಾತ್ರ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ extract(ಆರಂಭ, length, ಗುರಿ, dst_ಗಾತ್ರ, 0);
}

// else see unistr.cpp
#ಆದರೆ_ಅಂತ್ಯ

int32_t
UnicodeString::extract(int32_t ಆರಂಭ,
                       int32_t length,
                       ಅಕ್ಷರ *ಗುರಿ,
                       uint32_t dst_ಗಾತ್ರ,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage) ಸ್ಥಿರಾಂಕ
{
    // if the arguments are illegal, then do nothing
    ಆದರೆ (/*dstSize < 0 || */(dst_ಗಾತ್ರ > 0 && ಗುರಿ == ಶೂನ್ಯಸೂಚಕ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    // pin the indices to legal values
    pinIndices(ಆರಂಭ, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    ಆದರೆ(dst_ಗಾತ್ರ < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_ಗಾತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Pin the capacity so that a limit pointer does not wrap around.
        ಅಕ್ಷರ* ಗುರಿ_ಎಲ್ಲೆ = static_cast<ಅಕ್ಷರ*>(U_MAX_PTR(ಗುರಿ));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
    }

    // create the converter
    UConverter *converter;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    ಆದರೆ(length == 0) {
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, capacity, 0, &ಸ್ಥಿತಿ_2);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    ಆದರೆ (codepage == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *defaultName = ucnv_getDefaultName();
        ಆದರೆ(UCNV_FAST_IS_UTF8(defaultName)) {
            ಹಿಂತಿರುಗಿ ಗಮ್ಯ_UTF8(ಆರಂಭ, length, ಗುರಿ, capacity);
        }
        converter = u_getDefaultConverter(&ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_ಉದ್ದಳತೆ;
        ಆದರೆ(length <= capacity) {
            dest_ಉದ್ದಳತೆ = length;
        } ಇಲ್ಲದಿದ್ದರೆ {
            dest_ಉದ್ದಳತೆ = capacity;
        }
        u_UChars_ಗಮ್ಯ_Chars(get_Array_ಆರಂಭ() + ಆರಂಭ, ಗುರಿ, dest_ಉದ್ದಳತೆ);
        ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, capacity, length, &ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        converter = ucnv_open(codepage, &ಸ್ಥಿತಿ_2);
    }

    length = do_ಹೊರತೆಗೆ(ಆರಂಭ, length, ಗುರಿ, capacity, converter, ಸ್ಥಿತಿ_2);

    // close the converter
    ಆದರೆ (codepage == ಶೂನ್ಯಸೂಚಕ) {
        u_releaseDefaultConverter(converter);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_close(converter);
    }

    ಹಿಂತಿರುಗಿ length;
}

int32_t
UnicodeString::extract(ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                       UConverter *cnv,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (isBogus() || dest_ಸಾಮರ್ಥ್ಯ < 0 || (dest_ಸಾಮರ್ಥ್ಯ > 0 && dest == ಶೂನ್ಯಸೂಚಕ)) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    // nothing to do?
    ಆದರೆ(is_ಖಾಲಿಗಳು()) {
        ಹಿಂತಿರುಗಿ u_terminateChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, &error_ಸಂಕೇತ);
    }

    // get the converter
    UBool isDefaultConverter;
    ಆದರೆ (cnv == ಶೂನ್ಯಸೂಚಕ) {
        isDefaultConverter=ನಿಜ;
        cnv=u_getDefaultConverter(&error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        isDefaultConverter=ಸುಳ್ಳು;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_ಹೊರತೆಗೆ(0, length(), dest, dest_ಸಾಮರ್ಥ್ಯ, cnv, error_ಸಂಕೇತ);

    // release the converter
    ಆದರೆ(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ಹಿಂತಿರುಗಿ len;
}

int32_t
UnicodeString::do_ಹೊರತೆಗೆ(int32_t ಆರಂಭ, int32_t length,
                         ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                         UConverter *cnv,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ!=0) {
            *dest=0;
        }
        ಹಿಂತಿರುಗಿ 0;
    }

    ಸ್ಥಿರಾಂಕ char16_t *src=get_Array_ಆರಂಭ()+ಆರಂಭ, *src_ಎಲ್ಲೆ=src+length;
    ಅಕ್ಷರ *originalDest=dest;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dest_ಎಲ್ಲೆ;

    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ==0) {
        dest_ಎಲ್ಲೆ=dest=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_ಎಲ್ಲೆ = static_cast<ಅಕ್ಷರ*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_ಸಾಮರ್ಥ್ಯ=0x7fffffff;
    } ಇಲ್ಲದಿದ್ದರೆ {
        dest_ಎಲ್ಲೆ=dest+dest_ಸಾಮರ್ಥ್ಯ;
    }

    // perform the conversion
    UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಅಕ್ಷರ ಬಫರ್[1024];

        dest_ಎಲ್ಲೆ=ಬಫರ್+ಗಾತ್ರ(ಬಫರ್);
        ಮಾಡು {
            dest=ಬಫರ್;
            ಬಫರ್_ಸ್ಥಿತಿ=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_ಎಲ್ಲೆ, &src, src_ಎಲ್ಲೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);
            length += static_cast<int32_t>(dest - ಬಫರ್);
        } ಆಗ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR);
    }
    ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
        error_ಸಂಕೇತ = ಬಫರ್_ಸ್ಥಿತಿ;
    }

    ಹಿಂತಿರುಗಿ u_terminateChars(originalDest, dest_ಸಾಮರ್ಥ್ಯ, length, &error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
UnicodeString::doCodepageCreate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ,
                                int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage)
{
    // if there's nothing to convert, do nothing
    ಆದರೆ (codepage_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == 0 || ದತ್ತಾಂಶ_ಉದ್ದಳತೆ < -1) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == -1) {
        ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(codepage_ದತ್ತಾಂಶ));
    }

    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    ಆದರೆ (codepage == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *defaultName = ucnv_getDefaultName();
        ಆದರೆ(UCNV_FAST_IS_UTF8(defaultName)) {
            ಗಣ_ಗಮ್ಯ_UTF8(StringPiece(codepage_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ));
            ಹಿಂತಿರುಗಿ;
        }
        converter = u_getDefaultConverter(&ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*codepage == 0) {
        // use the "invariant characters" conversion
        ಆದರೆ(cloneArrayIfNeeded(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ಸುಳ್ಳು)) {
            u_chars_ಗಮ್ಯ_UChars(codepage_ದತ್ತಾಂಶ, get_Array_ಆರಂಭ(), ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
            ಗಣ_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        converter = ucnv_open(codepage, &ಸ್ಥಿತಿ_2);
    }

    // if we failed, set the appropriate flags and return
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ;
    }

    // perform the conversion
    doCodepageCreate(codepage_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, converter, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಗಣ_ಗಮ್ಯ_Bogus();
    }

    // close the converter
    ಆದರೆ (codepage == ಶೂನ್ಯಸೂಚಕ) {
        u_releaseDefaultConverter(converter);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_close(converter);
    }
}

ಶೂನ್ಯ
UnicodeString::doCodepageCreate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ,
                                int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ,
                                UConverter *converter,
                                UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    // set up the conversion parameters
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ     = codepage_ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಕೊನೆ  = my_ಮೂಲ + ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    char16_t *array, *my_ಗುರಿ;

    // estimate the size needed:
    int32_t array_ಗಾತ್ರ;
    ಆದರೆ(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ <= US_STACKBUF_ಗಾತ್ರ) {
        // try to use the stack buffer
        array_ಗಾತ್ರ = US_STACKBUF_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // 1.25 char16_t's per source byte should cover most cases
        array_ಗಾತ್ರ = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ + (ದತ್ತಾಂಶ_ಉದ್ದಳತೆ >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = ಸುಳ್ಳು;
    ಗಾಗಿ(;;) {
        ಆದರೆ(!cloneArrayIfNeeded(array_ಗಾತ್ರ, array_ಗಾತ್ರ, doCopyArray)) {
            ಗಣ_ಗಮ್ಯ_Bogus();
            ನಿಲ್ಲಿಸು;
        }

        // perform the conversion
        array = get_Array_ಆರಂಭ();
        my_ಗುರಿ = array + length();
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        ucnv_ಗಮ್ಯ_Unicode(converter, &my_ಗುರಿ,  array + get_ಸಾಮರ್ಥ್ಯ(),
            &my_ಮೂಲ, my_ಮೂಲ_ಕೊನೆ, ಶೂನ್ಯಸೂಚಕ, ನಿಜ, &ಬಫರ್_ಸ್ಥಿತಿ);

        // update the conversion parameters
        ಗಣ_ಉದ್ದಳತೆ(static_cast<int32_t>(my_ಗುರಿ - array));

        // allocate more space and copy data, if needed
        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
            // keep the previous conversion results
            doCopyArray = ನಿಜ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_ಗಾತ್ರ = static_cast<int32_t>(length() + 2 * (my_ಮೂಲ_ಕೊನೆ - my_ಮೂಲ));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
                ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
            }
            ನಿಲ್ಲಿಸು;
        }
    }
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
