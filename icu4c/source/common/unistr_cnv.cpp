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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "ustr_cnv.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#যদি !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    যদি(codepage_তথ্য != 0) {
        doCodepageCreate(codepage_তথ্য, (int32_t)uprv_strlen(codepage_তথ্য), 0);
    }
}

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য,
                             int32_t তথ্য_দৈর্ঘ্য) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    যদি(codepage_তথ্য != 0) {
        doCodepageCreate(codepage_তথ্য, তথ্য_দৈর্ঘ্য, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য,
                             ধ্রুবক অক্ষর *codepage) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    যদি (codepage_তথ্য != nullptr) {
        doCodepageCreate(codepage_তথ্য, static_cast<int32_t>(uprv_strlen(codepage_তথ্য)), codepage);
    }
}

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য,
                             int32_t তথ্য_দৈর্ঘ্য,
                             ধ্রুবক অক্ষর *codepage) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    যদি (codepage_তথ্য != nullptr) {
        doCodepageCreate(codepage_তথ্য, তথ্য_দৈর্ঘ্য, codepage);
    }
}

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    যদি(U_SUCCESS(errorCode)) {
        // check arguments
        যদি(src==nullptr) {
            // treat as an empty string, do nothing more
        } অন্যথা যদি(src_দৈর্ঘ্য<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } অন্যথা {
            // get input length
            যদি(src_দৈর্ঘ্য==-1) {
                src_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(src));
            }
            যদি(src_দৈর্ঘ্য>0) {
                যদি (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, src_দৈর্ঘ্য, cnv, errorCode);
                } অন্যথা {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, src_দৈর্ঘ্য, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        যদি(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#যদি !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       অক্ষর *target,
                       uint32_t dst_আকার) ধ্রুবক {
    ফেরত extract(start, length, target, dst_আকার, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       অক্ষর *target,
                       uint32_t dst_আকার,
                       ধ্রুবক অক্ষর *codepage) ধ্রুবক
{
    // if the arguments are illegal, then do nothing
    যদি (/*dstSize < 0 || */(dst_আকার > 0 && target == nullptr)) {
        ফেরত 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    যদি(dst_আকার < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_আকার);
    } অন্যথা {
        // Pin the capacity so that a limit pointer does not wrap around.
        অক্ষর* targetLimit = static_cast<অক্ষর*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    যদি(length == 0) {
        ফেরত u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    যদি (codepage == nullptr) {
        ধ্রুবক অক্ষর *defaultName = ucnv_getDefaultName();
        যদি(UCNV_FAST_IS_UTF8(defaultName)) {
            ফেরত toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } অন্যথা যদি (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_দৈর্ঘ্য;
        যদি(length <= capacity) {
            dest_দৈর্ঘ্য = length;
        } অন্যথা {
            dest_দৈর্ঘ্য = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, dest_দৈর্ঘ্য);
        ফেরত u_terminateChars(target, capacity, length, &status);
    } অন্যথা {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    যদি (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } অন্যথা {
        ucnv_close(converter);
    }

    ফেরত length;
}

int32_t
UnicodeString::extract(অক্ষর *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ধ্রুবক
{
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }

    যদি (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    // nothing to do?
    যদি(isEmpty()) {
        ফেরত u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    যদি (cnv == nullptr) {
        isDefaultConverter=সত্য;
        cnv=u_getDefaultConverter(&errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
    } অন্যথা {
        isDefaultConverter=মিথ্যা;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    যদি(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ফেরত len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         অক্ষর *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ধ্রুবক
{
    যদি(U_FAILURE(errorCode)) {
        যদি(destCapacity!=0) {
            *dest=0;
        }
        ফেরত 0;
    }

    ধ্রুবক char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    অক্ষর *originalDest=dest;
    ধ্রুবক অক্ষর *destLimit;

    যদি(destCapacity==0) {
        destLimit=dest=nullptr;
    } অন্যথা যদি(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<অক্ষর*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } অন্যথা {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode বাফার_Status = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR) {
        অক্ষর বাফার[1024];

        destLimit=বাফার+আকার(বাফার);
        করো {
            dest=বাফার;
            বাফার_Status=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &বাফার_Status);
            length += static_cast<int32_t>(dest - বাফার);
        } যখন(বাফার_Status==U_বাফার_OVERFLOW_ERROR);
    }
    যদি (U_FAILURE(বাফার_Status)) {
        errorCode = বাফার_Status;
    }

    ফেরত u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

শূন্য
UnicodeString::doCodepageCreate(ধ্রুবক অক্ষর *codepage_তথ্য,
                                int32_t তথ্য_দৈর্ঘ্য,
                                ধ্রুবক অক্ষর *codepage)
{
    // if there's nothing to convert, do nothing
    যদি (codepage_তথ্য == nullptr || তথ্য_দৈর্ঘ্য == 0 || তথ্য_দৈর্ঘ্য < -1) {
        ফেরত;
    }
    যদি(তথ্য_দৈর্ঘ্য == -1) {
        তথ্য_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(codepage_তথ্য));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    যদি (codepage == nullptr) {
        ধ্রুবক অক্ষর *defaultName = ucnv_getDefaultName();
        যদি(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepage_তথ্য, তথ্য_দৈর্ঘ্য));
            ফেরত;
        }
        converter = u_getDefaultConverter(&status);
    } অন্যথা যদি (*codepage == 0) {
        // use the "invariant characters" conversion
        যদি(cloneArrayIfNeeded(তথ্য_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য, মিথ্যা)) {
            u_charsToUChars(codepage_তথ্য, getArrayStart(), তথ্য_দৈর্ঘ্য);
            set_দৈর্ঘ্য(তথ্য_দৈর্ঘ্য);
        } অন্যথা {
            setToBogus();
        }
        ফেরত;
    } অন্যথা {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    যদি(U_FAILURE(status)) {
        setToBogus();
        ফেরত;
    }

    // perform the conversion
    doCodepageCreate(codepage_তথ্য, তথ্য_দৈর্ঘ্য, converter, status);
    যদি(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    যদি (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } অন্যথা {
        ucnv_close(converter);
    }
}

শূন্য
UnicodeString::doCodepageCreate(ধ্রুবক অক্ষর *codepage_তথ্য,
                                int32_t তথ্য_দৈর্ঘ্য,
                                UConverter *converter,
                                UErrorCode &status)
{
    যদি(U_FAILURE(status)) {
        ফেরত;
    }

    // set up the conversion parameters
    ধ্রুবক অক্ষর *mySource     = codepage_তথ্য;
    ধ্রুবক অক্ষর *mySourceEnd  = mySource + তথ্য_দৈর্ঘ্য;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t array_আকার;
    যদি(তথ্য_দৈর্ঘ্য <= US_STACKBUF_আকার) {
        // try to use the stack buffer
        array_আকার = US_STACKBUF_আকার;
    } অন্যথা {
        // 1.25 char16_t's per source byte should cover most cases
        array_আকার = তথ্য_দৈর্ঘ্য + (তথ্য_দৈর্ঘ্য >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = মিথ্যা;
    জন্য(;;) {
        যদি(!cloneArrayIfNeeded(array_আকার, array_আকার, doCopyArray)) {
            setToBogus();
            বিরতি;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, সত্য, &বাফার_Status);

        // update the conversion parameters
        set_দৈর্ঘ্য(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = সত্য;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_আকার = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } অন্যথা {
            যদি (U_FAILURE(বাফার_Status)) {
                status = বাফার_Status;
            }
            বিরতি;
        }
    }
}

U_NAMESPACE_END

#endif
