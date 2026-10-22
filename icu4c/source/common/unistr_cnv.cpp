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

UnicodeString::UnicodeString(ثابت حرف *codepage_داده) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    إذا(codepage_داده != 0) {
        doCodepageCreate(codepage_داده, (int32_t)uprv_strlen(codepage_داده), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_داده,
                             int32_t داده_درازی) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    إذا(codepage_داده != 0) {
        doCodepageCreate(codepage_داده, داده_درازی, 0);
    }
}

// else see unistr.cpp
#پایان_اگر

UnicodeString::UnicodeString(ثابت حرف *codepage_داده,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    إذا (codepage_داده != اشاره_تهی) {
        doCodepageCreate(codepage_داده, static_cast<int32_t>(uprv_strlen(codepage_داده)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_داده,
                             int32_t داده_درازی,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    إذا (codepage_داده != اشاره_تهی) {
        doCodepageCreate(codepage_داده, داده_درازی, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t src_درازی,
                             UConverter *cnv,
                             UError_کد &error_کد) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    إذا(U_SUCCESS(error_کد)) {
        // check arguments
        إذا(src==اشاره_تهی) {
            // treat as an empty string, do nothing more
        } وإلا إذا(src_درازی<-1) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
        } وإلا {
            // get input length
            إذا(src_درازی==-1) {
                src_درازی = static_cast<int32_t>(uprv_strlen(src));
            }
            إذا(src_درازی>0) {
                إذا (cnv != اشاره_تهی) {
                    // use the provided converter
                    ucnv_reset_مقصد_Unicode(cnv);
                    doCodepageCreate(src, src_درازی, cnv, error_کد);
                } وإلا {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_کد);
                    doCodepageCreate(src, src_درازی, cnv, error_کد);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        إذا(U_FAILURE(error_کد)) {
            مجموعه_مقصد_Bogus();
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
                       حرف *هدف,
                       uint32_t dst_اندازه) ثابت {
    أرجع extract(شروع, length, هدف, dst_اندازه, 0);
}

// else see unistr.cpp
#پایان_اگر

int32_t
UnicodeString::extract(int32_t شروع,
                       int32_t length,
                       حرف *هدف,
                       uint32_t dst_اندازه,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    إذا (/*dstSize < 0 || */(dst_اندازه > 0 && هدف == اشاره_تهی)) {
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
    إذا(dst_اندازه < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_اندازه);
    } وإلا {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* هدف_حد = static_cast<حرف*>(U_MAX_PTR(هدف));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(هدف_حد - هدف);
    }

    // create the converter
    UConverter *converter;
    UError_کد وضعیت = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    إذا(length == 0) {
        أرجع u_terminateChars(هدف, capacity, 0, &وضعیت);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    إذا (codepage == اشاره_تهی) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            أرجع مقصد_UTF8(شروع, length, هدف, capacity);
        }
        converter = u_getDefaultConverter(&وضعیت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_درازی;
        إذا(length <= capacity) {
            dest_درازی = length;
        } وإلا {
            dest_درازی = capacity;
        }
        u_UChars_مقصد_Chars(get_Array_شروع() + شروع, هدف, dest_درازی);
        أرجع u_terminateChars(هدف, capacity, length, &وضعیت);
    } وإلا {
        converter = ucnv_open(codepage, &وضعیت);
    }

    length = do_استخراج(شروع, length, هدف, capacity, converter, وضعیت);

    // close the converter
    إذا (codepage == اشاره_تهی) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }

    أرجع length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t dest_ظرفیت,
                       UConverter *cnv,
                       UError_کد &error_کد) ثابت
{
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }

    إذا (isBogus() || dest_ظرفیت < 0 || (dest_ظرفیت > 0 && dest == اشاره_تهی)) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    // nothing to do?
    إذا(is_خالی()) {
        أرجع u_terminateChars(dest, dest_ظرفیت, 0, &error_کد);
    }

    // get the converter
    UBool isDefaultConverter;
    إذا (cnv == اشاره_تهی) {
        isDefaultConverter=درست;
        cnv=u_getDefaultConverter(&error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
    } وإلا {
        isDefaultConverter=خطأ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_استخراج(0, length(), dest, dest_ظرفیت, cnv, error_کد);

    // release the converter
    إذا(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    أرجع len;
}

int32_t
UnicodeString::do_استخراج(int32_t شروع, int32_t length,
                         حرف *dest, int32_t dest_ظرفیت,
                         UConverter *cnv,
                         UError_کد &error_کد) ثابت
{
    إذا(U_FAILURE(error_کد)) {
        إذا(dest_ظرفیت!=0) {
            *dest=0;
        }
        أرجع 0;
    }

    ثابت char16_t *src=get_Array_شروع()+شروع, *src_حد=src+length;
    حرف *originalDest=dest;
    ثابت حرف *dest_حد;

    إذا(dest_ظرفیت==0) {
        dest_حد=dest=اشاره_تهی;
    } وإلا إذا(dest_ظرفیت==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_حد = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_ظرفیت=0x7fffffff;
    } وإلا {
        dest_حد=dest+dest_ظرفیت;
    }

    // perform the conversion
    UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR) {
        حرف میانگیر[1024];

        dest_حد=میانگیر+حجم(میانگیر);
        افعل {
            dest=میانگیر;
            میانگیر_وضعیت=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_حد, &src, src_حد, اشاره_تهی, درست, &میانگیر_وضعیت);
            length += static_cast<int32_t>(dest - میانگیر);
        } طالما(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR);
    }
    إذا (U_FAILURE(میانگیر_وضعیت)) {
        error_کد = میانگیر_وضعیت;
    }

    أرجع u_terminateChars(originalDest, dest_ظرفیت, length, &error_کد);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_داده,
                                int32_t داده_درازی,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    إذا (codepage_داده == اشاره_تهی || داده_درازی == 0 || داده_درازی < -1) {
        أرجع;
    }
    إذا(داده_درازی == -1) {
        داده_درازی = static_cast<int32_t>(uprv_strlen(codepage_داده));
    }

    UError_کد وضعیت = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    إذا (codepage == اشاره_تهی) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        إذا(UCNV_FAST_IS_UTF8(defaultName)) {
            مجموعه_مقصد_UTF8(StringPiece(codepage_داده, داده_درازی));
            أرجع;
        }
        converter = u_getDefaultConverter(&وضعیت);
    } وإلا إذا (*codepage == 0) {
        // use the "invariant characters" conversion
        إذا(cloneArrayIfNeeded(داده_درازی, داده_درازی, خطأ)) {
            u_chars_مقصد_UChars(codepage_داده, get_Array_شروع(), داده_درازی);
            مجموعه_درازی(داده_درازی);
        } وإلا {
            مجموعه_مقصد_Bogus();
        }
        أرجع;
    } وإلا {
        converter = ucnv_open(codepage, &وضعیت);
    }

    // if we failed, set the appropriate flags and return
    إذا(U_FAILURE(وضعیت)) {
        مجموعه_مقصد_Bogus();
        أرجع;
    }

    // perform the conversion
    doCodepageCreate(codepage_داده, داده_درازی, converter, وضعیت);
    إذا(U_FAILURE(وضعیت)) {
        مجموعه_مقصد_Bogus();
    }

    // close the converter
    إذا (codepage == اشاره_تهی) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepage_داده,
                                int32_t داده_درازی,
                                UConverter *converter,
                                UError_کد &وضعیت)
{
    إذا(U_FAILURE(وضعیت)) {
        أرجع;
    }

    // set up the conversion parameters
    ثابت حرف *my_منبع     = codepage_داده;
    ثابت حرف *my_منبع_انتها  = my_منبع + داده_درازی;
    char16_t *array, *my_هدف;

    // estimate the size needed:
    int32_t array_اندازه;
    إذا(داده_درازی <= US_STACKBUF_اندازه) {
        // try to use the stack buffer
        array_اندازه = US_STACKBUF_اندازه;
    } وإلا {
        // 1.25 char16_t's per source byte should cover most cases
        array_اندازه = داده_درازی + (داده_درازی >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = خطأ;
    لكل(;;) {
        إذا(!cloneArrayIfNeeded(array_اندازه, array_اندازه, doCopyArray)) {
            مجموعه_مقصد_Bogus();
            اكسر;
        }

        // perform the conversion
        array = get_Array_شروع();
        my_هدف = array + length();
        UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
        ucnv_مقصد_Unicode(converter, &my_هدف,  array + get_ظرفیت(),
            &my_منبع, my_منبع_انتها, اشاره_تهی, درست, &میانگیر_وضعیت);

        // update the conversion parameters
        مجموعه_درازی(static_cast<int32_t>(my_هدف - array));

        // allocate more space and copy data, if needed
        إذا(میانگیر_وضعیت == U_میانگیر_سرریز_ERROR) {
            // keep the previous conversion results
            doCopyArray = درست;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_اندازه = static_cast<int32_t>(length() + 2 * (my_منبع_انتها - my_منبع));
        } وإلا {
            إذا (U_FAILURE(میانگیر_وضعیت)) {
                وضعیت = میانگیر_وضعیت;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_انتها

#پایان_اگر
