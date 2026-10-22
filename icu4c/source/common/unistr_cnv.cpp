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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/putil.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/ucnv.h"
#समावेश "ucnv_imp.h"
#समावेश "putilimp.h"
#समावेश "ustr_cnv.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#अगर !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    अगर(codepage_तथ्य != 0) {
        doCodepageCreate(codepage_तथ्य, (int32_t)uprv_strlen(codepage_तथ्य), 0);
    }
}

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य,
                             int32_t तथ्य_लंबाई) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    अगर(codepage_तथ्य != 0) {
        doCodepageCreate(codepage_तथ्य, तथ्य_लंबाई, 0);
    }
}

// else see unistr.cpp
#पूर्ण_अगर

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य,
                             स्थिर अक्षर *codepage) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    अगर (codepage_तथ्य != nullptr) {
        doCodepageCreate(codepage_तथ्य, static_cast<int32_t>(uprv_strlen(codepage_तथ्य)), codepage);
    }
}

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य,
                             int32_t तथ्य_लंबाई,
                             स्थिर अक्षर *codepage) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    अगर (codepage_तथ्य != nullptr) {
        doCodepageCreate(codepage_तथ्य, तथ्य_लंबाई, codepage);
    }
}

UnicodeString::UnicodeString(स्थिर अक्षर *src, int32_t src_लंबाई,
                             UConverter *cnv,
                             UErrorCode &errorCode) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    अगर(U_SUCCESS(errorCode)) {
        // check arguments
        अगर(src==nullptr) {
            // treat as an empty string, do nothing more
        } अन्यथा अगर(src_लंबाई<-1) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        } अन्यथा {
            // get input length
            अगर(src_लंबाई==-1) {
                src_लंबाई = static_cast<int32_t>(uprv_strlen(src));
            }
            अगर(src_लंबाई>0) {
                अगर (cnv != nullptr) {
                    // use the provided converter
                    ucnv_resetToUnicode(cnv);
                    doCodepageCreate(src, src_लंबाई, cnv, errorCode);
                } अन्यथा {
                    // use the default converter
                    cnv=u_getDefaultConverter(&errorCode);
                    doCodepageCreate(src, src_लंबाई, cnv, errorCode);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        अगर(U_FAILURE(errorCode)) {
            setToBogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#अगर !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       अक्षर *target,
                       uint32_t dst_आकार) स्थिर {
    वापस extract(start, length, target, dst_आकार, 0);
}

// else see unistr.cpp
#पूर्ण_अगर

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       अक्षर *target,
                       uint32_t dst_आकार,
                       स्थिर अक्षर *codepage) स्थिर
{
    // if the arguments are illegal, then do nothing
    अगर (/*dstSize < 0 || */(dst_आकार > 0 && target == nullptr)) {
        वापस 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    अगर(dst_आकार < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_आकार);
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
    अगर(length == 0) {
        वापस u_terminateChars(target, capacity, 0, &status);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    अगर (codepage == nullptr) {
        स्थिर अक्षर *defaultName = ucnv_getDefaultName();
        अगर(UCNV_FAST_IS_UTF8(defaultName)) {
            वापस toUTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&status);
    } अन्यथा अगर (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_लंबाई;
        अगर(length <= capacity) {
            dest_लंबाई = length;
        } अन्यथा {
            dest_लंबाई = capacity;
        }
        u_UCharsToChars(getArrayStart() + start, target, dest_लंबाई);
        वापस u_terminateChars(target, capacity, length, &status);
    } अन्यथा {
        converter = ucnv_open(codepage, &status);
    }

    length = doExtract(start, length, target, capacity, converter, status);

    // close the converter
    अगर (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }

    वापस length;
}

int32_t
UnicodeString::extract(अक्षर *dest, int32_t destCapacity,
                       UConverter *cnv,
                       UErrorCode &errorCode) स्थिर
{
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }

    अगर (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    // nothing to do?
    अगर(isEmpty()) {
        वापस u_terminateChars(dest, destCapacity, 0, &errorCode);
    }

    // get the converter
    UBool isDefaultConverter;
    अगर (cnv == nullptr) {
        isDefaultConverter=true;
        cnv=u_getDefaultConverter(&errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
    } अन्यथा {
        isDefaultConverter=false;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, destCapacity, cnv, errorCode);

    // release the converter
    अगर(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    वापस len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         अक्षर *dest, int32_t destCapacity,
                         UConverter *cnv,
                         UErrorCode &errorCode) स्थिर
{
    अगर(U_FAILURE(errorCode)) {
        अगर(destCapacity!=0) {
            *dest=0;
        }
        वापस 0;
    }

    स्थिर char16_t *src=getArrayStart()+start, *srcLimit=src+length;
    अक्षर *originalDest=dest;
    स्थिर अक्षर *destLimit;

    अगर(destCapacity==0) {
        destLimit=dest=nullptr;
    } अन्यथा अगर(destCapacity==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        destLimit = static_cast<अक्षर*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        destCapacity=0x7fffffff;
    } अन्यथा {
        destLimit=dest+destCapacity;
    }

    // perform the conversion
    UErrorCode बफर_Status = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    अगर(बफर_Status==U_बफर_OVERFLOW_ERROR) {
        अक्षर बफर[1024];

        destLimit=बफर+माप(बफर);
        करो {
            dest=बफर;
            बफर_Status=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, destLimit, &src, srcLimit, nullptr, true, &बफर_Status);
            length += static_cast<int32_t>(dest - बफर);
        } जबतक(बफर_Status==U_बफर_OVERFLOW_ERROR);
    }
    अगर (U_FAILURE(बफर_Status)) {
        errorCode = बफर_Status;
    }

    वापस u_terminateChars(originalDest, destCapacity, length, &errorCode);
}

व्योम
UnicodeString::doCodepageCreate(स्थिर अक्षर *codepage_तथ्य,
                                int32_t तथ्य_लंबाई,
                                स्थिर अक्षर *codepage)
{
    // if there's nothing to convert, do nothing
    अगर (codepage_तथ्य == nullptr || तथ्य_लंबाई == 0 || तथ्य_लंबाई < -1) {
        वापस;
    }
    अगर(तथ्य_लंबाई == -1) {
        तथ्य_लंबाई = static_cast<int32_t>(uprv_strlen(codepage_तथ्य));
    }

    UErrorCode status = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    अगर (codepage == nullptr) {
        स्थिर अक्षर *defaultName = ucnv_getDefaultName();
        अगर(UCNV_FAST_IS_UTF8(defaultName)) {
            setToUTF8(StringPiece(codepage_तथ्य, तथ्य_लंबाई));
            वापस;
        }
        converter = u_getDefaultConverter(&status);
    } अन्यथा अगर (*codepage == 0) {
        // use the "invariant characters" conversion
        अगर(cloneArrayIfNeeded(तथ्य_लंबाई, तथ्य_लंबाई, false)) {
            u_charsToUChars(codepage_तथ्य, getArrayStart(), तथ्य_लंबाई);
            set_लंबाई(तथ्य_लंबाई);
        } अन्यथा {
            setToBogus();
        }
        वापस;
    } अन्यथा {
        converter = ucnv_open(codepage, &status);
    }

    // if we failed, set the appropriate flags and return
    अगर(U_FAILURE(status)) {
        setToBogus();
        वापस;
    }

    // perform the conversion
    doCodepageCreate(codepage_तथ्य, तथ्य_लंबाई, converter, status);
    अगर(U_FAILURE(status)) {
        setToBogus();
    }

    // close the converter
    अगर (codepage == nullptr) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }
}

व्योम
UnicodeString::doCodepageCreate(स्थिर अक्षर *codepage_तथ्य,
                                int32_t तथ्य_लंबाई,
                                UConverter *converter,
                                UErrorCode &status)
{
    अगर(U_FAILURE(status)) {
        वापस;
    }

    // set up the conversion parameters
    स्थिर अक्षर *mySource     = codepage_तथ्य;
    स्थिर अक्षर *mySourceEnd  = mySource + तथ्य_लंबाई;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t array_आकार;
    अगर(तथ्य_लंबाई <= US_STACKBUF_आकार) {
        // try to use the stack buffer
        array_आकार = US_STACKBUF_आकार;
    } अन्यथा {
        // 1.25 char16_t's per source byte should cover most cases
        array_आकार = तथ्य_लंबाई + (तथ्य_लंबाई >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = false;
    क्रम(;;) {
        अगर(!cloneArrayIfNeeded(array_आकार, array_आकार, doCopyArray)) {
            setToBogus();
            अवरोध;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UErrorCode बफर_Status = U_ZERO_ERROR;
        ucnv_toUnicode(converter, &myTarget,  array + getCapacity(),
            &mySource, mySourceEnd, nullptr, true, &बफर_Status);

        // update the conversion parameters
        set_लंबाई(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        अगर(बफर_Status == U_बफर_OVERFLOW_ERROR) {
            // keep the previous conversion results
            doCopyArray = true;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_आकार = static_cast<int32_t>(length() + 2 * (mySourceEnd - mySource));
        } अन्यथा {
            अगर (U_FAILURE(बफर_Status)) {
                status = बफर_Status;
            }
            अवरोध;
        }
    }
}

U_NAMESPACE_END

#पूर्ण_अगर
