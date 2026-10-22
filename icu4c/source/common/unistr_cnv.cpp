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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_imp.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "ustr_cnv.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#ଯଦି !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଯଦି(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଯଦି(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData,
                             ଅଚଳ ଅକ୍ଷର *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଯଦି (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData,
                             int32_t dataLength,
                             ଅଚଳ ଅକ୍ଷର *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଯଦି (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଯଦି(U_SUCCESS(errorCode)) {
        // check arguments
        ଯଦି(src==nullptr) {
            // treat as an empty string, do nothing more
        } ଅନ୍ୟଥା ଯଦି(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } ଅନ୍ୟଥା {
            // get input length
            ଯଦି(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            ଯଦି(srcLength>0) {
                ଯଦି (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } ଅନ୍ୟଥା {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        ଯଦି(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#ଯଦି !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ଅକ୍ଷର *target,
                       uint32_t dstSize) ଅଚଳ {
    ଫେରନ୍ତୁ extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ଅକ୍ଷର *target,
                       uint32_t dstSize,
                       ଅଚଳ ଅକ୍ଷର *codepage) ଅଚଳ
{
    // if the arguments are illegal, then do nothing
    ଯଦି (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        ଫେରନ୍ତୁ 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    ଯଦି(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } ଅନ୍ୟଥା {
        // Pin the capacity so that a limit pointer does not wrap around.
        ଅକ୍ଷର* targetLimit = static_cast<ଅକ୍ଷର*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    ଯଦି(length == 0) {
        ଫେରନ୍ତୁ u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    ଯଦି (codepage == nullptr) {
        ଅଚଳ ଅକ୍ଷର *defaultName = ucnv_getDefaultName();
        ଯଦି(UCNV_FAST_IS_UTF8(defaultName)) {
            ଫେରନ୍ତୁ toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } ଅନ୍ୟଥା ଯଦି (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        ଯଦି(length <= capacity) {
            destLength = length;
        } ଅନ୍ୟଥା {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        ଫେରନ୍ତୁ u_terminateChars(target, capacity, length, &status);
    } ଅନ୍ୟଥା {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    ଯଦି (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } ଅନ୍ୟଥା {
        ucnv_close(converter);
    }

    ଫେରନ୍ତୁ length;
}

int32_t
UnicodeString::extract(ଅକ୍ଷର *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ଅଚଳ
{
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    // nothing to do?
    ଯଦି(isEmpty()) {
        ଫେରନ୍ତୁ u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    ଯଦି (cnv == nullptr) {
        isDefaultConverter=ସତ୍ୟ;
        cnv=u_getDefaultConverter(&errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
    } ଅନ୍ୟଥା {
        isDefaultConverter=ମିଥ୍ୟା;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    ଯଦି(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    ଫେରନ୍ତୁ len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         ଅକ୍ଷର *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ଅଚଳ
{
    ଯଦି(U_FAILURE(errorCode)) {
        ଯଦି(destCapacity!=0) {
            *dest=0;
        }
        ଫେରନ୍ତୁ 0;
    }

    ଅଚଳ char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    ଅକ୍ଷର *originalDest=dest;
    ଅଚଳ ଅକ୍ଷର *destLimit;

    ଯଦି(destCapacity==0) {
        destLimit=dest=nullptr;
    } ଅନ୍ୟଥା ଯଦି(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<ଅକ୍ଷର*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } ଅନ୍ୟଥା {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, ସତ୍ୟ, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    ଯଦି(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        ଅକ୍ଷର buffer[1024];

        destLimit=buffer+ଆକାର(buffer);
        କର {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, ସତ୍ୟ, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } ଯେତେବେଳେ(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    ଯଦି (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    ଫେରନ୍ତୁ u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

ଶୂନ୍ୟ
UnicodeString::doCodepageCreate(ଅଚଳ ଅକ୍ଷର *codepageData,
                                int32_t dataLength,
                                ଅଚଳ ଅକ୍ଷର *codepage)
{
    // if there's nothing to convert, do nothing
    ଯଦି (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    ଯଦି (codepage == nullptr) {
        ଅଚଳ ଅକ୍ଷର *defaultName = ucnv_getDefaultName();
        ଯଦି(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            ଫେରନ୍ତୁ;
        }
        converter = u_getDefaultConverter(&status);
    } ଅନ୍ୟଥା ଯଦି (*codepage == 0) {
        // use the "invariant characters" conversion
        ଯଦି(cloneArrayIfNeeded(dataLength, dataLength, ମିଥ୍ୟା)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } ଅନ୍ୟଥା {
            setToBogus();
        }
        ଫେରନ୍ତୁ;
    } ଅନ୍ୟଥା {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    ଯଦି(U_FAILURE(status)) {
        setToBogus();
        ଫେରନ୍ତୁ;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    ଯଦି(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    ଯଦି (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } ଅନ୍ୟଥା {
        ucnv_close(converter);
    }
}

ଶୂନ୍ୟ
UnicodeString::doCodepageCreate(ଅଚଳ ଅକ୍ଷର *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    ଯଦି(U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    // set up the conversion parameters
    ଅଚଳ ଅକ୍ଷର *mySource     = codepageData;
    ଅଚଳ ଅକ୍ଷର *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    ଯଦି(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } ଅନ୍ୟଥା {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = ମିଥ୍ୟା;
    ପାଇଁ(;;) {
        ଯଦି(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            ଭାଙ୍ଗ;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, ସତ୍ୟ, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        ଯଦି(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = ସତ୍ୟ;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } ଅନ୍ୟଥା {
            ଯଦି (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            ଭାଙ୍ଗ;
        }
    }
}

U_NAMESPACE_END

#endif
