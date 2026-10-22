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

#𐡄𐡍 !UCONFIG_NO_CONVERSION

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

#𐡄𐡍 !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    𐡄𐡍(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    𐡄𐡍(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             ثابت حرف *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    𐡄𐡍 (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *codepageData,
                             int32_t dataLength,
                             ثابت حرف *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    𐡄𐡍 (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    𐡄𐡍(U_SUCCESS(errorCode)) {
        // check arguments
        𐡄𐡍(src==nullptr) {
            // treat as an empty string, do nothing more
        } وإلا 𐡄𐡍(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } وإلا {
            // get input length
            𐡄𐡍(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            𐡄𐡍(srcLength>0) {
                𐡄𐡍 (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } وإلا {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        𐡄𐡍(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#𐡄𐡍 !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       حرف *target,
                       uint32_t dstSize) ثابت {
    أرجع extract(start, length, target, dstSize, 0);
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
    𐡄𐡍 (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        أرجع 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    𐡄𐡍(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } وإلا {
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
    𐡄𐡍(length == 0) {
        أرجع u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    𐡄𐡍 (codepage == nullptr) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        𐡄𐡍(UCNV_FAST_IS_UTF8(defaultName)) {
            أرجع toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } وإلا 𐡄𐡍 (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        𐡄𐡍(length <= capacity) {
            destLength = length;
        } وإلا {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        أرجع u_terminateChars(target, capacity, length, &status);
    } وإلا {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    𐡄𐡍 (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }

    أرجع length;
}

int32_t
UnicodeString::extract(حرف *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ثابت
{
    𐡄𐡍(U_FAILURE(errorCode)) {
        أرجع 0;
    }

    𐡄𐡍 (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    // nothing to do?
    𐡄𐡍(isEmpty()) {
        أرجع u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    𐡄𐡍 (cnv == nullptr) {
        isDefaultConverter=𐡒𐡔𐡈;
        cnv=u_getDefaultConverter(&errorCode);
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع 0;
        }
    } وإلا {
        isDefaultConverter=خطأ;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    𐡄𐡍(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    أرجع len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         حرف *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ثابت
{
    𐡄𐡍(U_FAILURE(errorCode)) {
        𐡄𐡍(destCapacity!=0) {
            *dest=0;
        }
        أرجع 0;
    }

    ثابت char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    حرف *originalDest=dest;
    ثابت حرف *destLimit;

    𐡄𐡍(destCapacity==0) {
        destLimit=dest=nullptr;
    } وإلا 𐡄𐡍(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<حرف*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } وإلا {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, 𐡒𐡔𐡈, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    𐡄𐡍(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        حرف buffer[1024];

        destLimit=buffer+حجم(buffer);
        𐡏𐡁𐡃 {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, 𐡒𐡔𐡈, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } طالما(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    𐡄𐡍 (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    أرجع u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepageData,
                                int32_t dataLength,
                                ثابت حرف *codepage)
{
    // if there's nothing to convert, do nothing
    𐡄𐡍 (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        أرجع;
    }
    𐡄𐡍(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    𐡄𐡍 (codepage == nullptr) {
        ثابت حرف *defaultName = ucnv_getDefaultName();
        𐡄𐡍(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            أرجع;
        }
        converter = u_getDefaultConverter(&status);
    } وإلا 𐡄𐡍 (*codepage == 0) {
        // use the "invariant characters" conversion
        𐡄𐡍(cloneArrayIfNeeded(dataLength, dataLength, خطأ)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } وإلا {
            setToBogus();
        }
        أرجع;
    } وإلا {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    𐡄𐡍(U_FAILURE(status)) {
        setToBogus();
        أرجع;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    𐡄𐡍(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    𐡄𐡍 (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } وإلا {
        ucnv_close(converter);
    }
}

فراغ
UnicodeString::doCodepageCreate(ثابت حرف *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    𐡄𐡍(U_FAILURE(status)) {
        أرجع;
    }

    // set up the conversion parameters
    ثابت حرف *mySource     = codepageData;
    ثابت حرف *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    𐡄𐡍(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } وإلا {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = خطأ;
    لكل(;;) {
        𐡄𐡍(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            اكسر;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, 𐡒𐡔𐡈, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        𐡄𐡍(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = 𐡒𐡔𐡈;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } وإلا {
            𐡄𐡍 (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            اكسر;
        }
    }
}

U_NAMESPACE_END

#endif
