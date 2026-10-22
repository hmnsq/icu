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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#ਜੇ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਜੇ(codepage_ਅੰਕੜੇ != 0) {
        doCodepageCreate(codepage_ਅੰਕੜੇ, (int32_t)uprv_strlen(codepage_ਅੰਕੜੇ), 0);
    }
}

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ,
                             int32_t ਅੰਕੜੇ_ਲੰਮਾਈ) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਜੇ(codepage_ਅੰਕੜੇ != 0) {
        doCodepageCreate(codepage_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ, 0);
    }
}

// else see unistr.cpp
#ਜੇ_ਅੰਤ

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *codepage) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਜੇ (codepage_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        doCodepageCreate(codepage_ਅੰਕੜੇ, static_cast<int32_t>(uprv_strlen(codepage_ਅੰਕੜੇ)), codepage);
    }
}

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ,
                             int32_t ਅੰਕੜੇ_ਲੰਮਾਈ,
                             ਅਚਲ ਅੱਖਰ *codepage) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਜੇ (codepage_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        doCodepageCreate(codepage_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ, codepage);
    }
}

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                             UConverter *cnv,
                             UError_ਕੋਡ &error_ਕੋਡ) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        // check arguments
        ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ) {
            // treat as an empty string, do nothing more
        } ਨਹੀਂਤਾਂ ਜੇ(src_ਲੰਮਾਈ<-1) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        } ਨਹੀਂਤਾਂ {
            // get input length
            ਜੇ(src_ਲੰਮਾਈ==-1) {
                src_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(src));
            }
            ਜੇ(src_ਲੰਮਾਈ>0) {
                ਜੇ (cnv != ਖਾਲੀ_ਸੂਚਕ) {
                    // use the provided converter
                    ucnv_reset_ਮੰਜ਼ਿਲ_Unicode(cnv);
                    doCodepageCreate(src, src_ਲੰਮਾਈ, cnv, error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_ਕੋਡ);
                    doCodepageCreate(src, src_ਲੰਮਾਈ, cnv, error_ਕੋਡ);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#ਜੇ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ਸ਼ੁਰੂ,
                       int32_t length,
                       ਅੱਖਰ *ਨਿਸ਼ਾਨਾ,
                       uint32_t dst_ਮਾਪ) ਅਚਲ {
    ਵਾਪਸ extract(ਸ਼ੁਰੂ, length, ਨਿਸ਼ਾਨਾ, dst_ਮਾਪ, 0);
}

// else see unistr.cpp
#ਜੇ_ਅੰਤ

int32_t
UnicodeString::extract(int32_t ਸ਼ੁਰੂ,
                       int32_t length,
                       ਅੱਖਰ *ਨਿਸ਼ਾਨਾ,
                       uint32_t dst_ਮਾਪ,
                       ਅਚਲ ਅੱਖਰ *codepage) ਅਚਲ
{
    // if the arguments are illegal, then do nothing
    ਜੇ (/*dstSize < 0 || */(dst_ਮਾਪ > 0 && ਨਿਸ਼ਾਨਾ == ਖਾਲੀ_ਸੂਚਕ)) {
        ਵਾਪਸ 0;
    }

    // pin the indices to legal values
    pinIndices(ਸ਼ੁਰੂ, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    ਜੇ(dst_ਮਾਪ < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_ਮਾਪ);
    } ਨਹੀਂਤਾਂ {
        // Pin the capacity so that a limit pointer does not wrap around.
        ਅੱਖਰ* ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = static_cast<ਅੱਖਰ*>(U_MAX_PTR(ਨਿਸ਼ਾਨਾ));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
    }

    // create the converter
    UConverter *converter;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    ਜੇ(length == 0) {
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, capacity, 0, &ਸਥਿਤੀ);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    ਜੇ (codepage == ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ *defaultName = ucnv_getDefaultName();
        ਜੇ(UCNV_FAST_IS_UTF8(defaultName)) {
            ਵਾਪਸ ਮੰਜ਼ਿਲ_UTF8(ਸ਼ੁਰੂ, length, ਨਿਸ਼ਾਨਾ, capacity);
        }
        converter = u_getDefaultConverter(&ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ ਜੇ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_ਲੰਮਾਈ;
        ਜੇ(length <= capacity) {
            dest_ਲੰਮਾਈ = length;
        } ਨਹੀਂਤਾਂ {
            dest_ਲੰਮਾਈ = capacity;
        }
        u_UChars_ਮੰਜ਼ਿਲ_Chars(get_Array_ਸ਼ੁਰੂ() + ਸ਼ੁਰੂ, ਨਿਸ਼ਾਨਾ, dest_ਲੰਮਾਈ);
        ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, capacity, length, &ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        converter = ucnv_open(codepage, &ਸਥਿਤੀ);
    }

    length = do_ਕੱਢੋ(ਸ਼ੁਰੂ, length, ਨਿਸ਼ਾਨਾ, capacity, converter, ਸਥਿਤੀ);

    // close the converter
    ਜੇ (codepage == ਖਾਲੀ_ਸੂਚਕ) {
        u_releaseDefaultConverter(converter);
    } ਨਹੀਂਤਾਂ {
        ucnv_close(converter);
    }

    ਵਾਪਸ length;
}

int32_t
UnicodeString::extract(ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                       UConverter *cnv,
                       UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ
{
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ (isBogus() || dest_ਸਮਰੱਥਾ < 0 || (dest_ਸਮਰੱਥਾ > 0 && dest == ਖਾਲੀ_ਸੂਚਕ)) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    // nothing to do?
    ਜੇ(is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਵਾਪਸ u_terminateChars(dest, dest_ਸਮਰੱਥਾ, 0, &error_ਕੋਡ);
    }

    // get the converter
    UBool isDefaultConverter;
    ਜੇ (cnv == ਖਾਲੀ_ਸੂਚਕ) {
        isDefaultConverter=ਸੱਚ;
        cnv=u_getDefaultConverter(&error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
    } ਨਹੀਂਤਾਂ {
        isDefaultConverter=ਝੂਠ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_ਕੱਢੋ(0, length(), dest, dest_ਸਮਰੱਥਾ, cnv, error_ਕੋਡ);

    // release the converter
    ਜੇ(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ਵਾਪਸ len;
}

int32_t
UnicodeString::do_ਕੱਢੋ(int32_t ਸ਼ੁਰੂ, int32_t length,
                         ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                         UConverter *cnv,
                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ
{
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਜੇ(dest_ਸਮਰੱਥਾ!=0) {
            *dest=0;
        }
        ਵਾਪਸ 0;
    }

    ਅਚਲ char16_t *src=get_Array_ਸ਼ੁਰੂ()+ਸ਼ੁਰੂ, *src_ਹੱਦਬੰਦੀ=src+length;
    ਅੱਖਰ *originalDest=dest;
    ਅਚਲ ਅੱਖਰ *dest_ਹੱਦਬੰਦੀ;

    ਜੇ(dest_ਸਮਰੱਥਾ==0) {
        dest_ਹੱਦਬੰਦੀ=dest=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(dest_ਸਮਰੱਥਾ==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_ਹੱਦਬੰਦੀ = static_cast<ਅੱਖਰ*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_ਸਮਰੱਥਾ=0x7fffffff;
    } ਨਹੀਂਤਾਂ {
        dest_ਹੱਦਬੰਦੀ=dest+dest_ਸਮਰੱਥਾ;
    }

    // perform the conversion
    UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਅੱਖਰ ਬਫਰ[1024];

        dest_ਹੱਦਬੰਦੀ=ਬਫਰ+ਆਕਾਰ(ਬਫਰ);
        ਕਰੋ {
            dest=ਬਫਰ;
            ਬਫਰ_ਸਥਿਤੀ=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_ਹੱਦਬੰਦੀ, &src, src_ਹੱਦਬੰਦੀ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);
            length += static_cast<int32_t>(dest - ਬਫਰ);
        } ਜਦੋਂ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR);
    }
    ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
        error_ਕੋਡ = ਬਫਰ_ਸਥਿਤੀ;
    }

    ਵਾਪਸ u_terminateChars(originalDest, dest_ਸਮਰੱਥਾ, length, &error_ਕੋਡ);
}

ਖਾਲੀ
UnicodeString::doCodepageCreate(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ,
                                int32_t ਅੰਕੜੇ_ਲੰਮਾਈ,
                                ਅਚਲ ਅੱਖਰ *codepage)
{
    // if there's nothing to convert, do nothing
    ਜੇ (codepage_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ_ਲੰਮਾਈ == 0 || ਅੰਕੜੇ_ਲੰਮਾਈ < -1) {
        ਵਾਪਸ;
    }
    ਜੇ(ਅੰਕੜੇ_ਲੰਮਾਈ == -1) {
        ਅੰਕੜੇ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(codepage_ਅੰਕੜੇ));
    }

    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    ਜੇ (codepage == ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ *defaultName = ucnv_getDefaultName();
        ਜੇ(UCNV_FAST_IS_UTF8(defaultName)) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_UTF8(StringPiece(codepage_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ));
            ਵਾਪਸ;
        }
        converter = u_getDefaultConverter(&ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ ਜੇ (*codepage == 0) {
        // use the "invariant characters" conversion
        ਜੇ(cloneArrayIfNeeded(ਅੰਕੜੇ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ, ਝੂਠ)) {
            u_chars_ਮੰਜ਼ਿਲ_UChars(codepage_ਅੰਕੜੇ, get_Array_ਸ਼ੁਰੂ(), ਅੰਕੜੇ_ਲੰਮਾਈ);
            ਸੈੱਟ_ਲੰਮਾਈ(ਅੰਕੜੇ_ਲੰਮਾਈ);
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ {
        converter = ucnv_open(codepage, &ਸਥਿਤੀ);
    }

    // if we failed, set the appropriate flags and return
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ;
    }

    // perform the conversion
    doCodepageCreate(codepage_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ, converter, ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }

    // close the converter
    ਜੇ (codepage == ਖਾਲੀ_ਸੂਚਕ) {
        u_releaseDefaultConverter(converter);
    } ਨਹੀਂਤਾਂ {
        ucnv_close(converter);
    }
}

ਖਾਲੀ
UnicodeString::doCodepageCreate(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ,
                                int32_t ਅੰਕੜੇ_ਲੰਮਾਈ,
                                UConverter *converter,
                                UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    // set up the conversion parameters
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ     = codepage_ਅੰਕੜੇ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਸਿਰਾ  = my_ਸਰੋਤ + ਅੰਕੜੇ_ਲੰਮਾਈ;
    char16_t *array, *my_ਨਿਸ਼ਾਨਾ;

    // estimate the size needed:
    int32_t array_ਮਾਪ;
    ਜੇ(ਅੰਕੜੇ_ਲੰਮਾਈ <= US_STACKBUF_ਮਾਪ) {
        // try to use the stack buffer
        array_ਮਾਪ = US_STACKBUF_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        // 1.25 char16_t's per source byte should cover most cases
        array_ਮਾਪ = ਅੰਕੜੇ_ਲੰਮਾਈ + (ਅੰਕੜੇ_ਲੰਮਾਈ >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = ਝੂਠ;
    ਲਈ(;;) {
        ਜੇ(!cloneArrayIfNeeded(array_ਮਾਪ, array_ਮਾਪ, doCopyArray)) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਰੋਕੋ;
        }

        // perform the conversion
        array = get_Array_ਸ਼ੁਰੂ();
        my_ਨਿਸ਼ਾਨਾ = array + length();
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        ucnv_ਮੰਜ਼ਿਲ_Unicode(converter, &my_ਨਿਸ਼ਾਨਾ,  array + get_ਸਮਰੱਥਾ(),
            &my_ਸਰੋਤ, my_ਸਰੋਤ_ਸਿਰਾ, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ, &ਬਫਰ_ਸਥਿਤੀ);

        // update the conversion parameters
        ਸੈੱਟ_ਲੰਮਾਈ(static_cast<int32_t>(my_ਨਿਸ਼ਾਨਾ - array));

        // allocate more space and copy data, if needed
        ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
            // keep the previous conversion results
            doCopyArray = ਸੱਚ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_ਮਾਪ = static_cast<int32_t>(length() + 2 * (my_ਸਰੋਤ_ਸਿਰਾ - my_ਸਰੋਤ));
        } ਨਹੀਂਤਾਂ {
            ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
                ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
            }
            ਰੋਕੋ;
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
