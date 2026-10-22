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

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ustr_cnv.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#सचे !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    सचे(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    सचे(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData,
                             ध्रुव वर्ण *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    सचे (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData,
                             int32_t dataLength,
                             ध्रुव वर्ण *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    सचे (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    सचे(U_SUCCESS(errorCode)) {
        // check arguments
        सचे(src==nullptr) {
            // treat as an empty string, do nothing more
        } अञ्ञथा सचे(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } अञ्ञथा {
            // get input length
            सचे(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            सचे(srcLength>0) {
                सचे (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } अञ्ञथा {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        सचे(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#सचे !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       uint32_t dstSize) ध्रुव {
    पटिनिवत्त extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       uint32_t dstSize,
                       ध्रुव वर्ण *codepage) ध्रुव
{
    // if the arguments are illegal, then do nothing
    सचे (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        पटिनिवत्त 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    सचे(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } अञ्ञथा {
        // Pin the capacity so that a limit pointer does not wrap around.
        वर्ण* targetLimit = static_cast<वर्ण*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    सचे(length == 0) {
        पटिनिवत्त u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    सचे (codepage == nullptr) {
        ध्रुव वर्ण *defaultName = ucnv_getDefaultName();
        सचे(UCNV_FAST_IS_UTF8(defaultName)) {
            पटिनिवत्त toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } अञ्ञथा सचे (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        सचे(length <= capacity) {
            destLength = length;
        } अञ्ञथा {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        पटिनिवत्त u_terminateChars(target, capacity, length, &status);
    } अञ्ञथा {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    सचे (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अञ्ञथा {
        ucnv_close(converter);
    }

    पटिनिवत्त length;
}

int32_t
UnicodeString::extract(वर्ण *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ध्रुव
{
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }

    सचे (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    // nothing to do?
    सचे(isEmpty()) {
        पटिनिवत्त u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    सचे (cnv == nullptr) {
        isDefaultConverter=सच्च;
        cnv=u_getDefaultConverter(&errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
    } अञ्ञथा {
        isDefaultConverter=मिच्छा;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    सचे(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    पटिनिवत्त len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         वर्ण *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ध्रुव
{
    सचे(U_FAILURE(errorCode)) {
        सचे(destCapacity!=0) {
            *dest=0;
        }
        पटिनिवत्त 0;
    }

    ध्रुव char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    वर्ण *originalDest=dest;
    ध्रुव वर्ण *destLimit;

    सचे(destCapacity==0) {
        destLimit=dest=nullptr;
    } अञ्ञथा सचे(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<वर्ण*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } अञ्ञथा {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    सचे(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        वर्ण buffer[1024];

        destLimit=buffer+परिमाण(buffer);
        करोहि {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सच्च, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } याव(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    सचे (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    पटिनिवत्त u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

शून्य
UnicodeString::doCodepageCreate(ध्रुव वर्ण *codepageData,
                                int32_t dataLength,
                                ध्रुव वर्ण *codepage)
{
    // if there's nothing to convert, do nothing
    सचे (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        पटिनिवत्त;
    }
    सचे(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    सचे (codepage == nullptr) {
        ध्रुव वर्ण *defaultName = ucnv_getDefaultName();
        सचे(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            पटिनिवत्त;
        }
        converter = u_getDefaultConverter(&status);
    } अञ्ञथा सचे (*codepage == 0) {
        // use the "invariant characters" conversion
        सचे(cloneArrayIfNeeded(dataLength, dataLength, मिच्छा)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } अञ्ञथा {
            setToBogus();
        }
        पटिनिवत्त;
    } अञ्ञथा {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    सचे(U_FAILURE(status)) {
        setToBogus();
        पटिनिवत्त;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    सचे(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    सचे (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अञ्ञथा {
        ucnv_close(converter);
    }
}

शून्य
UnicodeString::doCodepageCreate(ध्रुव वर्ण *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    सचे(U_FAILURE(status)) {
        पटिनिवत्त;
    }

    // set up the conversion parameters
    ध्रुव वर्ण *mySource     = codepageData;
    ध्रुव वर्ण *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    सचे(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } अञ्ञथा {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = मिच्छा;
    अत्थाय(;;) {
        सचे(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            भङ्ग;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, सच्च, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        सचे(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = सच्च;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } अञ्ञथा {
            सचे (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            भङ्ग;
        }
    }
}

U_NAMESPACE_END

#endif
