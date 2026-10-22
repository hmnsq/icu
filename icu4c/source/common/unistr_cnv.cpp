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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#ಯದಿ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ಧ್ರುವ ವರ್ಣ *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ಯದಿ(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ಧ್ರುವ ವರ್ಣ *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ಯದಿ(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ಧ್ರುವ ವರ್ಣ *codepageData,
                             ಧ್ರುವ ವರ್ಣ *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ಯದಿ (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ಧ್ರುವ ವರ್ಣ *codepageData,
                             int32_t dataLength,
                             ಧ್ರುವ ವರ್ಣ *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ಯದಿ (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ಧ್ರುವ ವರ್ಣ *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ಯದಿ(U_SUCCESS(errorCode)) {
        // check arguments
        ಯದಿ(src==nullptr) {
            // treat as an empty string, do nothing more
        } ಅನ್ಯಥಾ ಯದಿ(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } ಅನ್ಯಥಾ {
            // get input length
            ಯದಿ(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            ಯದಿ(srcLength>0) {
                ಯದಿ (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } ಅನ್ಯಥಾ {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        ಯದಿ(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#ಯದಿ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ವರ್ಣ *target,
                       uint32_t dstSize) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ವರ್ಣ *target,
                       uint32_t dstSize,
                       ಧ್ರುವ ವರ್ಣ *codepage) ಧ್ರುವ
{
    // if the arguments are illegal, then do nothing
    ಯದಿ (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    ಯದಿ(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } ಅನ್ಯಥಾ {
        // Pin the capacity so that a limit pointer does not wrap around.
        ವರ್ಣ* targetLimit = static_cast<ವರ್ಣ*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    ಯದಿ(length == 0) {
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    ಯದಿ (codepage == nullptr) {
        ಧ್ರುವ ವರ್ಣ *defaultName = ucnv_getDefaultName();
        ಯದಿ(UCNV_FAST_IS_UTF8(defaultName)) {
            ಪ್ರತ್ಯಾಗಮ toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } ಅನ್ಯಥಾ ಯದಿ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        ಯದಿ(length <= capacity) {
            destLength = length;
        } ಅನ್ಯಥಾ {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(target, capacity, length, &status);
    } ಅನ್ಯಥಾ {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    ಯದಿ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } ಅನ್ಯಥಾ {
        ucnv_close(converter);
    }

    ಪ್ರತ್ಯಾಗಮ length;
}

int32_t
UnicodeString::extract(ವರ್ಣ *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ಧ್ರುವ
{
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // nothing to do?
    ಯದಿ(isEmpty()) {
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    ಯದಿ (cnv == nullptr) {
        isDefaultConverter=ಸತ್ಯ;
        cnv=u_getDefaultConverter(&errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    } ಅನ್ಯಥಾ {
        isDefaultConverter=ಅಸತ್ಯ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    ಯದಿ(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ಪ್ರತ್ಯಾಗಮ len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         ವರ್ಣ *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ಧ್ರುವ
{
    ಯದಿ(U_FAILURE(errorCode)) {
        ಯದಿ(destCapacity!=0) {
            *dest=0;
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಧ್ರುವ char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    ವರ್ಣ *originalDest=dest;
    ಧ್ರುವ ವರ್ಣ *destLimit;

    ಯದಿ(destCapacity==0) {
        destLimit=dest=nullptr;
    } ಅನ್ಯಥಾ ಯದಿ(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<ವರ್ಣ*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } ಅನ್ಯಥಾ {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, ಸತ್ಯ, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    ಯದಿ(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        ವರ್ಣ buffer[1024];

        destLimit=buffer+ಪರಿಮಾಣ(buffer);
        ಕುರು {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, ಸತ್ಯ, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } ಯಾವತ್(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    ಯದಿ (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    ಪ್ರತ್ಯಾಗಮ u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

ಶೂನ್ಯ
UnicodeString::doCodepageCreate(ಧ್ರುವ ವರ್ಣ *codepageData,
                                int32_t dataLength,
                                ಧ್ರುವ ವರ್ಣ *codepage)
{
    // if there's nothing to convert, do nothing
    ಯದಿ (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    ಯದಿ (codepage == nullptr) {
        ಧ್ರುವ ವರ್ಣ *defaultName = ucnv_getDefaultName();
        ಯದಿ(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            ಪ್ರತ್ಯಾಗಮ;
        }
        converter = u_getDefaultConverter(&status);
    } ಅನ್ಯಥಾ ಯದಿ (*codepage == 0) {
        // use the "invariant characters" conversion
        ಯದಿ(cloneArrayIfNeeded(dataLength, dataLength, ಅಸತ್ಯ)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } ಅನ್ಯಥಾ {
            setToBogus();
        }
        ಪ್ರತ್ಯಾಗಮ;
    } ಅನ್ಯಥಾ {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    ಯದಿ(U_FAILURE(status)) {
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    ಯದಿ(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    ಯದಿ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } ಅನ್ಯಥಾ {
        ucnv_close(converter);
    }
}

ಶೂನ್ಯ
UnicodeString::doCodepageCreate(ಧ್ರುವ ವರ್ಣ *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    ಯದಿ(U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    // set up the conversion parameters
    ಧ್ರುವ ವರ್ಣ *mySource     = codepageData;
    ಧ್ರುವ ವರ್ಣ *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    ಯದಿ(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } ಅನ್ಯಥಾ {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = ಅಸತ್ಯ;
    ಪ್ರತಿ(;;) {
        ಯದಿ(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            ಭಙ್ಗ;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, ಸತ್ಯ, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        ಯದಿ(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = ಸತ್ಯ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } ಅನ್ಯಥಾ {
            ಯದಿ (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            ಭಙ್ಗ;
        }
    }
}

U_NAMESPACE_END

#endif
