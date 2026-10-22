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

#якщо !UCONFIG_NO_CONVERSION

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

#якщо !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(const char *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    якщо(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(const char *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    якщо(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(const char *codepageData,
                             const char *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    якщо (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(const char *codepageData,
                             int32_t dataLength,
                             const char *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    якщо (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(const char *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    якщо(U_SUCCESS(errorCode)) {
        // check arguments
        якщо(src==nullptr) {
            // treat as an empty string, do nothing more
        } інакше якщо(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } інакше {
            // get input length
            якщо(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            якщо(srcLength>0) {
                якщо (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } інакше {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        якщо(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#якщо !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       char *target,
                       uint32_t dstSize) const {
    повернути extract(start, length, target, dstSize, 0);
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
    якщо (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        повернути 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    якщо(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } інакше {
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
    якщо(length == 0) {
        повернути u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    якщо (codepage == nullptr) {
        const char *defaultName = ucnv_getDefaultName();
        якщо(UCNV_FAST_IS_UTF8(defaultName)) {
            повернути toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } інакше якщо (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        якщо(length <= capacity) {
            destLength = length;
        } інакше {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        повернути u_terminateChars(target, capacity, length, &status);
    } інакше {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    якщо (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } інакше {
        ucnv_close(converter);
    }

    повернути length;
}

int32_t
UnicodeString::extract(char *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) const
{
    якщо(U_FAILURE(errorCode)) {
        повернути 0;
    }

    якщо (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        повернути 0;
    }

    // nothing to do?
    якщо(isEmpty()) {
        повернути u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    якщо (cnv == nullptr) {
        isDefaultConverter=true;
        cnv=u_getDefaultConverter(&errorCode);
        якщо(U_FAILURE(errorCode)) {
            повернути 0;
        }
    } інакше {
        isDefaultConverter=false;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    якщо(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    повернути len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         char *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) const
{
    якщо(U_FAILURE(errorCode)) {
        якщо(destCapacity!=0) {
            *dest=0;
        }
        повернути 0;
    }

    const char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    char *originalDest=dest;
    const char *destLimit;

    якщо(destCapacity==0) {
        destLimit=dest=nullptr;
    } інакше якщо(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<char*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } інакше {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    якщо(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        char buffer[1024];

        destLimit=buffer+sizeof(buffer);
        do {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } поки(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    якщо (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    повернути u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

void
UnicodeString::doCodepageCreate(const char *codepageData,
                                int32_t dataLength,
                                const char *codepage)
{
    // if there's nothing to convert, do nothing
    якщо (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        повернути;
    }
    якщо(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    якщо (codepage == nullptr) {
        const char *defaultName = ucnv_getDefaultName();
        якщо(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            повернути;
        }
        converter = u_getDefaultConverter(&status);
    } інакше якщо (*codepage == 0) {
        // use the "invariant characters" conversion
        якщо(cloneArrayIfNeeded(dataLength, dataLength, false)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } інакше {
            setToBogus();
        }
        повернути;
    } інакше {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    якщо(U_FAILURE(status)) {
        setToBogus();
        повернути;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    якщо(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    якщо (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } інакше {
        ucnv_close(converter);
    }
}

void
UnicodeString::doCodepageCreate(const char *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    якщо(U_FAILURE(status)) {
        повернути;
    }

    // set up the conversion parameters
    const char *mySource     = codepageData;
    const char *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    якщо(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } інакше {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = false;
    для(;;) {
        якщо(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
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
        якщо(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = true;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } інакше {
            якщо (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            break;
        }
    }
}

U_NAMESPACE_END

#endif
