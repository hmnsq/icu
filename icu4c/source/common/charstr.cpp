// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  charstr.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010may19
*   created by: Markus W. Scherer
*/

#అన్తర్భావ <cstdlib>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : బఫరు(std::move(src.బఫరు)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::క్రియాకారకం=(CharString&& src) noexcept {
    బఫరు = std::move(src.బఫరు);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    తిరిగి *ఇది;
}

అక్షరం *CharString::clone_దత్తాంశం(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    అక్షరం *p = static_cast<అక్షరం *>(uprv_malloc(len + 1));
    ఒకవేళ (p == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(p, బఫరు.getAlias(), len + 1);
    తిరిగి p;
}

int32_t CharString::extract(అక్షరం *dest, int32_t capacity, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి len; }
    ఒకవేళ (capacity < 0 || (capacity > 0 && dest == శూన్యసూచిక)) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి len;
    }
    స్థిరరాశి అక్షరం *src = బఫరు.getAlias();
    ఒకవేళ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    తిరిగి u_terminateChars(dest, capacity, len, &error_సంకేతం);
}

CharString &CharString::copyFrom(స్థిరరాశి CharString &s, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && ఇది!=&s && ensure_సామర్థ్యం(s.len+1, 0, error_సంకేతం)) {
        len=s.len;
        uprv_memcpy(బఫరు.getAlias(), s.బఫరు.getAlias(), len+1);
    }
    తిరిగి *ఇది;
}

CharString &CharString::copyFrom(StringPiece s, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    len = 0;
    append(s, error_సంకేతం);
    తిరిగి *ఇది;
}

int32_t CharString::చివరి_సూచీ_యొక్క(అక్షరం c) స్థిరరాశి {
    కొరకు(int32_t i=len; i>0;) {
        ఒకవేళ(బఫరు[--i]==c) {
            తిరిగి i;
        }
    }
    తిరిగి -1;
}

తార్కిక CharString::కలిగియుంది(StringPiece s) స్థిరరాశి {
    ఒకవేళ (s.empty()) { తిరిగి అబద్ధం; }
    స్థిరరాశి అక్షరం *p = బఫరు.getAlias();
    int32_t చివరి_ప్రారంభం = len - s.length();
    కొరకు (int32_t i = 0; i <= చివరి_ప్రారంభం; ++i) {
        ఒకవేళ (uprv_memcmp(p + i, s.దత్తాంశం(), s.length()) == 0) {
            తిరిగి నిజం;
        }
    }
    తిరిగి అబద్ధం;
}

CharString &CharString::truncate(int32_t new_నిడివి) {
    ఒకవేళ(new_నిడివి<0) {
        new_నిడివి=0;
    }
    ఒకవేళ(new_నిడివి<len) {
        బఫరు[len=new_నిడివి]=0;
    }
    తిరిగి *ఇది;
}

CharString &CharString::append(అక్షరం c, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(ensure_సామర్థ్యం(len+2, 0, error_సంకేతం)) {
        బఫరు[len++]=c;
        బఫరు[len]=0;
    }
    తిరిగి *ఇది;
}

