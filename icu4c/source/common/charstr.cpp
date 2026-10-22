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
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    प्रत्यागम *एतत्;
}

वर्ण *CharString::cloneData(UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    वर्ण *p = static_cast<वर्ण *>(uprv_malloc(len + 1));
    जइ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    प्रत्यागम p;
}

int32_t CharString::extract(वर्ण *dest, int32_t capacity, UErrorCode &errorCode) ध्रुव {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम len; }
    जइ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम len;
    }
    ध्रुव वर्ण *src = buffer.getAlias();
    जइ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    प्रत्यागम u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ध्रुव CharString &s, UErrorCode &errorCode) {
    जइ(U_SUCCESS(errorCode) && एतत्!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) {
        प्रत्यागम *एतत्;
    }
    len = 0;
    append(s, errorCode);
    प्रत्यागम *एतत्;
}

int32_t CharString::lastIndexOf(वर्ण c) ध्रुव {
    प्रति(int32_t i=len; i>0;) {
        जइ(buffer[--i]==c) {
            प्रत्यागम i;
        }
    }
    प्रत्यागम -1;
}

तार्किक CharString::contains(StringPiece s) ध्रुव {
    जइ (s.empty()) { प्रत्यागम मिच्छ; }
    ध्रुव वर्ण *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    प्रति (int32_t i = 0; i <= lastStart; ++i) {
        जइ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            प्रत्यागम सच्च;
        }
    }
    प्रत्यागम मिच्छ;
}

CharString &CharString::truncate(int32_t newLength) {
    जइ(newLength<0) {
        newLength=0;
    }
    जइ(newLength<len) {
        buffer[len=newLength]=0;
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append(वर्ण c, UErrorCode &errorCode) {
    जइ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append(ध्रुव वर्ण *s, int32_t sLength, UErrorCode &errorCode) {
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम *एतत्;
    }
    जइ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    जइ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    जइ(sLength>0) {
        जइ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            जइ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } अण्णहा {
                buffer[len+=sLength]=0;
            }
        } अण्णहा जइ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            प्रत्यागम append(CharString(s, sLength, errorCode), errorCode);
        } अण्णहा जइ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    प्रत्यागम *एतत्;
}

CharString &CharString::append_सङ्ख्या(int64_t number, UErrorCode &status) {
    जइ (number < 0) {
        एतत्->append('-', status);
        जइ (U_FAILURE(status)) {
            प्रत्यागम *एतत्;
        }
    }

    जइ (number == 0) {
        एतत्->append('0', status);
        प्रत्यागम *एतत्;
    }

    int32_t अङ्कः_Len = 0;
    जाव (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        एतत्->append(std::abs(residue) + '0', status);
        अङ्कः_Len++;
        जइ (U_FAILURE(status)) {
            प्रत्यागम *एतत्;
        }
    }

    int32_t start = एतत्->length() - अङ्कः_Len, end = एतत्->length() - 1;
    जाव(start < end) {
        std::swap(एतत्->data()[start++], एतत्->data()[end--]);
    }

    प्रत्यागम *एतत्;
}

वर्ण *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    जइ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        प्रत्यागम nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    जइ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        प्रत्यागम buffer.getAlias()+len;
    }
    जइ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        प्रत्यागम buffer.getAlias()+len;
    }
    resultCapacity=0;
    प्रत्यागम nullptr;
}

CharString &CharString::appendInvariantChars(ध्रुव UnicodeString &s, UErrorCode &errorCode) {
    प्रत्यागम appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ध्रुव char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम *एतत्;
    }
    जइ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        प्रत्यागम *एतत्;
    }
    जइ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    प्रत्यागम *एतत्;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम मिच्छ;
    }
    जइ(capacity>buffer.getCapacity()) {
        जइ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        जइ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम सच्च;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम *एतत्;
    }
    जइ(s.length()==0) {
        प्रत्यागम *एतत्;
    }
    वर्ण c;
    जइ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    प्रत्यागम *एतत्;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    वर्ण c;
    जइ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    प्रत्यागम *एतत्;
}

वर्ण CharString::getDirSepChar() ध्रुव {
    वर्ण dirSepChar = U_FILE_SEP_CHAR;
#जइ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    जइ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    प्रत्यागम dirSepChar;
}

U_NAMESPACE_END
