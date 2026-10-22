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

#जर !UCONFIG_NO_CONVERSION

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

#जर !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(स्थिरांक अक्षर *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    जर(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(स्थिरांक अक्षर *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    जर(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(स्थिरांक अक्षर *codepageData,
                             स्थिरांक अक्षर *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    जर (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(स्थिरांक अक्षर *codepageData,
                             int32_t dataLength,
                             स्थिरांक अक्षर *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    जर (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(स्थिरांक अक्षर *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    जर(U_SUCCESS(errorCode)) {
        // check arguments
        जर(src==nullptr) {
            // treat as an empty string, do nothing more
        } अन्यथा जर(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } अन्यथा {
            // get input length
            जर(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            जर(srcLength>0) {
                जर (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } अन्यथा {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        जर(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#जर !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       अक्षर *target,
                       uint32_t dstSize) स्थिरांक {
    परत extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       अक्षर *target,
                       uint32_t dstSize,
                       स्थिरांक अक्षर *codepage) स्थिरांक
{
    // if the arguments are illegal, then do nothing
    जर (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        परत 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    जर(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } अन्यथा {
        // Pin the capacity so that a limit pointer does not wrap around.
        अक्षर* targetLimit = static_cast<अक्षर*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    जर(length == 0) {
        परत u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    जर (codepage == nullptr) {
        स्थिरांक अक्षर *defaultName = ucnv_getDefaultName();
        जर(UCNV_FAST_IS_UTF8(defaultName)) {
            परत toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } अन्यथा जर (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        जर(length <= capacity) {
            destLength = length;
        } अन्यथा {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        परत u_terminateChars(target, capacity, length, &status);
    } अन्यथा {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    जर (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }

    परत length;
}

int32_t
UnicodeString::extract(अक्षर *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) स्थिरांक
{
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }

    जर (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    // nothing to do?
    जर(isEmpty()) {
        परत u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    जर (cnv == nullptr) {
        isDefaultConverter=सत्य;
        cnv=u_getDefaultConverter(&errorCode);
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
    } अन्यथा {
        isDefaultConverter=असत्य;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    जर(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    परत len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         अक्षर *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) स्थिरांक
{
    जर(U_FAILURE(errorCode)) {
        जर(destCapacity!=0) {
            *dest=0;
        }
        परत 0;
    }

    स्थिरांक char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    अक्षर *originalDest=dest;
    स्थिरांक अक्षर *destLimit;

    जर(destCapacity==0) {
        destLimit=dest=nullptr;
    } अन्यथा जर(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<अक्षर*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } अन्यथा {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सत्य, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    जर(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        अक्षर buffer[1024];

        destLimit=buffer+आकार(buffer);
        करा {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, सत्य, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } पर्यंत(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    जर (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    परत u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

शून्य
UnicodeString::doCodepageCreate(स्थिरांक अक्षर *codepageData,
                                int32_t dataLength,
                                स्थिरांक अक्षर *codepage)
{
    // if there's nothing to convert, do nothing
    जर (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        परत;
    }
    जर(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    जर (codepage == nullptr) {
        स्थिरांक अक्षर *defaultName = ucnv_getDefaultName();
        जर(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            परत;
        }
        converter = u_getDefaultConverter(&status);
    } अन्यथा जर (*codepage == 0) {
        // use the "invariant characters" conversion
        जर(cloneArrayIfNeeded(dataLength, dataLength, असत्य)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } अन्यथा {
            setToBogus();
        }
        परत;
    } अन्यथा {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    जर(U_FAILURE(status)) {
        setToBogus();
        परत;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    जर(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    जर (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }
}

शून्य
UnicodeString::doCodepageCreate(स्थिरांक अक्षर *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    जर(U_FAILURE(status)) {
        परत;
    }

    // set up the conversion parameters
    स्थिरांक अक्षर *mySource     = codepageData;
    स्थिरांक अक्षर *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    जर(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } अन्यथा {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = असत्य;
    साठी(;;) {
        जर(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            थांबा;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, सत्य, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        जर(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = सत्य;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } अन्यथा {
            जर (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            थांबा;
        }
    }
}

U_NAMESPACE_END

#endif