CharString &CharString::append(స్థిరరాశి అక్షరం *s, int32_t s_నిడివి, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ(s_నిడివి<-1 || (s==శూన్యసూచిక && s_నిడివి!=0)) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    ఒకవేళ(s_నిడివి<0) {
        s_నిడివి= static_cast<int32_t>(uprv_strlen(s));
    }
    ఒకవేళ(s_నిడివి>0) {
        ఒకవేళ(s==(బఫరు.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ఒకవేళ(s_నిడివి>=(బఫరు.get_సామర్థ్యం()-len)) {
                // The caller wrote too much.
                error_సంకేతం=U_అంతర్గత_కార్యక్రమం_ERROR;
            } లేకపోతే {
                బఫరు[len+=s_నిడివి]=0;
            }
        } లేకపోతే ఒకవేళ(బఫరు.getAlias()<=s && s<(బఫరు.getAlias()+len) &&
                  s_నిడివి>=(బఫరు.get_సామర్థ్యం()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            తిరిగి append(CharString(s, s_నిడివి, error_సంకేతం), error_సంకేతం);
        } లేకపోతే ఒకవేళ(ensure_సామర్థ్యం(len+s_నిడివి+1, 0, error_సంకేతం)) {
            uprv_memcpy(బఫరు.getAlias()+len, s, s_నిడివి);
            బఫరు[len+=s_నిడివి]=0;
        }
    }
    తిరిగి *ఇది;
}

CharString &CharString::append_సంఖ్య(int64_t number, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (number < 0) {
        ఇది->append('-', స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి *ఇది;
        }
    }

    ఒకవేళ (number == 0) {
        ఇది->append('0', స్థితి_2);
        తిరిగి *ఇది;
    }

    int32_t అంకె_Len = 0;
    అయితే (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ఇది->append(std::నిరపేక్ష(residue) + '0', స్థితి_2);
        అంకె_Len++;
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి *ఇది;
        }
    }

    int32_t ప్రారంభం = ఇది->length() - అంకె_Len, చివర = ఇది->length() - 1;
    అయితే(ప్రారంభం < చివర) {
        std::swap(ఇది->దత్తాంశం()[ప్రారంభం++], ఇది->దత్తాంశం()[చివర--]);
    }

    తిరిగి *ఇది;
}

అక్షరం *CharString::get_Append_బఫరు(int32_t min_సామర్థ్యం,
                                  int32_t desired_సామర్థ్యం_Hint,
                                  int32_t &ఫలితం_సామర్థ్యం,
                                  UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        ఫలితం_సామర్థ్యం=0;
        తిరిగి శూన్యసూచిక;
    }
    int32_t append_సామర్థ్యం=బఫరు.get_సామర్థ్యం()-len-1;  // -1 for NUL
    ఒకవేళ(append_సామర్థ్యం>=min_సామర్థ్యం) {
        ఫలితం_సామర్థ్యం=append_సామర్థ్యం;
        తిరిగి బఫరు.getAlias()+len;
    }
    ఒకవేళ(ensure_సామర్థ్యం(len+min_సామర్థ్యం+1, len+desired_సామర్థ్యం_Hint+1, error_సంకేతం)) {
        ఫలితం_సామర్థ్యం=బఫరు.get_సామర్థ్యం()-len-1;
        తిరిగి బఫరు.getAlias()+len;
    }
    ఫలితం_సామర్థ్యం=0;
    తిరిగి శూన్యసూచిక;
}

CharString &CharString::appendInvariantChars(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) {
    తిరిగి appendInvariantChars(s.get_బఫరు(), s.length(), error_సంకేతం);
}

CharString &CharString::appendInvariantChars(స్థిరరాశి char16_t* uchars, int32_t ucharsLen, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_సంకేతం = U_INVARIANT_CONVERSION_ERROR;
        తిరిగి *ఇది;
    }
    ఒకవేళ(ensure_సామర్థ్యం(len+ucharsLen+1, 0, error_సంకేతం)) {
        u_UChars_గమ్యం_Chars(uchars, బఫరు.getAlias()+len, ucharsLen);
        len += ucharsLen;
        బఫరు[len] = 0;
    }
    తిరిగి *ఇది;
}

UBool CharString::ensure_సామర్థ్యం(int32_t capacity,
                                 int32_t desired_సామర్థ్యం_Hint,
                                 UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(capacity>బఫరు.get_సామర్థ్యం()) {
        ఒకవేళ(desired_సామర్థ్యం_Hint==0) {
            desired_సామర్థ్యం_Hint=capacity+బఫరు.get_సామర్థ్యం();
        }
        ఒకవేళ( (desired_సామర్థ్యం_Hint<=capacity || బఫరు.resize(desired_సామర్థ్యం_Hint, len+1)==శూన్యసూచిక) &&
            బఫరు.resize(capacity, len+1)==శూన్యసూచిక
        ) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

CharString &CharString::append_మార్గం_Part(StringPiece s, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ(s.length()==0) {
        తిరిగి *ఇది;
    }
    అక్షరం c;
    ఒకవేళ(len>0 && (c=బఫరు[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_సంకేతం);
    }
    append(s, error_సంకేతం);
    తిరిగి *ఇది;
}

CharString &CharString::ensure_చివర్లు_With_File_Separator(UError_సంకేతం &error_సంకేతం) {
    అక్షరం c;
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && len>0 &&
            (c=బఫరు[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_సంకేతం);
    }
    తిరిగి *ఇది;
}

అక్షరం CharString::getDirSepChar() స్థిరరాశి {
    అక్షరం dirSepChar = U_FILE_SEP_CHAR;
#ఒకవేళ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ఒకవేళ(len>0 && !uprv_strchr(దత్తాంశం(), U_FILE_SEP_CHAR) && uprv_strchr(దత్తాంశం(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#అయితే_ముగింపు
    తిరిగి dirSepChar;
}

U_NAMESPACE_చివర
