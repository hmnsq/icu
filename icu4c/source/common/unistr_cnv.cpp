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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/putil.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/ucnv.h"
#ضمن "ucnv_imp.h"
#ضمن "putilimp.h"
#ضمن "ustr_cnv.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#إذا !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    إذا(codepage_انکڑے != 0) {
        doCodepageCreate(codepage_انکڑے, (int32_t)uprv_strlen(codepage_انکڑے), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے,
                             int32_t انکڑے_لمائی) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    إذا(codepage_انکڑے != 0) {
        doCodepageCreate(codepage_انکڑے, انکڑے_لمائی, 0);
    }
}

// else see unistr.cpp
#جے_انت

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    إذا (codepage_انکڑے != خالی_سوچک) {
        doCodepageCreate(codepage_انکڑے, static_cast<int32_t>(uprv_strlen(codepage_انکڑے)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے,
                             int32_t انکڑے_لمائی,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    إذا (codepage_انکڑے != خالی_سوچک) {
        doCodepageCreate(codepage_انکڑے, انکڑے_لمائی, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t src_لمائی,
                             UConverter *cnv,
                             UError_کوڈ &error_کوڈ) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    إذا(U_SUCCESS(error_کوڈ)) {
        // check arguments
        إذا(src==خالی_سوچک) {
            // treat as an empty string, do nothing more
        } وإلا إذا(src_لمائی<-1) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        } وإلا {
            // get input length
            إذا(src_لمائی==-1) {
                src_لمائی = static_cast<int32_t>(uprv_strlen(src));
            }
            إذا(src_لمائی>0) {
                إذا (cnv != خالی_سوچک) {
                    // use the provided converter
                    ucnv_reset_منزل_Unicode(cnv);
                    doCodepageCreate(src, src_لمائی, cnv, error_کوڈ);
                } وإلا {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_کوڈ);
                    doCodepageCreate(src, src_لمائی, cnv, error_کوڈ);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        إذا(U_FAILURE(error_کوڈ)) {
            سیٹ_منزل_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#إذا !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t شروع,
                       int32_t length,
                       حرف *نشانہ,
                       uint32_t dst_ناپ) ثابت {
    أرجع extract(شروع, length, نشانہ, dst_ناپ, 0);
}

// else see unistr.cpp
#جے_انت

int32_t
UnicodeString::extract(int32_t شروع,
                       int32_t length,
                       حرف *نشانہ,
                       uint32_t dst_ناپ,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    إذا (/*dstSize < 0 || */(dst_ناپ > 0 && نشانہ == خالی_سوچک)) {
        أرجع 0;
    }

    // pin the indices to legal values
    pinIndices(شروع, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    إذا(dst_ناپ < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_ناپ);
    } وإلا {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* نشانہ_حد_بندی = static_cast<حرف*>(U_MAX_PTR(نشانہ));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(نشانہ_حد_بندی - نشانہ);
    }

    // create the converter
    UConverter *converter;
    UError_کوڈ حالت = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    إذا(length == 0) {
        أرجع u_terminateChars(نشانہ, capacity, 0, &حالت);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    إذا (codepage == خالی_سوچک) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            أرجع منزل_UTF8(شروع, length, نشانہ, capacity);
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_لمائی;
        إذا(length <= capacity) {
            dest_لمائی = length;
        } وإلا {
            dest_لمائی = capacity;
        }
        u_UChars_منزل_Chars(get_Array_شروع() + شروع, نشانہ, dest_لمائی);
        أرجع u_terminateChars(نشانہ, capacity, length, &حالت);
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    length = do_کڈھو(شروع, length, نشانہ, capacity, converter, حالت);

    // close the converter
    إذا (codepage == خالی_سوچک) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }

    أرجع length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t dest_سمرتھا,
                       UConverter *cnv,
                       UError_کوڈ &error_کوڈ) ثابت
{
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }

    إذا (isBogus() || dest_سمرتھا < 0 || (dest_سمرتھا > 0 && dest == خالی_سوچک)) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    // nothing to do?
    إذا(is_خالی_تھاواں()) {
        أرجع u_terminateChars(dest, dest_سمرتھا, 0, &error_کوڈ);
    }

    // get the converter
    UBool isDefaultConverter;
    إذا (cnv == خالی_سوچک) {
        isDefaultConverter=سچ;
        cnv=u_getDefaultConverter(&error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
    } وإلا {
        isDefaultConverter=خطأ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_کڈھو(0, length(), dest, dest_سمرتھا, cnv, error_کوڈ);

    // release the converter
    إذا(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    أرجع len;
}

int32_t
UnicodeString::do_کڈھو(int32_t شروع, int32_t length,
                         حرف *dest, int32_t dest_سمرتھا,
                         UConverter *cnv,
                         UError_کوڈ &error_کوڈ) ثابت
{
    إذا(U_FAILURE(error_کوڈ)) {
        إذا(dest_سمرتھا!=0) {
            *dest=0;
        }
        أرجع 0;
    }

    ثابت char16_t *src=get_Array_شروع()+شروع, *src_حد_بندی=src+length;
    حرف *originalDest=dest;
    ثابت حرف *dest_حد_بندی;

    إذا(dest_سمرتھا==0) {
        dest_حد_بندی=dest=خالی_سوچک;
    } وإلا إذا(dest_سمرتھا==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_حد_بندی = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_سمرتھا=0x7fffffff;
    } وإلا {
        dest_حد_بندی=dest+dest_سمرتھا;
    }

    // perform the conversion
    UError_کوڈ بفر_حالت = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR) {
        حرف بفر[1024];

        dest_حد_بندی=بفر+حجم(بفر);
        افعل {
            dest=بفر;
            بفر_حالت=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_سوچک, سچ, &بفر_حالت);
            length += static_cast<int32_t>(dest - بفر);
        } طالما(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR);
    }
    إذا (U_FAILURE(بفر_حالت)) {
        error_کوڈ = بفر_حالت;
    }

    أرجع u_terminateChars(originalDest, dest_سمرتھا, length, &error_کوڈ);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_انکڑے,
                                int32_t انکڑے_لمائی,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    إذا (codepage_انکڑے == خالی_سوچک || انکڑے_لمائی == 0 || انکڑے_لمائی < -1) {
        أرجع;
    }
    إذا(انکڑے_لمائی == -1) {
        انکڑے_لمائی = static_cast<int32_t>(uprv_strlen(codepage_انکڑے));
    }

    UError_کوڈ حالت = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    إذا (codepage == خالی_سوچک) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            سیٹ_منزل_UTF8(StringPiece(codepage_انکڑے, انکڑے_لمائی));
            أرجع;
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        إذا(cloneArrayIfNeeded(انکڑے_لمائی, انکڑے_لمائی, خطأ)) {
            u_chars_منزل_UChars(codepage_انکڑے, get_Array_شروع(), انکڑے_لمائی);
            سیٹ_لمائی(انکڑے_لمائی);
        } وإلا {
            سیٹ_منزل_Bogus();
        }
        أرجع;
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    // if we failed, set the appropriate flags and return
    إذا(U_FAILURE(حالت)) {
        سیٹ_منزل_Bogus();
        أرجع;
    }

    // perform the conversion
    doCodepageCreate(codepage_انکڑے, انکڑے_لمائی, converter, حالت);
    إذا(U_FAILURE(حالت)) {
        سیٹ_منزل_Bogus();
    }

    // close the converter
    إذا (codepage == خالی_سوچک) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_انکڑے,
                                int32_t انکڑے_لمائی,
                                UConverter *converter,
                                UError_کوڈ &حالت)
{
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }

    // set up the conversion parameters
    ثابت حرف *my_سروت     = codepage_انکڑے;
    ثابت حرف *my_سروت_سرا  = my_سروت + انکڑے_لمائی;
    char16_t *array, *my_نشانہ;

    // estimate the size needed:
    int32_t array_ناپ;
    إذا(انکڑے_لمائی <= US_STACKBUF_ناپ) {
        // try to use the stack buffer
        array_ناپ = US_STACKBUF_ناپ;
    } وإلا {
        // 1.25 char16_t's per source byte should cover most cases
        array_ناپ = انکڑے_لمائی + (انکڑے_لمائی >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = خطأ;
    لكل(;;) {
        إذا(!cloneArrayIfNeeded(array_ناپ, array_ناپ, doCopyArray)) {
            سیٹ_منزل_Bogus();
            اكسر;
        }

        // perform the conversion
        array = get_Array_شروع();
        my_نشانہ = array + length();
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(converter, &my_نشانہ,  array + get_سمرتھا(),
            &my_سروت, my_سروت_سرا, خالی_سوچک, سچ, &بفر_حالت);

        // update the conversion parameters
        سیٹ_لمائی(static_cast<int32_t>(my_نشانہ - array));

        // allocate more space and copy data, if needed
        إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR) {
            // keep the previous conversion results
            doCopyArray = سچ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_ناپ = static_cast<int32_t>(length() + 2 * (my_سروت_سرا - my_سروت));
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                حالت = بفر_حالت;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_سرا

#جے_انت
