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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "ucnv_imp.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "ustr_cnv.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#എങ്കിൽ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    എങ്കിൽ(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    എങ്കിൽ(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData,
                             സ്ഥിരം അക്ഷരം *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    എങ്കിൽ (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData,
                             int32_t dataLength,
                             സ്ഥിരം അക്ഷരം *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    എങ്കിൽ (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        // check arguments
        എങ്കിൽ(src==nullptr) {
            // treat as an empty string, do nothing more
        } അല്ലെങ്കിൽ എങ്കിൽ(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } അല്ലെങ്കിൽ {
            // get input length
            എങ്കിൽ(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            എങ്കിൽ(srcLength>0) {
                എങ്കിൽ (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } അല്ലെങ്കിൽ {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        എങ്കിൽ(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#എങ്കിൽ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       അക്ഷരം *target,
                       uint32_t dstSize) സ്ഥിരം {
    തിരികെ extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       അക്ഷരം *target,
                       uint32_t dstSize,
                       സ്ഥിരം അക്ഷരം *codepage) സ്ഥിരം
{
    // if the arguments are illegal, then do nothing
    എങ്കിൽ (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        തിരികെ 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    എങ്കിൽ(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } അല്ലെങ്കിൽ {
        // Pin the capacity so that a limit pointer does not wrap around.
        അക്ഷരം* targetLimit = static_cast<അക്ഷരം*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    എങ്കിൽ(length == 0) {
        തിരികെ u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    എങ്കിൽ (codepage == nullptr) {
        സ്ഥിരം അക്ഷരം *defaultName = ucnv_getDefaultName();
        എങ്കിൽ(UCNV_FAST_IS_UTF8(defaultName)) {
            തിരികെ toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } അല്ലെങ്കിൽ എങ്കിൽ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        എങ്കിൽ(length <= capacity) {
            destLength = length;
        } അല്ലെങ്കിൽ {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        തിരികെ u_terminateChars(target, capacity, length, &status);
    } അല്ലെങ്കിൽ {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    എങ്കിൽ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } അല്ലെങ്കിൽ {
        ucnv_close(converter);
    }

    തിരികെ length;
}

int32_t
UnicodeString::extract(അക്ഷരം *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) സ്ഥിരം
{
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }

    എങ്കിൽ (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    // nothing to do?
    എങ്കിൽ(isEmpty()) {
        തിരികെ u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    എങ്കിൽ (cnv == nullptr) {
        isDefaultConverter=സത്യം;
        cnv=u_getDefaultConverter(&errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
    } അല്ലെങ്കിൽ {
        isDefaultConverter=അസത്യം;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    എങ്കിൽ(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    തിരികെ len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         അക്ഷരം *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) സ്ഥിരം
{
    എങ്കിൽ(U_FAILURE(errorCode)) {
        എങ്കിൽ(destCapacity!=0) {
            *dest=0;
        }
        തിരികെ 0;
    }

    സ്ഥിരം char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    അക്ഷരം *originalDest=dest;
    സ്ഥിരം അക്ഷരം *destLimit;

    എങ്കിൽ(destCapacity==0) {
        destLimit=dest=nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<അക്ഷരം*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } അല്ലെങ്കിൽ {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, സത്യം, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    എങ്കിൽ(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        അക്ഷരം buffer[1024];

        destLimit=buffer+വലുപ്പം(buffer);
        ചെയ്യുക {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, സത്യം, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } ആകുമ്പോൾ(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    എങ്കിൽ (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    തിരികെ u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

ശൂന്യം
UnicodeString::doCodepageCreate(സ്ഥിരം അക്ഷരം *codepageData,
                                int32_t dataLength,
                                സ്ഥിരം അക്ഷരം *codepage)
{
    // if there's nothing to convert, do nothing
    എങ്കിൽ (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        തിരികെ;
    }
    എങ്കിൽ(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    എങ്കിൽ (codepage == nullptr) {
        സ്ഥിരം അക്ഷരം *defaultName = ucnv_getDefaultName();
        എങ്കിൽ(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            തിരികെ;
        }
        converter = u_getDefaultConverter(&status);
    } അല്ലെങ്കിൽ എങ്കിൽ (*codepage == 0) {
        // use the "invariant characters" conversion
        എങ്കിൽ(cloneArrayIfNeeded(dataLength, dataLength, അസത്യം)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } അല്ലെങ്കിൽ {
            setToBogus();
        }
        തിരികെ;
    } അല്ലെങ്കിൽ {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    എങ്കിൽ(U_FAILURE(status)) {
        setToBogus();
        തിരികെ;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    എങ്കിൽ(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    എങ്കിൽ (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } അല്ലെങ്കിൽ {
        ucnv_close(converter);
    }
}

ശൂന്യം
UnicodeString::doCodepageCreate(സ്ഥിരം അക്ഷരം *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ;
    }

    // set up the conversion parameters
    സ്ഥിരം അക്ഷരം *mySource     = codepageData;
    സ്ഥിരം അക്ഷരം *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    എങ്കിൽ(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } അല്ലെങ്കിൽ {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = അസത്യം;
    വേണ്ടി(;;) {
        എങ്കിൽ(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            നിർത്തുക;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, സത്യം, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        എങ്കിൽ(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = സത്യം;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            നിർത്തുക;
        }
    }
}

U_NAMESPACE_END

#endif
