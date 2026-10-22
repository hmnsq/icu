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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "ustr_cnv.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#যদি !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ধ্রুব বর্ণ *codepageData) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    যদি(codepageData != 0) {
        doCodepageCreate(codepageData, (int32_t)uprv_strlen(codepageData), 0);
    }
}

UnicodeString::UnicodeString(ধ্রুব বর্ণ *codepageData,
                             int32_t dataLength) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    যদি(codepageData != 0) {
        doCodepageCreate(codepageData, dataLength, 0);
    }
}

// else see unistr.cpp
#endif

UnicodeString::UnicodeString(ধ্রুব বর্ণ *codepageData,
                             ধ্রুব বর্ণ *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    যদি (codepageData != nullptr) {
        doCodepageCreate(codepageData, static_cast<int32_t>(uprv_strlen(codepageData)), codepage);
    }
}

UnicodeString::UnicodeString(ধ্রুব বর্ণ *codepageData,
                             int32_t dataLength,
                             ধ্রুব বর্ণ *codepage) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    যদি (codepageData != nullptr) {
        doCodepageCreate(codepageData, dataLength, codepage);
    }
}

UnicodeString::UnicodeString(ধ্রুব বর্ণ *src, int32_t srcLength,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    যদি(U_SUCCESS(errorCode)) {
        // check arguments
        যদি(src==nullptr) {
            // treat as an empty string, do nothing more
        } অন্যথা যদি(srcLength<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } অন্যথা {
            // get input length
            যদি(srcLength==-1) {
                srcLength = static_cast<int32_t>(uprv_strlen(src));
            }
            যদি(srcLength>0) {
                যদি (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                } অন্যথা {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, srcLength, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        যদি(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#যদি !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       বর্ণ *target,
                       uint32_t dstSize) ধ্রুব {
    প্রত্যাগম extract(start, length, target, dstSize, 0);
}

// else see unistr.cpp
#endif

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       বর্ণ *target,
                       uint32_t dstSize,
                       ধ্রুব বর্ণ *codepage) ধ্রুব
{
    // if the arguments are illegal, then do nothing
    যদি (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
        প্রত্যাগম 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    যদি(dstSize < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dstSize);
    } অন্যথা {
        // Pin the capacity so that a limit pointer does not wrap around.
        বর্ণ* targetLimit = static_cast<বর্ণ*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(targetLimit - target);
    }

    // create the converter
    UConverter *converter;
    UErrorCode status = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    যদি(length == 0) {
        প্রত্যাগম u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    যদি (codepage == nullptr) {
        ধ্রুব বর্ণ *defaultName = ucnv_getDefaultName();
        যদি(UCNV_FAST_IS_UTF8(defaultName)) {
            প্রত্যাগম toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } অন্যথা যদি (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t destLength;
        যদি(length <= capacity) {
            destLength = length;
        } অন্যথা {
            destLength = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, destLength);
        প্রত্যাগম u_terminateChars(target, capacity, length, &status);
    } অন্যথা {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    যদি (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } অন্যথা {
        ucnv_close(converter);
    }

    প্রত্যাগম length;
}

int32_t
UnicodeString::extract(বর্ণ *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) ধ্রুব
{
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }

    যদি (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    // nothing to do?
    যদি(isEmpty()) {
        প্রত্যাগম u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    যদি (cnv == nullptr) {
        isDefaultConverter=সত্য;
        cnv=u_getDefaultConverter(&errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
    } অন্যথা {
        isDefaultConverter=অসত্য;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    যদি(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    প্রত্যাগম len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         বর্ণ *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) ধ্রুব
{
    যদি(U_FAILURE(errorCode)) {
        যদি(destCapacity!=0) {
            *dest=0;
        }
        প্রত্যাগম 0;
    }

    ধ্রুব char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    বর্ণ *originalDest=dest;
    ধ্রুব বর্ণ *destLimit;

    যদি(destCapacity==0) {
        destLimit=dest=nullptr;
    } অন্যথা যদি(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<বর্ণ*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } অন্যথা {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode bufferStatus = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &bufferStatus);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    যদি(bufferStatus==U_BUFFER_OVERFLOW_ERROR) {
        বর্ণ buffer[1024];

        destLimit=buffer+পরিমাণ(buffer);
        কুরু {
            dest=buffer;
            bufferStatus=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, সত্য, &bufferStatus);
            length += static_cast<int32_t>(dest - buffer);
        } যাবত্(bufferStatus==U_BUFFER_OVERFLOW_ERROR);
    }
    যদি (U_FAILURE(bufferStatus)) {
        errorCode = bufferStatus;
    }

    প্রত্যাগম u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

শূন্য
UnicodeString::doCodepageCreate(ধ্রুব বর্ণ *codepageData,
                                int32_t dataLength,
                                ধ্রুব বর্ণ *codepage)
{
    // if there's nothing to convert, do nothing
    যদি (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
        প্রত্যাগম;
    }
    যদি(dataLength == -1) {
        dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    যদি (codepage == nullptr) {
        ধ্রুব বর্ণ *defaultName = ucnv_getDefaultName();
        যদি(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepageData, dataLength));
            প্রত্যাগম;
        }
        converter = u_getDefaultConverter(&status);
    } অন্যথা যদি (*codepage == 0) {
        // use the "invariant characters" conversion
        যদি(cloneArrayIfNeeded(dataLength, dataLength, অসত্য)) {
            u_charsToUChars(codepageData, getArrayStart(), dataLength);
            setLength(dataLength);
        } অন্যথা {
            setToBogus();
        }
        প্রত্যাগম;
    } অন্যথা {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    যদি(U_FAILURE(status)) {
        setToBogus();
        প্রত্যাগম;
    }

    // perform the conversion
    doCodepageCreate(codepageData, dataLength, converter, status);
    যদি(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    যদি (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } অন্যথা {
        ucnv_close(converter);
    }
}

শূন্য
UnicodeString::doCodepageCreate(ধ্রুব বর্ণ *codepageData,
                                int32_t dataLength,
                                UConverter *converter,
                                UErrorCode &status)
{
    যদি(U_FAILURE(status)) {
        প্রত্যাগম;
    }

    // set up the conversion parameters
    ধ্রুব বর্ণ *mySource     = codepageData;
    ধ্রুব বর্ণ *mySourceEnd  = mySource + dataLength;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t arraySize;
    যদি(dataLength <= US_STACKBUF_SIZE) {
        // try to use the stack buffer
        arraySize = US_STACKBUF_SIZE;
    } অন্যথা {
        // 1.25 char16_t's per source byte should cover most cases
        arraySize = dataLength + (dataLength >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = অসত্য;
    প্রতি(;;) {
        যদি(!cloneArrayIfNeeded(arraySize, arraySize, doCopyArray)) {
            setToBogus();
            ভঙ্গ;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode bufferStatus = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, সত্য, &bufferStatus);

        // update the conversion parameters
        setLength(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        যদি(bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = সত্য;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            arraySize = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } অন্যথা {
            যদি (U_FAILURE(bufferStatus)) {
                status = bufferStatus;
            }
            ভঙ্গ;
        }
    }
}

U_NAMESPACE_END

#endif
