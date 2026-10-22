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

#यदि !UCONFIG_NO_CONVERSION

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

#यदि !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    यदि(codepage_दत्त != 0) {
        doCodepageCreate(codepage_दत्त, (int32_t)uprv_strlen(codepage_दत्त), 0);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त,
                             int32_t दत्त_दीर्घता) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    यदि(codepage_दत्त != 0) {
        doCodepageCreate(codepage_दत्त, दत्त_दीर्घता, 0);
    }
}

// else see unistr.cpp
#यदि_अन्तः

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त,
                             ध्रुव वर्ण *codepage) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    यदि (codepage_दत्त != शून्यसूचकः) {
        doCodepageCreate(codepage_दत्त, static_cast<int32_t>(uprv_strlen(codepage_दत्त)), codepage);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त,
                             int32_t दत्त_दीर्घता,
                             ध्रुव वर्ण *codepage) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    यदि (codepage_दत्त != शून्यसूचकः) {
        doCodepageCreate(codepage_दत्त, दत्त_दीर्घता, codepage);
    }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *src, int32_t src_दीर्घता,
                             UConverter *cnv,
                             UError_संहिता &error_संहिता) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    यदि(U_SUCCESS(error_संहिता)) {
        // check arguments
        यदि(src==शून्यसूचकः) {
            // treat as an empty string, do nothing more
        } अन्यथा यदि(src_दीर्घता<-1) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        } अन्यथा {
            // get input length
            यदि(src_दीर्घता==-1) {
                src_दीर्घता = static_cast<int32_t>(uprv_strlen(src));
            }
            यदि(src_दीर्घता>0) {
                यदि (cnv != शून्यसूचकः) {
                    // use the provided converter
                    ucnv_reset_गन्तव्यम्_Unicode(cnv);
                    doCodepageCreate(src, src_दीर्घता, cnv, error_संहिता);
                } अन्यथा {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_संहिता);
                    doCodepageCreate(src, src_दीर्घता, cnv, error_संहिता);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        यदि(U_FAILURE(error_संहिता)) {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#यदि !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       uint32_t dst_आकार) ध्रुव {
    प्रत्यागम extract(start, length, target, dst_आकार, 0);
}

// else see unistr.cpp
#यदि_अन्तः

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       uint32_t dst_आकार,
                       ध्रुव वर्ण *codepage) ध्रुव
{
    // if the arguments are illegal, then do nothing
    यदि (/*dstSize < 0 || */(dst_आकार > 0 && target == शून्यसूचकः)) {
        प्रत्यागम 0;
    }

    // pin the indices to legal values
    pinIndices(start, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    यदि(dst_आकार < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_आकार);
    } अन्यथा {
        // Pin the capacity so that a limit pointer does not wrap around.
        वर्ण* target_सीमा = static_cast<वर्ण*>(U_MAX_PTR(target));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(target_सीमा - target);
    }

    // create the converter
    UConverter *converter;
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    यदि(length == 0) {
        प्रत्यागम u_terminateChars(target, capacity, 0, &स्थितिः);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    यदि (codepage == शून्यसूचकः) {
        ध्रुव वर्ण *defaultName = ucnv_getDefaultName();
        यदि(UCNV_FAST_IS_UTF8(defaultName)) {
            प्रत्यागम गन्तव्यम्_UTF8(start, length, target, capacity);
        }
        converter = u_getDefaultConverter(&स्थितिः);
    } अन्यथा यदि (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_दीर्घता;
        यदि(length <= capacity) {
            dest_दीर्घता = length;
        } अन्यथा {
            dest_दीर्घता = capacity;
        }
        u_UChars_गन्तव्यम्_Chars(getArrayStart() + start, target, dest_दीर्घता);
        प्रत्यागम u_terminateChars(target, capacity, length, &स्थितिः);
    } अन्यथा {
        converter = ucnv_open(codepage, &स्थितिः);
    }

    length = doExtract(start, length, target, capacity, converter, स्थितिः);

    // close the converter
    यदि (codepage == शून्यसूचकः) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }

    प्रत्यागम length;
}

int32_t
UnicodeString::extract(वर्ण *dest, int32_t dest_क्षमता,
                       UConverter *cnv,
                       UError_संहिता &error_संहिता) ध्रुव
{
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि (isBogus() || dest_क्षमता < 0 || (dest_क्षमता > 0 && dest == शून्यसूचकः)) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    // nothing to do?
    यदि(is_रिक्तानि()) {
        प्रत्यागम u_terminateChars(dest, dest_क्षमता, 0, &error_संहिता);
    }

    // get the converter
    UBool isDefaultConverter;
    यदि (cnv == शून्यसूचकः) {
        isDefaultConverter=सत्य;
        cnv=u_getDefaultConverter(&error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
    } अन्यथा {
        isDefaultConverter=असत्य;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=doExtract(0, length(), dest, dest_क्षमता, cnv, error_संहिता);

    // release the converter
    यदि(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    प्रत्यागम len;
}

int32_t
UnicodeString::doExtract(int32_t start, int32_t length,
                         वर्ण *dest, int32_t dest_क्षमता,
                         UConverter *cnv,
                         UError_संहिता &error_संहिता) ध्रुव
{
    यदि(U_FAILURE(error_संहिता)) {
        यदि(dest_क्षमता!=0) {
            *dest=0;
        }
        प्रत्यागम 0;
    }

    ध्रुव char16_t *src=getArrayStart()+start, *src_सीमा=src+length;
    वर्ण *originalDest=dest;
    ध्रुव वर्ण *dest_सीमा;

    यदि(dest_क्षमता==0) {
        dest_सीमा=dest=शून्यसूचकः;
    } अन्यथा यदि(dest_क्षमता==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_सीमा = static_cast<वर्ण*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_क्षमता=0x7fffffff;
    } अन्यथा {
        dest_सीमा=dest+dest_क्षमता;
    }

    // perform the conversion
    UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR) {
        वर्ण बफर[1024];

        dest_सीमा=बफर+परिमाण(बफर);
        कुरु {
            dest=बफर;
            बफर_स्थितिः=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_सीमा, &src, src_सीमा, शून्यसूचकः, सत्य, &बफर_स्थितिः);
            length += static_cast<int32_t>(dest - बफर);
        } यावत्(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR);
    }
    यदि (U_FAILURE(बफर_स्थितिः)) {
        error_संहिता = बफर_स्थितिः;
    }

    प्रत्यागम u_terminateChars(originalDest, dest_क्षमता, length, &error_संहिता);
}

शून्य
UnicodeString::doCodepageCreate(ध्रुव वर्ण *codepage_दत्त,
                                int32_t दत्त_दीर्घता,
                                ध्रुव वर्ण *codepage)
{
    // if there's nothing to convert, do nothing
    यदि (codepage_दत्त == शून्यसूचकः || दत्त_दीर्घता == 0 || दत्त_दीर्घता < -1) {
        प्रत्यागम;
    }
    यदि(दत्त_दीर्घता == -1) {
        दत्त_दीर्घता = static_cast<int32_t>(uprv_strlen(codepage_दत्त));
    }

    UError_संहिता स्थितिः = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    यदि (codepage == शून्यसूचकः) {
        ध्रुव वर्ण *defaultName = ucnv_getDefaultName();
        यदि(UCNV_FAST_IS_UTF8(defaultName)) {
            समुच्चयः_गन्तव्यम्_UTF8(StringPiece(codepage_दत्त, दत्त_दीर्घता));
            प्रत्यागम;
        }
        converter = u_getDefaultConverter(&स्थितिः);
    } अन्यथा यदि (*codepage == 0) {
        // use the "invariant characters" conversion
        यदि(cloneArrayIfNeeded(दत्त_दीर्घता, दत्त_दीर्घता, असत्य)) {
            u_chars_गन्तव्यम्_UChars(codepage_दत्त, getArrayStart(), दत्त_दीर्घता);
            समुच्चयः_दीर्घता(दत्त_दीर्घता);
        } अन्यथा {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
        प्रत्यागम;
    } अन्यथा {
        converter = ucnv_open(codepage, &स्थितिः);
    }

    // if we failed, set the appropriate flags and return
    यदि(U_FAILURE(स्थितिः)) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम;
    }

    // perform the conversion
    doCodepageCreate(codepage_दत्त, दत्त_दीर्घता, converter, स्थितिः);
    यदि(U_FAILURE(स्थितिः)) {
        समुच्चयः_गन्तव्यम्_Bogus();
    }

    // close the converter
    यदि (codepage == शून्यसूचकः) {
        u_releaseDefaultConverter(converter);
    } अन्यथा {
        ucnv_close(converter);
    }
}

शून्य
UnicodeString::doCodepageCreate(ध्रुव वर्ण *codepage_दत्त,
                                int32_t दत्त_दीर्घता,
                                UConverter *converter,
                                UError_संहिता &स्थितिः)
{
    यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    // set up the conversion parameters
    ध्रुव वर्ण *mySource     = codepage_दत्त;
    ध्रुव वर्ण *my_Source_अन्तः  = mySource + दत्त_दीर्घता;
    char16_t *array, *myTarget;

    // estimate the size needed:
    int32_t array_आकार;
    यदि(दत्त_दीर्घता <= US_STACKBUF_आकार) {
        // try to use the stack buffer
        array_आकार = US_STACKBUF_आकार;
    } अन्यथा {
        // 1.25 char16_t's per source byte should cover most cases
        array_आकार = दत्त_दीर्घता + (दत्त_दीर्घता >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = असत्य;
    प्रति(;;) {
        यदि(!cloneArrayIfNeeded(array_आकार, array_आकार, doCopyArray)) {
            समुच्चयः_गन्तव्यम्_Bogus();
            भङ्ग;
        }

        // perform the conversion
        array = getArrayStart();
        myTarget = array + length();
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        ucnv_गन्तव्यम्_Unicode(converter, &myTarget,  array + get_क्षमता(),
            &mySource, my_Source_अन्तः, शून्यसूचकः, सत्य, &बफर_स्थितिः);

        // update the conversion parameters
        समुच्चयः_दीर्घता(static_cast<int32_t>(myTarget - array));

        // allocate more space and copy data, if needed
        यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR) {
            // keep the previous conversion results
            doCopyArray = सत्य;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_आकार = static_cast<int32_t>(length() + 2 * (my_Source_अन्तः - mySource));
        } अन्यथा {
            यदि (U_FAILURE(बफर_स्थितिः)) {
                स्थितिः = बफर_स्थितिः;
            }
            भङ्ग;
        }
    }
}

U_NAMESPACE_अन्तः

#यदि_अन्तः
