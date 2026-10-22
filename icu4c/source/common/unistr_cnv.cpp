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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ustr_cnv.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Constructors
//========================================

#ఒకవేళ !U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    ఒకవేళ(codepage_దత్తాంశం != 0) {
        doCodepageCreate(codepage_దత్తాంశం, (int32_t)uprv_strlen(codepage_దత్తాంశం), 0);
    }
}

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం,
                             int32_t దత్తాంశం_నిడివి) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    ఒకవేళ(codepage_దత్తాంశం != 0) {
        doCodepageCreate(codepage_దత్తాంశం, దత్తాంశం_నిడివి, 0);
    }
}

// else see unistr.cpp
#అయితే_ముగింపు

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం,
                             స్థిరరాశి అక్షరం *codepage) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    ఒకవేళ (codepage_దత్తాంశం != శూన్యసూచిక) {
        doCodepageCreate(codepage_దత్తాంశం, static_cast<int32_t>(uprv_strlen(codepage_దత్తాంశం)), codepage);
    }
}

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం,
                             int32_t దత్తాంశం_నిడివి,
                             స్థిరరాశి అక్షరం *codepage) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    ఒకవేళ (codepage_దత్తాంశం != శూన్యసూచిక) {
        doCodepageCreate(codepage_దత్తాంశం, దత్తాంశం_నిడివి, codepage);
    }
}

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                             UConverter *cnv,
                             UError_సంకేతం &error_సంకేతం) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        // check arguments
        ఒకవేళ(src==శూన్యసూచిక) {
            // treat as an empty string, do nothing more
        } లేకపోతే ఒకవేళ(src_నిడివి<-1) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        } లేకపోతే {
            // get input length
            ఒకవేళ(src_నిడివి==-1) {
                src_నిడివి = static_cast<int32_t>(uprv_strlen(src));
            }
            ఒకవేళ(src_నిడివి>0) {
                ఒకవేళ (cnv != శూన్యసూచిక) {
                    // use the provided converter
                    ucnv_reset_గమ్యం_Unicode(cnv);
                    doCodepageCreate(src, src_నిడివి, cnv, error_సంకేతం);
                } లేకపోతే {
                    // use the default converter
                    cnv=u_getDefaultConverter(&error_సంకేతం);
                    doCodepageCreate(src, src_నిడివి, cnv, error_సంకేతం);
                    u_releaseDefaultConverter(cnv);
                }
            }
        }

        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            సమితి_గమ్యం_Bogus();
        }
    }
}

//========================================
// Codeset conversion
//========================================

#ఒకవేళ !U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ప్రారంభం,
                       int32_t length,
                       అక్షరం *లక్ష్యం,
                       uint32_t dst_కొలత) స్థిరరాశి {
    తిరిగి extract(ప్రారంభం, length, లక్ష్యం, dst_కొలత, 0);
}

// else see unistr.cpp
#అయితే_ముగింపు

int32_t
UnicodeString::extract(int32_t ప్రారంభం,
                       int32_t length,
                       అక్షరం *లక్ష్యం,
                       uint32_t dst_కొలత,
                       స్థిరరాశి అక్షరం *codepage) స్థిరరాశి
{
    // if the arguments are illegal, then do nothing
    ఒకవేళ (/*dstSize < 0 || */(dst_కొలత > 0 && లక్ష్యం == శూన్యసూచిక)) {
        తిరిగి 0;
    }

    // pin the indices to legal values
    pinIndices(ప్రారంభం, length);

    // We need to cast dstSize to int32_t for all subsequent code.
    // I don't know why the API was defined with uint32_t but we are stuck with it.
    // Also, dstSize==0xffffffff means "unlimited" but if we use target+dstSize
    // as a limit in some functions, it may wrap around and yield a pointer
    // that compares less-than target.
    int32_t capacity;
    ఒకవేళ(dst_కొలత < 0x7fffffff) {
        // Assume that the capacity is real and a limit pointer won't wrap around.
        capacity = static_cast<int32_t>(dst_కొలత);
    } లేకపోతే {
        // Pin the capacity so that a limit pointer does not wrap around.
        అక్షరం* లక్ష్యం_హద్దు = static_cast<అక్షరం*>(U_MAX_PTR(లక్ష్యం));
        // U_MAX_PTR(target) returns a targetLimit that is at most 0x7fffffff
        // greater than target and does not wrap around the top of the address space.
        capacity = static_cast<int32_t>(లక్ష్యం_హద్దు - లక్ష్యం);
    }

    // create the converter
    UConverter *converter;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    // just write the NUL if the string length is 0
    ఒకవేళ(length == 0) {
        తిరిగి u_terminateChars(లక్ష్యం, capacity, 0, &స్థితి_2);
    }

    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    ఒకవేళ (codepage == శూన్యసూచిక) {
        స్థిరరాశి అక్షరం *defaultName = ucnv_getDefaultName();
        ఒకవేళ(UCNV_FAST_IS_UTF8(defaultName)) {
            తిరిగి గమ్యం_UTF8(ప్రారంభం, length, లక్ష్యం, capacity);
        }
        converter = u_getDefaultConverter(&స్థితి_2);
    } లేకపోతే ఒకవేళ (*codepage == 0) {
        // use the "invariant characters" conversion
        int32_t dest_నిడివి;
        ఒకవేళ(length <= capacity) {
            dest_నిడివి = length;
        } లేకపోతే {
            dest_నిడివి = capacity;
        }
        u_UChars_గమ్యం_Chars(get_Array_ప్రారంభం() + ప్రారంభం, లక్ష్యం, dest_నిడివి);
        తిరిగి u_terminateChars(లక్ష్యం, capacity, length, &స్థితి_2);
    } లేకపోతే {
        converter = ucnv_open(codepage, &స్థితి_2);
    }

    length = do_వెలికితీయి(ప్రారంభం, length, లక్ష్యం, capacity, converter, స్థితి_2);

    // close the converter
    ఒకవేళ (codepage == శూన్యసూచిక) {
        u_releaseDefaultConverter(converter);
    } లేకపోతే {
        ucnv_close(converter);
    }

    తిరిగి length;
}

