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

#如果 !UCONFIG_NO_CONVERSION

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

#如果 !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(const char *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    如果(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(const char *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    如果(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(const char *codepageData,
                             const char *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    如果 (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(const char *codepageData,
                             int32_t dataLength,
                             const char *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    如果 (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(const char *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    如果(U_SUCCESS(errorCode)) {
        // check arguments
        如果(src==nullptr) {
            // treat as an empty string, do nothing more
        } 否则 如果(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } 否则 {
            // get input length
            如果(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            如果(srcLength>0) {
                如果 (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } 否则 {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        如果(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#如果 !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       char *target,
                       uint32_t dstSize) const {
    返回 extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       char *target,
                       uint32_t dstSize,
                       const char *codepage) const
{
    // if the arguments are illegal, then do nothing
    如果 (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        返回 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    如果(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } 否则 {
        // Pin the capacity so that a limit pointer does not wrap around.
        char* targetLimit = static_cast<char*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    如果(length == 0) {
        返回 u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    如果 (codepage == nullptr) {
        const char *defaultName = ucnv_getDefaultName();
        如果(UCNV_FAST_IS_UTF8(defaultName)) {
            返回 toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } 否则 如果 (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        如果(length <= capacity) {
            destLength = length;
        } 否则 {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        返回 u_terminateChars(target, capacity, length, &status);
    } 否则 {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    如果 (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } 否则 {
        ucnv_close(converter);
    }

    返回 length;
}

int32_t
UnicodeString::extract(char *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) const
{
    如果(U_FAILURE(errorCode)) {
        返回 0;
    }

    如果 (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回 0;
    }

    // nothing to do?
    如果(isEmpty()) {
        返回 u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    如果 (cnv == nullptr) {
        isDefaultConverter=true;
        cnv=u_getDefaultConverter(&errorCode);
        如果(U_FAILURE(errorCode)) {
            返回 0;
        }
    } 否则 {
        isDefaultConverter=false;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    如果(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    返回 len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         char *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) const
{
    如果(U_FAILURE(errorCode)) {
        如果(destCapacity!=0) {
            *dest=0;
        }
        返回 0;
    }

    const char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    char *originalDest=dest;
    const char *destLimit;

    如果(destCapacity==0) {
        destLimit=dest=nullptr;
    } 否则 如果(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<char*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } 否则 {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    如果(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        char buffer[1024];

        destLimit=buffer+sizeof(buffer);
        do {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } 当(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    如果 (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    返回 u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

void
UnicodeString::doCodepageCreate(const char *codepageData,
                                int32_t dataLength,
                                const char *codepage)
{
    // if there's nothing to convert, do nothing
    如果 (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        返回;
    }
    如果(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    如果 (codepage == nullptr) {
        const char *defaultName = ucnv_getDefaultName();
        如果(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            返回;
        }
        converter = u_getDefaultConverter(&status);
    } 否则 如果 (*codepage == 0) {
        // use the "invariant characters" conversion
        如果(cloneArrayIfNeeded(dataLength, dataLength, false)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } 否则 {
            setToBogus();
        }
        返回;
    } 否则 {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    如果(U_FAILURE(status)) {
        setToBogus();
        返回;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    如果(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    如果 (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } 否则 {
        ucnv_close(converter);
    }
}

void
UnicodeString::doCodepageCreate(const char *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    如果(U_FAILURE(status)) {
        返回;
    }

    // set up the conversion parameters
    const char *mySource     = codepageData;
    const char *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    如果(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } 否则 {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = false;
    为(;;) {
        如果(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            break;
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
        如果(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = true;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } 否则 {
            如果 (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            break;
        }
    }
}

U_NAMESPACE_END

#endif
