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

#include "unicode/utypes.h"

#အကယ်၍ !UCONFIG_NO_CONVERSION

#include "unicode/putil.h"
#include "cstring.h"
#include "cmemory.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/ucnv.h"
#include "ucnv_imp.h"
#include "putilimp.h"
#include "ustr_cnv.h"
#include "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#အကယ်၍ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(const var_na *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    အကယ်၍(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(const var_na *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    အကယ်၍(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(const var_na *codepageData,
                             const var_na *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    အကယ်၍ (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(const var_na *codepageData,
                             int32_t dataLength,
                             const var_na *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    အကယ်၍ (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(const var_na *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    အကယ်၍(U_SUCCESS(errorCode)) {
        // check arguments
        အကယ်၍(src==nullptr) {
            // treat as an empty string, do nothing more
        } မဟုတ်ပါက အကယ်၍(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } မဟုတ်ပါက {
            // get input length
            အကယ်၍(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            အကယ်၍(srcLength>0) {
                အကယ်၍ (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } မဟုတ်ပါက {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        အကယ်၍(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#အကယ်၍ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       var_na *target,
                       uint32_t dstSize) const {
    ပြန်ပေး extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       var_na *target,
                       uint32_t dstSize,
                       const var_na *codepage) const
{
    // if the arguments are illegal, then do nothing
    အကယ်၍ (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        ပြန်ပေး 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    အကယ်၍(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } မဟုတ်ပါက {
        // Pin the capacity so that a limit pointer does not wrap around.
        var_na* targetLimit = static_cast<var_na*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    အကယ်၍(length == 0) {
        ပြန်ပေး u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    အကယ်၍ (codepage == nullptr) {
        const var_na *defaultName = ucnv_getDefaultName();
        အကယ်၍(UCNV_FAST_IS_UTF8(defaultName)) {
            ပြန်ပေး toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } မဟုတ်ပါက အကယ်၍ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        အကယ်၍(length <= capacity) {
            destLength = length;
        } မဟုတ်ပါက {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        ပြန်ပေး u_terminateChars(target, capacity, length, &status);
    } မဟုတ်ပါက {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    အကယ်၍ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } မဟုတ်ပါက {
        ucnv_close(converter);
    }

    ပြန်ပေး length;
}

int32_t
UnicodeString::extract(var_na *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) const
{
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး 0;
    }

    အကယ်၍ (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး 0;
    }

    // nothing to do?
    အကယ်၍(isEmpty()) {
        ပြန်ပေး u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    အကယ်၍ (cnv == nullptr) {
        isDefaultConverter=true;
        cnv=u_getDefaultConverter(&errorCode);
        အကယ်၍(U_FAILURE(errorCode)) {
            ပြန်ပေး 0;
        }
    } မဟုတ်ပါက {
        isDefaultConverter=false;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    အကယ်၍(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ပြန်ပေး len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         var_na *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) const
{
    အကယ်၍(U_FAILURE(errorCode)) {
        အကယ်၍(destCapacity!=0) {
            *dest=0;
        }
        ပြန်ပေး 0;
    }

    const char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    var_na *originalDest=dest;
    const var_na *destLimit;

    အကယ်၍(destCapacity==0) {
        destLimit=dest=nullptr;
    } မဟုတ်ပါက အကယ်၍(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<var_na*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } မဟုတ်ပါက {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    အကယ်၍(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        var_na buffer[1024];

        destLimit=buffer+parimaa_na(buffer);
        do {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } နေစဉ်(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    အကယ်၍ (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    ပြန်ပေး u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

void
UnicodeString::doCodepageCreate(const var_na *codepageData,
                                int32_t dataLength,
                                const var_na *codepage)
{
    // if there's nothing to convert, do nothing
    အကယ်၍ (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        ပြန်ပေး;
    }
    အကယ်၍(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    အကယ်၍ (codepage == nullptr) {
        const var_na *defaultName = ucnv_getDefaultName();
        အကယ်၍(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            ပြန်ပေး;
        }
        converter = u_getDefaultConverter(&status);
    } မဟုတ်ပါက အကယ်၍ (*codepage == 0) {
        // use the "invariant characters" conversion
        အကယ်၍(cloneArrayIfNeeded(dataLength, dataLength, false)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } မဟုတ်ပါက {
            setToBogus();
        }
        ပြန်ပေး;
    } မဟုတ်ပါက {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    အကယ်၍(U_FAILURE(status)) {
        setToBogus();
        ပြန်ပေး;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    အကယ်၍(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    အကယ်၍ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } မဟုတ်ပါက {
        ucnv_close(converter);
    }
}

void
UnicodeString::doCodepageCreate(const var_na *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    အကယ်၍(U_FAILURE(status)) {
        ပြန်ပေး;
    }

    // set up the conversion parameters
    const var_na *mySource     = codepageData;
    const var_na *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    အကယ်၍(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } မဟုတ်ပါက {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = false;
    အတွက်(;;) {
        အကယ်၍(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            bha_nga;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, true, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        အကယ်၍(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = true;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } မဟုတ်ပါက {
            အကယ်၍ (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            bha_nga;
        }
    }
}

U_NAMESPACE_END

#endif
