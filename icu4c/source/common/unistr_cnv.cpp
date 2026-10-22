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

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    إذا(codepage_معلومات != 0) {
        doCodepageCreate(codepage_معلومات, (int32_t)uprv_strlen(codepage_معلومات), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             int32_t معلومات_ڊيگهائي) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    إذا(codepage_معلومات != 0) {
        doCodepageCreate(codepage_معلومات, معلومات_ڊيگهائي, 0);
    }
}

// else see unistr.cpp
#جيڪڏهن_پورو

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    إذا (codepage_معلومات != خالي_اشارو) {
        doCodepageCreate(codepage_معلومات, static_cast<int32_t>(uprv_strlen(codepage_معلومات)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             int32_t معلومات_ڊيگهائي,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    إذا (codepage_معلومات != خالي_اشارو) {
        doCodepageCreate(codepage_معلومات, معلومات_ڊيگهائي, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t src_ڊيگهائي,
                             UConverter *cnv,
                             UError_ڪوڊ &error_ڪوڊ) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    إذا(U_SUCCESS(error_ڪوڊ)) {
        // check arguments
        إذا(src==خالي_اشارو) {
            // treat as an empty string, do nothing more
        } وإلا إذا(src_ڊيگهائي<-1) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        } وإلا {
            // get input length
            إذا(src_ڊيگهائي==-1) {
                src_ڊيگهائي = static_cast<int32_t>(uprv_strlen(src));
            }
            إذا(src_ڊيگهائي>0) {
                إذا (cnv != خالي_اشارو) {
                    // use the provided converter
                    ucnv_reset_منزل_Unicode(cnv);
                    doCodepageCreate(src, src_ڊيگهائي, cnv, error_ڪوڊ);
                } وإلا {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_ڪوڊ);
                    doCodepageCreate(src, src_ڊيگهائي, cnv, error_ڪوڊ);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        إذا(U_FAILURE(error_ڪوڊ)) {
            سيٽ_منزل_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#إذا !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t شروعات,
                       int32_t length,
                       حرف *هدف,
                       uint32_t dst_سائيز) ثابت {
    أرجع extract(شروعات, length, هدف, dst_سائيز, 0);
}

// else see unistr.cpp
#جيڪڏهن_پورو

int32_t
UnicodeString::extract(int32_t شروعات,
                       int32_t length,
                       حرف *هدف,
                       uint32_t dst_سائيز,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    إذا (/*dstSize < 0 || */(dst_سائيز > 0 && هدف == خالي_اشارو)) {
        أرجع 0;
    }

    // pin the indices to legal values
    pinIndices(شروعات, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    إذا(dst_سائيز < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_سائيز);
    } وإلا {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* هدف_حد_بندي = static_cast<حرف*>(U_MAX_PTR(هدف));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(هدف_حد_بندي - هدف);
    }

    // create the converter
    UConverter *converter;
    UError_ڪوڊ حالت = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    إذا(length == 0) {
        أرجع u_terminateChars(هدف, capacity, 0, &حالت);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    إذا (codepage == خالي_اشارو) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            أرجع منزل_UTF8(شروعات, length, هدف, capacity);
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_ڊيگهائي;
        إذا(length <= capacity) {
            dest_ڊيگهائي = length;
        } وإلا {
            dest_ڊيگهائي = capacity;
        }
        u_UChars_منزل_Chars(get_Array_شروعات() + شروعات, هدف, dest_ڊيگهائي);
        أرجع u_terminateChars(هدف, capacity, length, &حالت);
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    length = do_ڪڍو(شروعات, length, هدف, capacity, converter, حالت);

    // close the converter
    إذا (codepage == خالي_اشارو) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }

    أرجع length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t dest_صلاحيت,
                       UConverter *cnv,
                       UError_ڪوڊ &error_ڪوڊ) ثابت
{
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }

    إذا (isBogus() || dest_صلاحيت < 0 || (dest_صلاحيت > 0 && dest == خالي_اشارو)) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    // nothing to do?
    إذا(is_خالي_جايون()) {
        أرجع u_terminateChars(dest, dest_صلاحيت, 0, &error_ڪوڊ);
    }

    // get the converter
    UBool isDefaultConverter;
    إذا (cnv == خالي_اشارو) {
        isDefaultConverter=سچ;
        cnv=u_getDefaultConverter(&error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
    } وإلا {
        isDefaultConverter=خطأ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_ڪڍو(0, length(), dest, dest_صلاحيت, cnv, error_ڪوڊ);

    // release the converter
    إذا(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    أرجع len;
}

int32_t
UnicodeString::do_ڪڍو(int32_t شروعات, int32_t length,
                         حرف *dest, int32_t dest_صلاحيت,
                         UConverter *cnv,
                         UError_ڪوڊ &error_ڪوڊ) ثابت
{
    إذا(U_FAILURE(error_ڪوڊ)) {
        إذا(dest_صلاحيت!=0) {
            *dest=0;
        }
        أرجع 0;
    }

    ثابت char16_t *src=get_Array_شروعات()+شروعات, *src_حد_بندي=src+length;
    حرف *originalDest=dest;
    ثابت حرف *dest_حد_بندي;

    إذا(dest_صلاحيت==0) {
        dest_حد_بندي=dest=خالي_اشارو;
    } وإلا إذا(dest_صلاحيت==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_حد_بندي = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_صلاحيت=0x7fffffff;
    } وإلا {
        dest_حد_بندي=dest+dest_صلاحيت;
    }

    // perform the conversion
    UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR) {
        حرف بفر[1024];

        dest_حد_بندي=بفر+حجم(بفر);
        افعل {
            dest=بفر;
            بفر_حالت=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_حد_بندي, &src, src_حد_بندي, خالي_اشارو, سچ, &بفر_حالت);
            length += static_cast<int32_t>(dest - بفر);
        } طالما(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR);
    }
    إذا (U_FAILURE(بفر_حالت)) {
        error_ڪوڊ = بفر_حالت;
    }

    أرجع u_terminateChars(originalDest, dest_صلاحيت, length, &error_ڪوڊ);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_معلومات,
                                int32_t معلومات_ڊيگهائي,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    إذا (codepage_معلومات == خالي_اشارو || معلومات_ڊيگهائي == 0 || معلومات_ڊيگهائي < -1) {
        أرجع;
    }
    إذا(معلومات_ڊيگهائي == -1) {
        معلومات_ڊيگهائي = static_cast<int32_t>(uprv_strlen(codepage_معلومات));
    }

    UError_ڪوڊ حالت = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    إذا (codepage == خالي_اشارو) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            سيٽ_منزل_UTF8(StringPiece(codepage_معلومات, معلومات_ڊيگهائي));
            أرجع;
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        إذا(cloneArrayIfNeeded(معلومات_ڊيگهائي, معلومات_ڊيگهائي, خطأ)) {
            u_chars_منزل_UChars(codepage_معلومات, get_Array_شروعات(), معلومات_ڊيگهائي);
            سيٽ_ڊيگهائي(معلومات_ڊيگهائي);
        } وإلا {
            سيٽ_منزل_Bogus();
        }
        أرجع;
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    // if we failed, set the appropriate flags and return
    إذا(U_FAILURE(حالت)) {
        سيٽ_منزل_Bogus();
        أرجع;
    }

    // perform the conversion
    doCodepageCreate(codepage_معلومات, معلومات_ڊيگهائي, converter, حالت);
    إذا(U_FAILURE(حالت)) {
        سيٽ_منزل_Bogus();
    }

    // close the converter
    إذا (codepage == خالي_اشارو) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_معلومات,
                                int32_t معلومات_ڊيگهائي,
                                UConverter *converter,
                                UError_ڪوڊ &حالت)
{
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }

    // set up the conversion parameters
    ثابت حرف *my_ذريعو     = codepage_معلومات;
    ثابت حرف *my_ذريعو_پڇاڙي  = my_ذريعو + معلومات_ڊيگهائي;
    char16_t *array, *my_هدف;

    // estimate the size needed:
    int32_t array_سائيز;
    إذا(معلومات_ڊيگهائي <= US_STACKBUF_سائيز) {
        // try to use the stack buffer
        array_سائيز = US_STACKBUF_سائيز;
    } وإلا {
        // 1.25 char16_t's per source byte should cover most cases
        array_سائيز = معلومات_ڊيگهائي + (معلومات_ڊيگهائي >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = خطأ;
    لكل(;;) {
        إذا(!cloneArrayIfNeeded(array_سائيز, array_سائيز, doCopyArray)) {
            سيٽ_منزل_Bogus();
            اكسر;
        }

        // perform the conversion
        array = get_Array_شروعات();
        my_هدف = array + length();
        UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(converter, &my_هدف,  array + get_صلاحيت(),
            &my_ذريعو, my_ذريعو_پڇاڙي, خالي_اشارو, سچ, &بفر_حالت);

        // update the conversion parameters
        سيٽ_ڊيگهائي(static_cast<int32_t>(my_هدف - array));

        // allocate more space and copy data, if needed
        إذا(بفر_حالت == U_بفر_حد_کان_مٿي_ERROR) {
            // keep the previous conversion results
            doCopyArray = سچ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_سائيز = static_cast<int32_t>(length() + 2 * (my_ذريعو_پڇاڙي - my_ذريعو));
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                حالت = بفر_حالت;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