int32_t
UnicodeString::extract(అక్షరం *dest, int32_t dest_సామర్థ్యం,
                       UConverter *cnv,
                       UError_సంకేతం &error_సంకేతం) స్థిరరాశి
{
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ (isBogus() || dest_సామర్థ్యం < 0 || (dest_సామర్థ్యం > 0 && dest == శూన్యసూచిక)) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    // nothing to do?
    ఒకవేళ(is_ఖాళీలు()) {
        తిరిగి u_terminateChars(dest, dest_సామర్థ్యం, 0, &error_సంకేతం);
    }

    // get the converter
    UBool isDefaultConverter;
    ఒకవేళ (cnv == శూన్యసూచిక) {
        isDefaultConverter=నిజం;
        cnv=u_getDefaultConverter(&error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
    } లేకపోతే {
        isDefaultConverter=అబద్ధం;
        ucnv_resetFromUnicode(cnv);
    }

    // convert
    int32_t len=do_వెలికితీయి(0, length(), dest, dest_సామర్థ్యం, cnv, error_సంకేతం);

    // release the converter
    ఒకవేళ(isDefaultConverter) {
        u_releaseDefaultConverter(cnv);
    }

    తిరిగి len;
}

int32_t
UnicodeString::do_వెలికితీయి(int32_t ప్రారంభం, int32_t length,
                         అక్షరం *dest, int32_t dest_సామర్థ్యం,
                         UConverter *cnv,
                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి
{
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        ఒకవేళ(dest_సామర్థ్యం!=0) {
            *dest=0;
        }
        తిరిగి 0;
    }

    స్థిరరాశి char16_t *src=get_Array_ప్రారంభం()+ప్రారంభం, *src_హద్దు=src+length;
    అక్షరం *originalDest=dest;
    స్థిరరాశి అక్షరం *dest_హద్దు;

    ఒకవేళ(dest_సామర్థ్యం==0) {
        dest_హద్దు=dest=శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(dest_సామర్థ్యం==-1) {
        // Pin the limit to U_MAX_PTR if the "magic" destCapacity is used.
        dest_హద్దు = static_cast<అక్షరం*>(U_MAX_PTR(dest));
        // for NUL-termination, translate into highest int32_t
        dest_సామర్థ్యం=0x7fffffff;
    } లేకపోతే {
        dest_హద్దు=dest+dest_సామర్థ్యం;
    }

    // perform the conversion
    UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
    length = static_cast<int32_t>(dest - originalDest);

    // if an overflow occurs, then get the preflighting length
    ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR) {
        అక్షరం బఫరు[1024];

        dest_హద్దు=బఫరు+పరిమాణం(బఫరు);
        చేయి {
            dest=బఫరు;
            బఫరు_స్థితి=U_ZERO_ERROR;
            ucnv_fromUnicode(cnv, &dest, dest_హద్దు, &src, src_హద్దు, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);
            length += static_cast<int32_t>(dest - బఫరు);
        } అయితే(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR);
    }
    ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
        error_సంకేతం = బఫరు_స్థితి;
    }

    తిరిగి u_terminateChars(originalDest, dest_సామర్థ్యం, length, &error_సంకేతం);
}

