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
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    إذا(codepage_معلومات != 0) {
        doCodepageCreate(codepage_معلومات, (int32_t)uprv_strlen(codepage_معلومات), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             int32_t معلومات_اوږدتیا) {
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    إذا(codepage_معلومات != 0) {
        doCodepageCreate(codepage_معلومات, معلومات_اوږدتیا, 0);
    }
}

// else see unistr.cpp
#د_که_پای

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    إذا (codepage_معلومات != تش_اشاره) {
        doCodepageCreate(codepage_معلومات, static_cast<int32_t>(uprv_strlen(codepage_معلومات)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات,
                             int32_t معلومات_اوږدتیا,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    إذا (codepage_معلومات != تش_اشاره) {
        doCodepageCreate(codepage_معلومات, معلومات_اوږدتیا, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t src_اوږدتیا,
                             UConverter *cnv,
                             UError_کوډ &error_کوډ) {
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    إذا(U_SUCCESS(error_کوډ)) {
        // check arguments
        إذا(src==تش_اشاره) {
            // treat as an empty string, do nothing more
        } وإلا إذا(src_اوږدتیا<-1) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
        } وإلا {
            // get input length
            إذا(src_اوږدتیا==-1) {
                src_اوږدتیا = static_cast<int32_t>(uprv_strlen(src));
            }
            إذا(src_اوږدتیا>0) {
                إذا (cnv != تش_اشاره) {
                    // use the provided converter
                    ucnv_reset_منزل_Unicode(cnv);
                    doCodepageCreate(src, src_اوږدتیا, cnv, error_کوډ);
                } وإلا {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_کوډ);
                    doCodepageCreate(src, src_اوږدتیا, cnv, error_کوډ);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        إذا(U_FAILURE(error_کوډ)) {
            سېټ_منزل_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#إذا !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t پیل,
                       int32_t length,
                       حرف *موخه,
                       uint32_t dst_اندازه) ثابت {
    أرجع extract(پیل, length, موخه, dst_اندازه, 0);
}

// else see unistr.cpp
#د_که_پای

int32_t
UnicodeString::extract(int32_t پیل,
                       int32_t length,
                       حرف *موخه,
                       uint32_t dst_اندازه,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    إذا (/*dstSize < 0 || */(dst_اندازه > 0 && موخه == تش_اشاره)) {
        أرجع 0;
    }

    // pin the indices to legal values
    pinIndices(پیل, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    إذا(dst_اندازه < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_اندازه);
    } وإلا {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* موخه_حد = static_cast<حرف*>(U_MAX_PTR(موخه));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(موخه_حد - موخه);
    }

    // create the converter
    UConverter *converter;
    UError_کوډ حالت = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    إذا(length == 0) {
        أرجع u_terminateChars(موخه, capacity, 0, &حالت);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    إذا (codepage == تش_اشاره) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            أرجع منزل_UTF8(پیل, length, موخه, capacity);
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_اوږدتیا;
        إذا(length <= capacity) {
            dest_اوږدتیا = length;
        } وإلا {
            dest_اوږدتیا = capacity;
        }
        u_UChars_منزل_Chars(get_Array_پیل() + پیل, موخه, dest_اوږدتیا);
        أرجع u_terminateChars(موخه, capacity, length, &حالت);
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    length = do_ایستل(پیل, length, موخه, capacity, converter, حالت);

    // close the converter
    إذا (codepage == تش_اشاره) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }

    أرجع length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t dest_ظرفیت,
                       UConverter *cnv,
                       UError_کوډ &error_کوډ) ثابت
{
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }

    إذا (isBogus() || dest_ظرفیت < 0 || (dest_ظرفیت > 0 && dest == تش_اشاره)) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    // nothing to do?
    إذا(is_تشه()) {
        أرجع u_terminateChars(dest, dest_ظرفیت, 0, &error_کوډ);
    }

    // get the converter
    UBool isDefaultConverter;
    إذا (cnv == تش_اشاره) {
        isDefaultConverter=ریښتیا;
        cnv=u_getDefaultConverter(&error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
    } وإلا {
        isDefaultConverter=خطأ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_ایستل(0, length(), dest, dest_ظرفیت, cnv, error_کوډ);

    // release the converter
    إذا(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    أرجع len;
}

int32_t
UnicodeString::do_ایستل(int32_t پیل, int32_t length,
                         حرف *dest, int32_t dest_ظرفیت,
                         UConverter *cnv,
                         UError_کوډ &error_کوډ) ثابت
{
    إذا(U_FAILURE(error_کوډ)) {
        إذا(dest_ظرفیت!=0) {
            *dest=0;
        }
        أرجع 0;
    }

    ثابت char16_t *src=get_Array_پیل()+پیل, *src_حد=src+length;
    حرف *originalDest=dest;
    ثابت حرف *dest_حد;

    إذا(dest_ظرفیت==0) {
        dest_حد=dest=تش_اشاره;
    } وإلا إذا(dest_ظرفیت==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_حد = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_ظرفیت=0x7fffffff;
    } وإلا {
        dest_حد=dest+dest_ظرفیت;
    }

    // perform the conversion
    UError_کوډ بفر_حالت = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR) {
        حرف بفر[1024];

        dest_حد=بفر+حجم(بفر);
        افعل {
            dest=بفر;
            بفر_حالت=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, تش_اشاره, ریښتیا, &بفر_حالت);
            length += static_cast<int32_t>(dest - بفر);
        } طالما(بفر_حالت==U_بفر_ډکېدنه_ERROR);
    }
    إذا (U_FAILURE(بفر_حالت)) {
        error_کوډ = بفر_حالت;
    }

    أرجع u_terminateChars(originalDest, dest_ظرفیت, length, &error_کوډ);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_معلومات,
                                int32_t معلومات_اوږدتیا,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    إذا (codepage_معلومات == تش_اشاره || معلومات_اوږدتیا == 0 || معلومات_اوږدتیا < -1) {
        أرجع;
    }
    إذا(معلومات_اوږدتیا == -1) {
        معلومات_اوږدتیا = static_cast<int32_t>(uprv_strlen(codepage_معلومات));
    }

    UError_کوډ حالت = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    إذا (codepage == تش_اشاره) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            سېټ_منزل_UTF8(StringPiece(codepage_معلومات, معلومات_اوږدتیا));
            أرجع;
        }
        converter = u_getDefaultConverter(&حالت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        إذا(cloneArrayIfNeeded(معلومات_اوږدتیا, معلومات_اوږدتیا, خطأ)) {
            u_chars_منزل_UChars(codepage_معلومات, get_Array_پیل(), معلومات_اوږدتیا);
            سېټ_اوږدتیا(معلومات_اوږدتیا);
        } وإلا {
            سېټ_منزل_Bogus();
        }
        أرجع;
    } وإلا {
        converter = ucnv_open(codepage, &حالت);
    }

    // if we failed, set the appropriate flags and return
    إذا(U_FAILURE(حالت)) {
        سېټ_منزل_Bogus();
        أرجع;
    }

    // perform the conversion
    doCodepageCreate(codepage_معلومات, معلومات_اوږدتیا, converter, حالت);
    إذا(U_FAILURE(حالت)) {
        سېټ_منزل_Bogus();
    }

    // close the converter
    إذا (codepage == تش_اشاره) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_معلومات,
                                int32_t معلومات_اوږدتیا,
                                UConverter *converter,
                                UError_کوډ &حالت)
{
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }

    // set up the conversion parameters
    ثابت حرف *my_سرچینه     = codepage_معلومات;
    ثابت حرف *my_سرچینه_پای_ټکی  = my_سرچینه + معلومات_اوږدتیا;
    char16_t *array, *my_موخه;

    // estimate the size needed:
    int32_t array_اندازه;
    إذا(معلومات_اوږدتیا <= US_STACKBUF_اندازه) {
        // try to use the stack buffer
        array_اندازه = US_STACKBUF_اندازه;
    } وإلا {
        // 1.25 char16_t's per source byte should cover most cases
        array_اندازه = معلومات_اوږدتیا + (معلومات_اوږدتیا >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = خطأ;
    لكل(;;) {
        إذا(!cloneArrayIfNeeded(array_اندازه, array_اندازه, doCopyArray)) {
            سېټ_منزل_Bogus();
            اكسر;
        }

        // perform the conversion
        array = get_Array_پیل();
        my_موخه = array + length();
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        ucnv_منزل_Unicode(converter, &my_موخه,  array + get_ظرفیت(),
            &my_سرچینه, my_سرچینه_پای_ټکی, تش_اشاره, ریښتیا, &بفر_حالت);

        // update the conversion parameters
        سېټ_اوږدتیا(static_cast<int32_t>(my_موخه - array));

        // allocate more space and copy data, if needed
        إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR) {
            // keep the previous conversion results
            doCopyArray = ریښتیا;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_اندازه = static_cast<int32_t>(length() + 2 * (my_سرچینه_پای_ټکی - my_سرچینه));
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                حالت = بفر_حالت;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_پای_ټکی

#د_که_پای
