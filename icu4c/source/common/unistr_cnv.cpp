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

UnicodeString::UnicodeString(ثابت نویسه *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    اگر(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ثابت نویسه *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    اگر(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ثابت نویسه *codepageData,
                             ثابت نویسه *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    اگر (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت نویسه *codepageData,
                             int32_t dataLength,
                             ثابت نویسه *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    اگر (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ثابت نویسه *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    اگر(U_SUCCESS(errorCode)) {
        // check arguments
        اگر(src==nullptr) {
            // treat as an empty string, do nothing more
        } وگرنه اگر(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } وگرنه {
            // get input length
            اگر(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            اگر(srcLength>0) {
                اگر (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } وگرنه {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        اگر(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#اگر !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       نویسه *target,
                       uint32_t dstSize) ثابت {
    بازگشت extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       نویسه *target,
                       uint32_t dstSize,
                       ثابت نویسه *codepage) ثابت
{
    // if the arguments are illegal, then do nothing
    اگر (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        بازگشت 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    اگر(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } وگرنه {
        // Pin the capacity so that a limit pointer does not wrap around.
        نویسه* targetLimit = static_cast<نویسه*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    اگر(length == 0) {
        بازگشت u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    اگر (codepage == nullptr) {
        ثابت نویسه *defaultName = ucnv_getDefaultName();
        اگر(UCNV_FAST_IS_UTF8(defaultName)) {
            بازگشت toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } وگرنه اگر (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        اگر(length <= capacity) {
            destLength = length;
        } وگرنه {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        بازگشت u_terminateChars(target, capacity, length, &status);
    } وگرنه {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    اگر (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } وگرنه {
        ucnv_close(converter);
    }

    بازگشت length;
}

int32_t
UnicodeString::extract(نویسه *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ثابت
{
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }

    اگر (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    // nothing to do?
    اگر(isEmpty()) {
        بازگشت u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    اگر (cnv == nullptr) {
        isDefaultConverter=درست;
        cnv=u_getDefaultConverter(&errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
    } وگرنه {
        isDefaultConverter=نادرست;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    اگر(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    بازگشت len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         نویسه *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ثابت
{
    اگر(U_FAILURE(errorCode)) {
        اگر(destCapacity!=0) {
            *dest=0;
        }
        بازگشت 0;
    }

    ثابت char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    نویسه *originalDest=dest;
    ثابت نویسه *destLimit;

    اگر(destCapacity==0) {
        destLimit=dest=nullptr;
    } وگرنه اگر(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<نویسه*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } وگرنه {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    اگر(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        نویسه buffer[1024];

        destLimit=buffer+اندازه(buffer);
        انجامبده {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, درست, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } وقتی(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    اگر (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    بازگشت u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

تهی
UnicodeString::doCodepageCreate(ثابت نویسه *codepageData,
                                int32_t dataLength,
                                ثابت نویسه *codepage)
{
    // if there's nothing to convert, do nothing
    اگر (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        بازگشت;
    }
    اگر(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    اگر (codepage == nullptr) {
        ثابت نویسه *defaultName = ucnv_getDefaultName();
        اگر(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            بازگشت;
        }
        converter = u_getDefaultConverter(&status);
    } وگرنه اگر (*codepage == 0) {
        // use the "invariant characters" conversion
        اگر(cloneArrayIfNeeded(dataLength, dataLength, نادرست)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } وگرنه {
            setToBogus();
        }
        بازگشت;
    } وگرنه {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    اگر(U_FAILURE(status)) {
        setToBogus();
        بازگشت;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    اگر(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    اگر (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } وگرنه {
        ucnv_close(converter);
    }
}

تهی
UnicodeString::doCodepageCreate(ثابت نویسه *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    اگر(U_FAILURE(status)) {
        بازگشت;
    }

    // set up the conversion parameters
    ثابت نویسه *mySource     = codepageData;
    ثابت نویسه *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    اگر(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } وگرنه {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = نادرست;
    برای(;;) {
        اگر(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            بشکن;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, درست, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        اگر(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = درست;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } وگرنه {
            اگر (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            بشکن;
        }
    }
}

U_NAMESPACE_END

#endif
