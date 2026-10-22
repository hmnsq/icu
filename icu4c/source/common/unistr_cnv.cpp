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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "ucnv_imp.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "ustr_cnv.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#જો !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    જો(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    જો(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData,
                             અચલ અક્ષર *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    જો (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData,
                             int32_t dataLength,
                             અચલ અક્ષર *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    જો (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(અચલ અક્ષર *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    જો(U_SUCCESS(errorCode)) {
        // check arguments
        જો(src==nullptr) {
            // treat as an empty string, do nothing more
        } નહીંતર જો(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } નહીંતર {
            // get input length
            જો(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            જો(srcLength>0) {
                જો (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } નહીંતર {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        જો(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#જો !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       અક્ષર *target,
                       uint32_t dstSize) અચલ {
    પાછા extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       અક્ષર *target,
                       uint32_t dstSize,
                       અચલ અક્ષર *codepage) અચલ
{
    // if the arguments are illegal, then do nothing
    જો (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        પાછા 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    જો(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } નહીંતર {
        // Pin the capacity so that a limit pointer does not wrap around.
        અક્ષર* targetLimit = static_cast<અક્ષર*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    જો(length == 0) {
        પાછા u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    જો (codepage == nullptr) {
        અચલ અક્ષર *defaultName = ucnv_getDefaultName();
        જો(UCNV_FAST_IS_UTF8(defaultName)) {
            પાછા toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } નહીંતર જો (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        જો(length <= capacity) {
            destLength = length;
        } નહીંતર {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        પાછા u_terminateChars(target, capacity, length, &status);
    } નહીંતર {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    જો (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } નહીંતર {
        ucnv_close(converter);
    }

    પાછા length;
}

int32_t
UnicodeString::extract(અક્ષર *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) અચલ
{
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }

    જો (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    // nothing to do?
    જો(isEmpty()) {
        પાછા u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    જો (cnv == nullptr) {
        isDefaultConverter=સાચું;
        cnv=u_getDefaultConverter(&errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
    } નહીંતર {
        isDefaultConverter=ખોટું;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    જો(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    પાછા len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         અક્ષર *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) અચલ
{
    જો(U_FAILURE(errorCode)) {
        જો(destCapacity!=0) {
            *dest=0;
        }
        પાછા 0;
    }

    અચલ char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    અક્ષર *originalDest=dest;
    અચલ અક્ષર *destLimit;

    જો(destCapacity==0) {
        destLimit=dest=nullptr;
    } નહીંતર જો(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<અક્ષર*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } નહીંતર {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, સાચું, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    જો(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        અક્ષર buffer[1024];

        destLimit=buffer+કદ(buffer);
        કરો {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, સાચું, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } જ્યારે(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    જો (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    પાછા u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

શૂન્ય
UnicodeString::doCodepageCreate(અચલ અક્ષર *codepageData,
                                int32_t dataLength,
                                અચલ અક્ષર *codepage)
{
    // if there's nothing to convert, do nothing
    જો (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        પાછા;
    }
    જો(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    જો (codepage == nullptr) {
        અચલ અક્ષર *defaultName = ucnv_getDefaultName();
        જો(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            પાછા;
        }
        converter = u_getDefaultConverter(&status);
    } નહીંતર જો (*codepage == 0) {
        // use the "invariant characters" conversion
        જો(cloneArrayIfNeeded(dataLength, dataLength, ખોટું)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } નહીંતર {
            setToBogus();
        }
        પાછા;
    } નહીંતર {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    જો(U_FAILURE(status)) {
        setToBogus();
        પાછા;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    જો(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    જો (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } નહીંતર {
        ucnv_close(converter);
    }
}

શૂન્ય
UnicodeString::doCodepageCreate(અચલ અક્ષર *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    જો(U_FAILURE(status)) {
        પાછા;
    }

    // set up the conversion parameters
    અચલ અક્ષર *mySource     = codepageData;
    અચલ અક્ષર *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    જો(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } નહીંતર {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = ખોટું;
    માટે(;;) {
        જો(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            તોડો;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, સાચું, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        જો(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = સાચું;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } નહીંતર {
            જો (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            તોડો;
        }
    }
}

U_NAMESPACE_END

#endif
