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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/putil.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/ucnv.h"
#شامل "ucnv_imp.h"
#شامل "putilimp.h"
#شامل "ustr_cnv.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#اگر !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    اگر(codepage_ڈیٹا != 0) {
        doCodepageCreate(codepage_ڈیٹا, (int32_t)uprv_strlen(codepage_ڈیٹا), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا,
                             int32_t ڈیٹا_لمبائی) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    اگر(codepage_ڈیٹا != 0) {
        doCodepageCreate(codepage_ڈیٹا, ڈیٹا_لمبائی, 0);
    }
}

// else see unistr.cpp
#اگر_ختم

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    اگر (codepage_ڈیٹا != خالی_اشاریہ) {
        doCodepageCreate(codepage_ڈیٹا, static_cast<int32_t>(uprv_strlen(codepage_ڈیٹا)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا,
                             int32_t ڈیٹا_لمبائی,
                             ثابت حرف *codepage) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    اگر (codepage_ڈیٹا != خالی_اشاریہ) {
        doCodepageCreate(codepage_ڈیٹا, ڈیٹا_لمبائی, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t src_لمبائی,
                             UConverter *cnv,
                             UError_رمز &error_رمز) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    اگر(U_SUCCESS(error_رمز)) {
        // check arguments
        اگر(src==خالی_اشاریہ) {
            // treat as an empty string, do nothing more
        } ورنہ اگر(src_لمبائی<-1) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
        } ورنہ {
            // get input length
            اگر(src_لمبائی==-1) {
                src_لمبائی = static_cast<int32_t>(uprv_strlen(src));
            }
            اگر(src_لمبائی>0) {
                اگر (cnv != خالی_اشاریہ) {
                    // use the provided converter
                    ucnv_reset_مقصد_Unicode(cnv);
                    doCodepageCreate(src, src_لمبائی, cnv, error_رمز);
                } ورنہ {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_رمز);
                    doCodepageCreate(src, src_لمبائی, cnv, error_رمز);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        اگر(U_FAILURE(error_رمز)) {
            سیٹ_مقصد_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#اگر !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t آغاز,
                       int32_t length,
                       حرف *ہدف,
                       uint32_t dst_سائز) ثابت {
    واپس extract(آغاز, length, ہدف, dst_سائز, 0);
}

// else see unistr.cpp
#اگر_ختم

int32_t
UnicodeString::extract(int32_t آغاز,
                       int32_t length,
                       حرف *ہدف,
                       uint32_t dst_سائز,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    اگر (/*dstSize < 0 || */(dst_سائز > 0 && ہدف == خالی_اشاریہ)) {
        واپس 0;
    }

    // pin the indices to legal values
    pinIndices(آغاز, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    اگر(dst_سائز < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_سائز);
    } ورنہ {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* ہدف_حد_بندی = static_cast<حرف*>(U_MAX_PTR(ہدف));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(ہدف_حد_بندی - ہدف);
    }

    // create the converter
    UConverter *converter;
    UError_رمز حالت_2 = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    اگر(length == 0) {
        واپس u_terminateChars(ہدف, capacity, 0, &حالت_2);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    اگر (codepage == خالی_اشاریہ) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        اگر(UCNV_FAST_IS_UTF8(defaultName)) {
            واپس مقصد_UTF8(آغاز, length, ہدف, capacity);
        }
        converter = u_getDefaultConverter(&حالت_2);
    } ورنہ اگر (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_لمبائی;
        اگر(length <= capacity) {
            dest_لمبائی = length;
        } ورنہ {
            dest_لمبائی = capacity;
        }
        u_UChars_مقصد_Chars(get_Array_آغاز() + آغاز, ہدف, dest_لمبائی);
        واپس u_terminateChars(ہدف, capacity, length, &حالت_2);
    } ورنہ {
        converter = ucnv_open(codepage, &حالت_2);
    }

    length = do_نکالو(آغاز, length, ہدف, capacity, converter, حالت_2);

    // close the converter
    اگر (codepage == خالی_اشاریہ) {
        u_releaseDefaultConverter(converter);
    } ورنہ {
        ucnv_close(converter);
    }

    واپس length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t dest_استعداد,
                       UConverter *cnv,
                       UError_رمز &error_رمز) ثابت
{
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }

    اگر (isBogus() || dest_استعداد < 0 || (dest_استعداد > 0 && dest == خالی_اشاریہ)) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    // nothing to do?
    اگر(is_خالی_خانے()) {
        واپس u_terminateChars(dest, dest_استعداد, 0, &error_رمز);
    }

    // get the converter
    UBool isDefaultConverter;
    اگر (cnv == خالی_اشاریہ) {
        isDefaultConverter=سچ;
        cnv=u_getDefaultConverter(&error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
    } ورنہ {
        isDefaultConverter=جھوٹ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_نکالو(0, length(), dest, dest_استعداد, cnv, error_رمز);

    // release the converter
    اگر(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    واپس len;
}

int32_t
UnicodeString::do_نکالو(int32_t آغاز, int32_t length,
                         حرف *dest, int32_t dest_استعداد,
                         UConverter *cnv,
                         UError_رمز &error_رمز) ثابت
{
    اگر(U_FAILURE(error_رمز)) {
        اگر(dest_استعداد!=0) {
            *dest=0;
        }
        واپس 0;
    }

    ثابت char16_t *src=get_Array_آغاز()+آغاز, *src_حد_بندی=src+length;
    حرف *originalDest=dest;
    ثابت حرف *dest_حد_بندی;

    اگر(dest_استعداد==0) {
        dest_حد_بندی=dest=خالی_اشاریہ;
    } ورنہ اگر(dest_استعداد==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_حد_بندی = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_استعداد=0x7fffffff;
    } ورنہ {
        dest_حد_بندی=dest+dest_استعداد;
    }

    // perform the conversion
    UError_رمز بفر_حالت = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR) {
        حرف بفر[1024];

        dest_حد_بندی=بفر+حجم(بفر);
        کرو {
            dest=بفر;
            بفر_حالت=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_حد_بندی, &src, src_حد_بندی, خالی_اشاریہ, سچ, &بفر_حالت);
            length += static_cast<int32_t>(dest - بفر);
        } جب(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR);
    }
    اگر (U_FAILURE(بفر_حالت)) {
        error_رمز = بفر_حالت;
    }

    واپس u_terminateChars(originalDest, dest_استعداد, length, &error_رمز);
}

خالی
UnicodeString::doCodepageCreate(ثابت حرف *codepage_ڈیٹا,
                                int32_t ڈیٹا_لمبائی,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    اگر (codepage_ڈیٹا == خالی_اشاریہ || ڈیٹا_لمبائی == 0 || ڈیٹا_لمبائی < -1) {
        واپس;
    }
    اگر(ڈیٹا_لمبائی == -1) {
        ڈیٹا_لمبائی = static_cast<int32_t>(uprv_strlen(codepage_ڈیٹا));
    }

    UError_رمز حالت_2 = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    اگر (codepage == خالی_اشاریہ) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        اگر(UCNV_FAST_IS_UTF8(defaultName)) {
            سیٹ_مقصد_UTF8(StringPiece(codepage_ڈیٹا, ڈیٹا_لمبائی));
            واپس;
        }
        converter = u_getDefaultConverter(&حالت_2);
    } ورنہ اگر (*codepage == 0) {
        // use the "invariant characters" conversion
        اگر(cloneArrayIfNeeded(ڈیٹا_لمبائی, ڈیٹا_لمبائی, جھوٹ)) {
            u_chars_مقصد_UChars(codepage_ڈیٹا, get_Array_آغاز(), ڈیٹا_لمبائی);
            سیٹ_لمبائی(ڈیٹا_لمبائی);
        } ورنہ {
            سیٹ_مقصد_Bogus();
        }
        واپس;
    } ورنہ {
        converter = ucnv_open(codepage, &حالت_2);
    }

    // if we failed, set the appropriate flags and return
    اگر(U_FAILURE(حالت_2)) {
        سیٹ_مقصد_Bogus();
        واپس;
    }

    // perform the conversion
    doCodepageCreate(codepage_ڈیٹا, ڈیٹا_لمبائی, converter, حالت_2);
    اگر(U_FAILURE(حالت_2)) {
        سیٹ_مقصد_Bogus();
    }

    // close the converter
    اگر (codepage == خالی_اشاریہ) {
        u_releaseDefaultConverter(converter);
    } ورنہ {
        ucnv_close(converter);
    }
}

خالی
UnicodeString::doCodepageCreate(ثابت حرف *codepage_ڈیٹا,
                                int32_t ڈیٹا_لمبائی,
                                UConverter *converter,
                                UError_رمز &حالت_2)
{
    اگر(U_FAILURE(حالت_2)) {
        واپس;
    }

    // set up the conversion parameters
    ثابت حرف *my_ماخذ     = codepage_ڈیٹا;
    ثابت حرف *my_ماخذ_انتہا  = my_ماخذ + ڈیٹا_لمبائی;
    char16_t *array, *my_ہدف;

    // estimate the size needed:
    int32_t array_سائز;
    اگر(ڈیٹا_لمبائی <= US_STACKBUF_سائز) {
        // try to use the stack buffer
        array_سائز = US_STACKBUF_سائز;
    } ورنہ {
        // 1.25 char16_t's per source byte should cover most cases
        array_سائز = ڈیٹا_لمبائی + (ڈیٹا_لمبائی >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = جھوٹ;
    برائے(;;) {
        اگر(!cloneArrayIfNeeded(array_سائز, array_سائز, doCopyArray)) {
            سیٹ_مقصد_Bogus();
            توڑو;
        }

        // perform the conversion
        array = get_Array_آغاز();
        my_ہدف = array + length();
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        ucnv_مقصد_Unicode(converter, &my_ہدف,  array + get_استعداد(),
            &my_ماخذ, my_ماخذ_انتہا, خالی_اشاریہ, سچ, &بفر_حالت);

        // update the conversion parameters
        سیٹ_لمبائی(static_cast<int32_t>(my_ہدف - array));

        // allocate more space and copy data, if needed
        اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR) {
            // keep the previous conversion results
            doCopyArray = سچ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_سائز = static_cast<int32_t>(length() + 2 * (my_ماخذ_انتہا - my_ماخذ));
        } ورنہ {
            اگر (U_FAILURE(بفر_حالت)) {
                حالت_2 = بفر_حالت;
            }
            توڑو;
        }
    }
}

U_NAMESPACE_انتہا

#اگر_ختم