శూన్య
UnicodeString::doCodepageCreate(స్థిరరాశి అక్షరం *codepage_దత్తాంశం,
                                int32_t దత్తాంశం_నిడివి,
                                స్థిరరాశి అక్షరం *codepage)
{
    // if there's nothing to convert, do nothing
    ఒకవేళ (codepage_దత్తాంశం == శూన్యసూచిక || దత్తాంశం_నిడివి == 0 || దత్తాంశం_నిడివి < -1) {
        తిరిగి;
    }
    ఒకవేళ(దత్తాంశం_నిడివి == -1) {
        దత్తాంశం_నిడివి = static_cast<int32_t>(uprv_strlen(codepage_దత్తాంశం));
    }

    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    // create the converter
    // if the codepage is the default, use our cache
    // if it is an empty string, then use the "invariant character" conversion
    UConverter *converter;
    ఒకవేళ (codepage == శూన్యసూచిక) {
        స్థిరరాశి అక్షరం *defaultName = ucnv_getDefaultName();
        ఒకవేళ(UCNV_FAST_IS_UTF8(defaultName)) {
            సమితి_గమ్యం_UTF8(StringPiece(codepage_దత్తాంశం, దత్తాంశం_నిడివి));
            తిరిగి;
        }
        converter = u_getDefaultConverter(&స్థితి_2);
    } లేకపోతే ఒకవేళ (*codepage == 0) {
        // use the "invariant characters" conversion
        ఒకవేళ(cloneArrayIfNeeded(దత్తాంశం_నిడివి, దత్తాంశం_నిడివి, అబద్ధం)) {
            u_chars_గమ్యం_UChars(codepage_దత్తాంశం, get_Array_ప్రారంభం(), దత్తాంశం_నిడివి);
            సమితి_నిడివి(దత్తాంశం_నిడివి);
        } లేకపోతే {
            సమితి_గమ్యం_Bogus();
        }
        తిరిగి;
    } లేకపోతే {
        converter = ucnv_open(codepage, &స్థితి_2);
    }

    // if we failed, set the appropriate flags and return
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        సమితి_గమ్యం_Bogus();
        తిరిగి;
    }

    // perform the conversion
    doCodepageCreate(codepage_దత్తాంశం, దత్తాంశం_నిడివి, converter, స్థితి_2);
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        సమితి_గమ్యం_Bogus();
    }

    // close the converter
    ఒకవేళ (codepage == శూన్యసూచిక) {
        u_releaseDefaultConverter(converter);
    } లేకపోతే {
        ucnv_close(converter);
    }
}

శూన్య
UnicodeString::doCodepageCreate(స్థిరరాశి అక్షరం *codepage_దత్తాంశం,
                                int32_t దత్తాంశం_నిడివి,
                                UConverter *converter,
                                UError_సంకేతం &స్థితి_2)
{
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    // set up the conversion parameters
    స్థిరరాశి అక్షరం *my_ఆధారం     = codepage_దత్తాంశం;
    స్థిరరాశి అక్షరం *my_ఆధారం_చివర  = my_ఆధారం + దత్తాంశం_నిడివి;
    char16_t *array, *my_లక్ష్యం;

    // estimate the size needed:
    int32_t array_కొలత;
    ఒకవేళ(దత్తాంశం_నిడివి <= US_STACKBUF_కొలత) {
        // try to use the stack buffer
        array_కొలత = US_STACKBUF_కొలత;
    } లేకపోతే {
        // 1.25 char16_t's per source byte should cover most cases
        array_కొలత = దత్తాంశం_నిడివి + (దత్తాంశం_నిడివి >> 2);
    }

    // we do not care about the current contents
    UBool doCopyArray = అబద్ధం;
    కొరకు(;;) {
        ఒకవేళ(!cloneArrayIfNeeded(array_కొలత, array_కొలత, doCopyArray)) {
            సమితి_గమ్యం_Bogus();
            ఆపు;
        }

        // perform the conversion
        array = get_Array_ప్రారంభం();
        my_లక్ష్యం = array + length();
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        ucnv_గమ్యం_Unicode(converter, &my_లక్ష్యం,  array + get_సామర్థ్యం(),
            &my_ఆధారం, my_ఆధారం_చివర, శూన్యసూచిక, నిజం, &బఫరు_స్థితి);

        // update the conversion parameters
        సమితి_నిడివి(static_cast<int32_t>(my_లక్ష్యం - array));

        // allocate more space and copy data, if needed
        ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR) {
            // keep the previous conversion results
            doCopyArray = నిజం;

            // estimate the new size needed, larger than before
            // try 2 char16_t's per remaining source byte
            array_కొలత = static_cast<int32_t>(length() + 2 * (my_ఆధారం_చివర - my_ఆధారం));
        } లేకపోతే {
            ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
                స్థితి_2 = బఫరు_స్థితి;
            }
            ఆపు;
        }
    }
}

U_NAMESPACE_చివర

#అయితే_ముగింపు
