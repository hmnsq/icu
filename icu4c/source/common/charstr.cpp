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

CharString& CharString::चालक=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    फर्काऊ *एतत्;
}

वर्ण *CharString::cloneData(UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    वर्ण *p = static_cast<वर्ण *>(uprv_malloc(len + 1));
    यदि (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    फर्काऊ p;
}

int32_t CharString::extract(वर्ण *dest, int32_t capacity, UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode)) { फर्काऊ len; }
    यदि (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ len;
    }
    अचर वर्ण *src = buffer.getAlias();
    यदि (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    फर्काऊ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(अचर CharString &s, UErrorCode &errorCode) {
    यदि(U_SUCCESS(errorCode) && एतत्!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    फर्काऊ *एतत्;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    len = 0;
    append(s, errorCode);
    फर्काऊ *एतत्;
}

int32_t CharString::lastIndexOf(वर्ण c) अचर {
    लागि(int32_t i=len; i>0;) {
        यदि(buffer[--i]==c) {
            फर्काऊ i;
        }
    }
    फर्काऊ -1;
}

तार्किक CharString::contains(StringPiece s) अचर {
    यदि (s.empty()) { फर्काऊ असत्य; }
    अचर वर्ण *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    लागि (int32_t i = 0; i <= lastStart; ++i) {
        यदि (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            फर्काऊ सत्य;
        }
    }
    फर्काऊ असत्य;
}

CharString &CharString::truncate(int32_t newLength) {
    यदि(newLength<0) {
        newLength=0;
    }
    यदि(newLength<len) {
        buffer[len=newLength]=0;
    }
    फर्काऊ *एतत्;
}

CharString &CharString::append(वर्ण c, UErrorCode &errorCode) {
    यदि(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    फर्काऊ *एतत्;
}

CharString &CharString::append(अचर वर्ण *s, int32_t sLength, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    यदि(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    यदि(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(sLength>0) {
        यदि(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            यदि(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } अन्यथा {
                buffer[len+=sLength]=0;
            }
        } अन्यथा यदि(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            फर्काऊ append(CharString(s, sLength, errorCode), errorCode);
        } अन्यथा यदि(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    फर्काऊ *एतत्;
}

CharString &CharString::append_सङ्ख्या(int64_t number, UErrorCode &status) {
    यदि (number < 0) {
        एतत्->append('-', status);
        यदि (U_FAILURE(status)) {
            फर्काऊ *एतत्;
        }
    }

    यदि (number == 0) {
        एतत्->append('0', status);
        फर्काऊ *एतत्;
    }

    int32_t अङ्क_Len = 0;
    जबसम्म (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        एतत्->append(std::असल(residue) + '0', status);
        अङ्क_Len++;
        यदि (U_FAILURE(status)) {
            फर्काऊ *एतत्;
        }
    }

    int32_t start = एतत्->length() - अङ्क_Len, end = एतत्->length() - 1;
    जबसम्म(start < end) {
        std::swap(एतत्->data()[start++], एतत्->data()[end--]);
    }

    फर्काऊ *एतत्;
}

वर्ण *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        resultCapacity=0;
        फर्काऊ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    यदि(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        फर्काऊ buffer.getAlias()+len;
    }
    यदि(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        फर्काऊ buffer.getAlias()+len;
    }
    resultCapacity=0;
    फर्काऊ nullptr;
}

CharString &CharString::appendInvariantChars(अचर UnicodeString &s, UErrorCode &errorCode) {
    फर्काऊ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(अचर char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    यदि (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        फर्काऊ *एतत्;
    }
    यदि(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    फर्काऊ *एतत्;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    यदि(capacity>buffer.getCapacity()) {
        यदि(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        यदि( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }
    }
    फर्काऊ सत्य;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    यदि(s.length()==0) {
        फर्काऊ *एतत्;
    }
    वर्ण c;
    यदि(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    फर्काऊ *एतत्;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    वर्ण c;
    यदि(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    फर्काऊ *एतत्;
}

वर्ण CharString::getDirSepChar() अचर {
    वर्ण dirSepChar = U_FILE_SEP_CHAR;
#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    यदि(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    फर्काऊ dirSepChar;
}

U_NAMESPACE_END
