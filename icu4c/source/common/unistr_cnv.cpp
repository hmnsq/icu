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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "ucnv_imp.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "ustr_cnv.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#எனில் !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    எனில்(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    எனில்(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData,
                             மாறிலி எழுத்து *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    எனில் (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData,
                             int32_t dataLength,
                             மாறிலி எழுத்து *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    எனில் (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(மாறிலி எழுத்து *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    எனில்(U_SUCCESS(errorCode)) {
        // check arguments
        எனில்(src==nullptr) {
            // treat as an empty string, do nothing more
        } இல்லையெனில் எனில்(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } இல்லையெனில் {
            // get input length
            எனில்(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            எனில்(srcLength>0) {
                எனில் (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } இல்லையெனில் {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        எனில்(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#எனில் !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       எழுத்து *target,
                       uint32_t dstSize) மாறிலி {
    திரும்பு extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       எழுத்து *target,
                       uint32_t dstSize,
                       மாறிலி எழுத்து *codepage) மாறிலி
{
    // if the arguments are illegal, then do nothing
    எனில் (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        திரும்பு 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    எனில்(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } இல்லையெனில் {
        // Pin the capacity so that a limit pointer does not wrap around.
        எழுத்து* targetLimit = static_cast<எழுத்து*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    எனில்(length == 0) {
        திரும்பு u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    எனில் (codepage == nullptr) {
        மாறிலி எழுத்து *defaultName = ucnv_getDefaultName();
        எனில்(UCNV_FAST_IS_UTF8(defaultName)) {
            திரும்பு toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } இல்லையெனில் எனில் (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        எனில்(length <= capacity) {
            destLength = length;
        } இல்லையெனில் {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        திரும்பு u_terminateChars(target, capacity, length, &status);
    } இல்லையெனில் {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    எனில் (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } இல்லையெனில் {
        ucnv_close(converter);
    }

    திரும்பு length;
}

int32_t
UnicodeString::extract(எழுத்து *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) மாறிலி
{
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }

    எனில் (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    // nothing to do?
    எனில்(isEmpty()) {
        திரும்பு u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    எனில் (cnv == nullptr) {
        isDefaultConverter=உண்மை;
        cnv=u_getDefaultConverter(&errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
    } இல்லையெனில் {
        isDefaultConverter=பொய்;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    எனில்(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    திரும்பு len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         எழுத்து *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) மாறிலி
{
    எனில்(U_FAILURE(errorCode)) {
        எனில்(destCapacity!=0) {
            *dest=0;
        }
        திரும்பு 0;
    }

    மாறிலி char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    எழுத்து *originalDest=dest;
    மாறிலி எழுத்து *destLimit;

    எனில்(destCapacity==0) {
        destLimit=dest=nullptr;
    } இல்லையெனில் எனில்(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<எழுத்து*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } இல்லையெனில் {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, உண்மை, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    எனில்(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        எழுத்து buffer[1024];

        destLimit=buffer+அளவு(buffer);
        செய் {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, உண்மை, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } போது(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    எனில் (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    திரும்பு u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

வெறுமை
UnicodeString::doCodepageCreate(மாறிலி எழுத்து *codepageData,
                                int32_t dataLength,
                                மாறிலி எழுத்து *codepage)
{
    // if there's nothing to convert, do nothing
    எனில் (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        திரும்பு;
    }
    எனில்(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    எனில் (codepage == nullptr) {
        மாறிலி எழுத்து *defaultName = ucnv_getDefaultName();
        எனில்(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            திரும்பு;
        }
        converter = u_getDefaultConverter(&status);
    } இல்லையெனில் எனில் (*codepage == 0) {
        // use the "invariant characters" conversion
        எனில்(cloneArrayIfNeeded(dataLength, dataLength, பொய்)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } இல்லையெனில் {
            setToBogus();
        }
        திரும்பு;
    } இல்லையெனில் {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    எனில்(U_FAILURE(status)) {
        setToBogus();
        திரும்பு;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    எனில்(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    எனில் (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } இல்லையெனில் {
        ucnv_close(converter);
    }
}

வெறுமை
UnicodeString::doCodepageCreate(மாறிலி எழுத்து *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    எனில்(U_FAILURE(status)) {
        திரும்பு;
    }

    // set up the conversion parameters
    மாறிலி எழுத்து *mySource     = codepageData;
    மாறிலி எழுத்து *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    எனில்(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } இல்லையெனில் {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = பொய்;
    க்கு(;;) {
        எனில்(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            நிறுத்து;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, உண்மை, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        எனில்(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = உண்மை;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } இல்லையெனில் {
            எனில் (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            நிறுத்து;
        }
    }
}

U_NAMESPACE_END

#endif
