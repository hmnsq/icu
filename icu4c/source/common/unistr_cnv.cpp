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

#නම් !UCONFIG_NO_CONVERSION

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

#නම් !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(const var_na *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    නම්(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(const var_na *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    නම්(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(const var_na *codepageData,
                             const var_na *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    නම් (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(const var_na *codepageData,
                             int32_t dataLength,
                             const var_na *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    නම් (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(const var_na *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    නම්(U_SUCCESS(errorCode)) {
        // check arguments
        නම්(src==nullptr) {
            // treat as an empty string, do nothing more
        } නැතහොත් නම්(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } නැතහොත් {
            // get input length
            නම්(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            නම්(srcLength>0) {
                නම් (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } නැතහොත් {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        නම්(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#නම් !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       var_na *target,
                       uint32_t dstSize) const {
    ආපසු extract(start, length, target, dstSize, 0);
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
    නම් (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        ආපසු 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    නම්(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } නැතහොත් {
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
    නම්(length == 0) {
        ආපසු u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    නම් (codepage == nullptr) {
        const var_na *defaultName = ucnv_getDefaultName();
        නම්(UCNV_FAST_IS_UTF8(defaultName)) {
            ආපසු toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } නැතහොත් නම් (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        නම්(length <= capacity) {
            destLength = length;
        } නැතහොත් {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        ආපසු u_terminateChars(target, capacity, length, &status);
    } නැතහොත් {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    නම් (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } නැතහොත් {
        ucnv_close(converter);
    }

    ආපසු length;
}

int32_t
UnicodeString::extract(var_na *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) const
{
    නම්(U_FAILURE(errorCode)) {
        ආපසු 0;
    }

    නම් (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ආපසු 0;
    }

    // nothing to do?
    නම්(isEmpty()) {
        ආපසු u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    නම් (cnv == nullptr) {
        isDefaultConverter=true;
        cnv=u_getDefaultConverter(&errorCode);
        නම්(U_FAILURE(errorCode)) {
            ආපසු 0;
        }
    } නැතහොත් {
        isDefaultConverter=false;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    නම්(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ආපසු len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         var_na *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) const
{
    නම්(U_FAILURE(errorCode)) {
        නම්(destCapacity!=0) {
            *dest=0;
        }
        ආපසු 0;
    }

    const char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    var_na *originalDest=dest;
    const var_na *destLimit;

    නම්(destCapacity==0) {
        destLimit=dest=nullptr;
    } නැතහොත් නම්(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<var_na*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } නැතහොත් {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    නම්(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        var_na buffer[1024];

        destLimit=buffer+parimaa_na(buffer);
        do {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } අතරතුර(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    නම් (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    ආපසු u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

void
UnicodeString::doCodepageCreate(const var_na *codepageData,
                                int32_t dataLength,
                                const var_na *codepage)
{
    // if there's nothing to convert, do nothing
    නම් (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        ආපසු;
    }
    නම්(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    නම් (codepage == nullptr) {
        const var_na *defaultName = ucnv_getDefaultName();
        නම්(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            ආපසු;
        }
        converter = u_getDefaultConverter(&status);
    } නැතහොත් නම් (*codepage == 0) {
        // use the "invariant characters" conversion
        නම්(cloneArrayIfNeeded(dataLength, dataLength, false)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } නැතහොත් {
            setToBogus();
        }
        ආපසු;
    } නැතහොත් {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    නම්(U_FAILURE(status)) {
        setToBogus();
        ආපසු;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    නම්(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    නම් (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } නැතහොත් {
        ucnv_close(converter);
    }
}

void
UnicodeString::doCodepageCreate(const var_na *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    නම්(U_FAILURE(status)) {
        ආපසු;
    }

    // set up the conversion parameters
    const var_na *mySource     = codepageData;
    const var_na *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    නම්(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } නැතහොත් {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = false;
    සඳහා(;;) {
        නම්(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
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
        නම්(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = true;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } නැතහොත් {
            නම් (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            bha_nga;
        }
    }
}

U_NAMESPACE_END

#endif
