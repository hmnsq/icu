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

#अन्तर्भाव <cstdlib>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : बफर(std::move(src.बफर)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::संक्रियकः=(CharString&& src) noexcept {
    बफर = std::move(src.बफर);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    प्रत्यागम *एतत्;
}

वर्ण *CharString::clone_दत्त(UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    वर्ण *p = static_cast<वर्ण *>(uprv_malloc(len + 1));
    यदि (p == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(p, बफर.getAlias(), len + 1);
    प्रत्यागम p;
}

int32_t CharString::extract(वर्ण *dest, int32_t capacity, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम len; }
    यदि (capacity < 0 || (capacity > 0 && dest == शून्यसूचकः)) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम len;
    }
    ध्रुव वर्ण *src = बफर.getAlias();
    यदि (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    प्रत्यागम u_terminateChars(dest, capacity, len, &error_संहिता);
}

CharString &CharString::copyFrom(ध्रुव CharString &s, UError_संहिता &error_संहिता) {
    यदि(U_SUCCESS(error_संहिता) && एतत्!=&s && ensure_क्षमता(s.len+1, 0, error_संहिता)) {
        len=s.len;
        uprv_memcpy(बफर.getAlias(), s.बफर.getAlias(), len+1);
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::copyFrom(StringPiece s, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    len = 0;
    append(s, error_संहिता);
    प्रत्यागम *एतत्;
}

int32_t CharString::चरमः_सूचकांक_सम्बन्धि(वर्ण c) ध्रुव {
    प्रति(int32_t i=len; i>0;) {
        यदि(बफर[--i]==c) {
            प्रत्यागम i;
        }
    }
    प्रत्यागम -1;
}

तार्किक CharString::अन्तर्भवति(StringPiece s) ध्रुव {
    यदि (s.empty()) { प्रत्यागम असत्य; }
    ध्रुव वर्ण *p = बफर.getAlias();
    int32_t चरमः_Start = len - s.length();
    प्रति (int32_t i = 0; i <= चरमः_Start; ++i) {
        यदि (uprv_memcmp(p + i, s.दत्त(), s.length()) == 0) {
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

CharString &CharString::truncate(int32_t new_दीर्घता) {
    यदि(new_दीर्घता<0) {
        new_दीर्घता=0;
    }
    यदि(new_दीर्घता<len) {
        बफर[len=new_दीर्घता]=0;
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append(वर्ण c, UError_संहिता &error_संहिता) {
    यदि(ensure_क्षमता(len+2, 0, error_संहिता)) {
        बफर[len++]=c;
        बफर[len]=0;
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append(ध्रुव वर्ण *s, int32_t s_दीर्घता, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    यदि(s_दीर्घता<-1 || (s==शून्यसूचकः && s_दीर्घता!=0)) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि(s_दीर्घता<0) {
        s_दीर्घता= static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(s_दीर्घता>0) {
        यदि(s==(बफर.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            यदि(s_दीर्घता>=(बफर.get_क्षमता()-len)) {
                // The caller wrote too much.
                error_संहिता=U_आन्तरिकम्_क्रमादेशः_ERROR;
            } अन्यथा {
                बफर[len+=s_दीर्घता]=0;
            }
        } अन्यथा यदि(बफर.getAlias()<=s && s<(बफर.getAlias()+len) &&
                  s_दीर्घता>=(बफर.get_क्षमता()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            प्रत्यागम append(CharString(s, s_दीर्घता, error_संहिता), error_संहिता);
        } अन्यथा यदि(ensure_क्षमता(len+s_दीर्घता+1, 0, error_संहिता)) {
            uprv_memcpy(बफर.getAlias()+len, s, s_दीर्घता);
            बफर[len+=s_दीर्घता]=0;
        }
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append_सङ्ख्या(int64_t number, UError_संहिता &स्थितिः) {
    यदि (number < 0) {
        एतत्->append('-', स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम *एतत्;
        }
    }

    यदि (number == 0) {
        एतत्->append('0', स्थितिः);
        प्रत्यागम *एतत्;
    }

    int32_t अङ्कः_Len = 0;
    यावत् (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        एतत्->append(std::निरपेक्षम्(residue) + '0', स्थितिः);
        अङ्कः_Len++;
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम *एतत्;
        }
    }

    int32_t start = एतत्->length() - अङ्कः_Len, अन्तः = एतत्->length() - 1;
    यावत्(start < अन्तः) {
        std::swap(एतत्->दत्त()[start++], एतत्->दत्त()[अन्तः--]);
    }

    प्रत्यागम *एतत्;
}

वर्ण *CharString::get_Append_बफर(int32_t min_क्षमता,
                                  int32_t desired_क्षमता_Hint,
                                  int32_t &परिणाम_क्षमता,
                                  UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        परिणाम_क्षमता=0;
        प्रत्यागम शून्यसूचकः;
    }
    int32_t append_क्षमता=बफर.get_क्षमता()-len-1;  // -1 for NUL
    यदि(append_क्षमता>=min_क्षमता) {
        परिणाम_क्षमता=append_क्षमता;
        प्रत्यागम बफर.getAlias()+len;
    }
    यदि(ensure_क्षमता(len+min_क्षमता+1, len+desired_क्षमता_Hint+1, error_संहिता)) {
        परिणाम_क्षमता=बफर.get_क्षमता()-len-1;
        प्रत्यागम बफर.getAlias()+len;
    }
    परिणाम_क्षमता=0;
    प्रत्यागम शून्यसूचकः;
}

CharString &CharString::appendInvariantChars(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) {
    प्रत्यागम appendInvariantChars(s.get_बफर(), s.length(), error_संहिता);
}

CharString &CharString::appendInvariantChars(ध्रुव char16_t* uchars, int32_t ucharsLen, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    यदि (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_संहिता = U_INVARIANT_CONVERSION_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि(ensure_क्षमता(len+ucharsLen+1, 0, error_संहिता)) {
        u_UChars_गन्तव्यम्_Chars(uchars, बफर.getAlias()+len, ucharsLen);
        len += ucharsLen;
        बफर[len] = 0;
    }
    प्रत्यागम *एतत्;
}

UBool CharString::ensure_क्षमता(int32_t capacity,
                                 int32_t desired_क्षमता_Hint,
                                 UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    यदि(capacity>बफर.get_क्षमता()) {
        यदि(desired_क्षमता_Hint==0) {
            desired_क्षमता_Hint=capacity+बफर.get_क्षमता();
        }
        यदि( (desired_क्षमता_Hint<=capacity || बफर.resize(desired_क्षमता_Hint, len+1)==शून्यसूचकः) &&
            बफर.resize(capacity, len+1)==शून्यसूचकः
        ) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

CharString &CharString::append_मार्गः_Part(StringPiece s, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    यदि(s.length()==0) {
        प्रत्यागम *एतत्;
    }
    वर्ण c;
    यदि(len>0 && (c=बफर[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_संहिता);
    }
    append(s, error_संहिता);
    प्रत्यागम *एतत्;
}

CharString &CharString::ensure_अन्ताः_With_File_Separator(UError_संहिता &error_संहिता) {
    वर्ण c;
    यदि(U_SUCCESS(error_संहिता) && len>0 &&
            (c=बफर[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_संहिता);
    }
    प्रत्यागम *एतत्;
}

वर्ण CharString::getDirSepChar() ध्रुव {
    वर्ण dirSepChar = U_FILE_SEP_CHAR;
#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    यदि(len>0 && !uprv_strchr(दत्त(), U_FILE_SEP_CHAR) && uprv_strchr(दत्त(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#यदि_अन्तः
    प्रत्यागम dirSepChar;
}

U_NAMESPACE_अन्तः
