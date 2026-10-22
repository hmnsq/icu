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

#אם !UCONFIG_NO_CONVERSION

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

#אם !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    אם(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    אם(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             ثابت حرف *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    אם (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             int32_t dataLength,
                             ثابت حرف *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    אם (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    אם(U_SUCCESS(errorCode)) {
        // check arguments
        אם(src==nullptr) {
            // treat as an empty string, do nothing more
        } אחרת אם(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } אחרת {
            // get input length
            אם(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            אם(srcLength>0) {
                אם (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } אחרת {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        אם(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#אם !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       حرف *target,
                       uint32_t dstSize) ثابت {
    החזר extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       حرف *target,
                       uint32_t dstSize,
                       ثابت حرف *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    אם (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        החזר 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    אם(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } אחרת {
        // Pin the capacity so that a limit pointer does not wrap around.
        حرف* targetLimit = static_cast<حرف*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    אם(length == 0) {
        החזר u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    אם (codepage == nullptr) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        אם(UCNV_FAST_IS_UTF8(defaultName)) {
            החזר toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } אחרת אם (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        אם(length <= capacity) {
            destLength = length;
        } אחרת {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        החזר u_terminateChars(target, capacity, length, &status);
    } אחרת {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    אם (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } אחרת {
        ucnv_close(converter);
    }

    החזר length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ثابت
{
    אם(U_FAILURE(errorCode)) {
        החזר 0;
    }

    אם (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        החזר 0;
    }

    // nothing to do?
    אם(isEmpty()) {
        החזר u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    אם (cnv == nullptr) {
        isDefaultConverter=אמת;
        cnv=u_getDefaultConverter(&errorCode);
        אם(U_FAILURE(errorCode)) {
            החזר 0;
        }
    } אחרת {
        isDefaultConverter=שקר;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    אם(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    החזר len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         حرف *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ثابت
{
    אם(U_FAILURE(errorCode)) {
        אם(destCapacity!=0) {
            *dest=0;
        }
        החזר 0;
    }

    ثابت char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    حرف *originalDest=dest;
    ثابت حرف *destLimit;

    אם(destCapacity==0) {
        destLimit=dest=nullptr;
    } אחרת אם(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } אחרת {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, אמת, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    אם(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        حرف buffer[1024];

        destLimit=buffer+حجم(buffer);
        עשה {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, אמת, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } כלעוד(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    אם (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    החזר u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepageData,
                                int32_t dataLength,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    אם (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        החזר;
    }
    אם(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    אם (codepage == nullptr) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        אם(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            החזר;
        }
        converter = u_getDefaultConverter(&status);
    } אחרת אם (*codepage == 0) {
        // use the "invariant characters" conversion
        אם(cloneArrayIfNeeded(dataLength, dataLength, שקר)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } אחרת {
            setToBogus();
        }
        החזר;
    } אחרת {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    אם(U_FAILURE(status)) {
        setToBogus();
        החזר;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    אם(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    אם (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } אחרת {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    אם(U_FAILURE(status)) {
        החזר;
    }

    // set up the conversion parameters
    ثابت حرف *mySource     = codepageData;
    ثابت حرف *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    אם(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } אחרת {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = שקר;
    עבור(;;) {
        אם(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            اكسر;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, אמת, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        אם(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = אמת;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } אחרת {
            אם (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_END

#endif
