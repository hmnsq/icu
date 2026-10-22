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

#समावेश <cstdlib>

#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uinvchar.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : बफर(std::move(src.बफर)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::चालक=(CharString&& src) noexcept {
    बफर = std::move(src.बफर);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    वापस *यह;
}

अक्षर *CharString::clone_तथ्य(UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    अक्षर *p = static_cast<अक्षर *>(uprv_malloc(len + 1));
    अगर (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(p, बफर.getAlias(), len + 1);
    वापस p;
}

int32_t CharString::extract(अक्षर *dest, int32_t capacity, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस len; }
    अगर (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस len;
    }
    स्थिर अक्षर *src = बफर.getAlias();
    अगर (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    वापस u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(स्थिर CharString &s, UErrorCode &errorCode) {
    अगर(U_SUCCESS(errorCode) && यह!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(बफर.getAlias(), s.बफर.getAlias(), len+1);
    }
    वापस *यह;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस *यह;
    }
    len = 0;
    append(s, errorCode);
    वापस *यह;
}

int32_t CharString::last_सूचकांक_Of(अक्षर c) स्थिर {
    क्रम(int32_t i=len; i>0;) {
        अगर(बफर[--i]==c) {
            वापस i;
        }
    }
    वापस -1;
}

bool CharString::contains(StringPiece s) स्थिर {
    अगर (s.empty()) { वापस false; }
    स्थिर अक्षर *p = बफर.getAlias();
    int32_t lastStart = len - s.length();
    क्रम (int32_t i = 0; i <= lastStart; ++i) {
        अगर (uprv_memcmp(p + i, s.तथ्य(), s.length()) == 0) {
            वापस true;
        }
    }
    वापस false;
}

CharString &CharString::truncate(int32_t new_लंबाई) {
    अगर(new_लंबाई<0) {
        new_लंबाई=0;
    }
    अगर(new_लंबाई<len) {
        बफर[len=new_लंबाई]=0;
    }
    वापस *यह;
}

CharString &CharString::append(अक्षर c, UErrorCode &errorCode) {
    अगर(ensureCapacity(len+2, 0, errorCode)) {
        बफर[len++]=c;
        बफर[len]=0;
    }
    वापस *यह;
}

CharString &CharString::append(स्थिर अक्षर *s, int32_t s_लंबाई, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस *यह;
    }
    अगर(s_लंबाई<-1 || (s==nullptr && s_लंबाई!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    अगर(s_लंबाई<0) {
        s_लंबाई= static_cast<int32_t>(uprv_strlen(s));
    }
    अगर(s_लंबाई>0) {
        अगर(s==(बफर.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            अगर(s_लंबाई>=(बफर.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } अन्यथा {
                बफर[len+=s_लंबाई]=0;
            }
        } अन्यथा अगर(बफर.getAlias()<=s && s<(बफर.getAlias()+len) &&
                  s_लंबाई>=(बफर.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            वापस append(CharString(s, s_लंबाई, errorCode), errorCode);
        } अन्यथा अगर(ensureCapacity(len+s_लंबाई+1, 0, errorCode)) {
            uprv_memcpy(बफर.getAlias()+len, s, s_लंबाई);
            बफर[len+=s_लंबाई]=0;
        }
    }
    वापस *यह;
}

CharString &CharString::append_संख्या(int64_t number, UErrorCode &status) {
    अगर (number < 0) {
        यह->append('-', status);
        अगर (U_FAILURE(status)) {
            वापस *यह;
        }
    }

    अगर (number == 0) {
        यह->append('0', status);
        वापस *यह;
    }

    int32_t अंक_Len = 0;
    जबतक (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        यह->append(std::असल(residue) + '0', status);
        अंक_Len++;
        अगर (U_FAILURE(status)) {
            वापस *यह;
        }
    }

    int32_t start = यह->length() - अंक_Len, end = यह->length() - 1;
    जबतक(start < end) {
        std::swap(यह->तथ्य()[start++], यह->तथ्य()[end--]);
    }

    वापस *यह;
}

अक्षर *CharString::get_Append_बफर(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &परिणाम_Capacity,
                                  UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        परिणाम_Capacity=0;
        वापस nullptr;
    }
    int32_t appendCapacity=बफर.getCapacity()-len-1;  // -1 for NUL
    अगर(appendCapacity>=minCapacity) {
        परिणाम_Capacity=appendCapacity;
        वापस बफर.getAlias()+len;
    }
    अगर(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        परिणाम_Capacity=बफर.getCapacity()-len-1;
        वापस बफर.getAlias()+len;
    }
    परिणाम_Capacity=0;
    वापस nullptr;
}

CharString &CharString::appendInvariantChars(स्थिर UnicodeString &s, UErrorCode &errorCode) {
    वापस appendInvariantChars(s.get_बफर(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(स्थिर char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस *यह;
    }
    अगर (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        वापस *यह;
    }
    अगर(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, बफर.getAlias()+len, ucharsLen);
        len += ucharsLen;
        बफर[len] = 0;
    }
    वापस *यह;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    अगर(capacity>बफर.getCapacity()) {
        अगर(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+बफर.getCapacity();
        }
        अगर( (desiredCapacityHint<=capacity || बफर.resize(desiredCapacityHint, len+1)==nullptr) &&
            बफर.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }
    }
    वापस true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस *यह;
    }
    अगर(s.length()==0) {
        वापस *यह;
    }
    अक्षर c;
    अगर(len>0 && (c=बफर[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    वापस *यह;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    अक्षर c;
    अगर(U_SUCCESS(errorCode) && len>0 &&
            (c=बफर[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    वापस *यह;
}

अक्षर CharString::getDirSepChar() स्थिर {
    अक्षर dirSepChar = U_FILE_SEP_CHAR;
#अगर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    अगर(len>0 && !uprv_strchr(तथ्य(), U_FILE_SEP_CHAR) && uprv_strchr(तथ्य(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#पूर्ण_अगर
    वापस dirSepChar;
}

U_NAMESPACE_END
