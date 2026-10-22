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

#அந்தர்பாவ <cstdlib>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    திரும்பு *இது;
}

எழுத்து *CharString::cloneData(UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    எழுத்து *p = static_cast<எழுத்து *>(uprv_malloc(len + 1));
    எனில் (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    திரும்பு p;
}

int32_t CharString::extract(எழுத்து *dest, int32_t capacity, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு len; }
    எனில் (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு len;
    }
    மாறிலி எழுத்து *src = buffer.getAlias();
    எனில் (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    திரும்பு u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(மாறிலி CharString &s, UErrorCode &errorCode) {
    எனில்(U_SUCCESS(errorCode) && இது!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    திரும்பு *இது;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    len = 0;
    append(s, errorCode);
    திரும்பு *இது;
}

int32_t CharString::lastIndexOf(எழுத்து c) மாறிலி {
    க்கு(int32_t i=len; i>0;) {
        எனில்(buffer[--i]==c) {
            திரும்பு i;
        }
    }
    திரும்பு -1;
}

தருக்க CharString::contains(StringPiece s) மாறிலி {
    எனில் (s.empty()) { திரும்பு பொய்; }
    மாறிலி எழுத்து *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    க்கு (int32_t i = 0; i <= lastStart; ++i) {
        எனில் (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            திரும்பு உண்மை;
        }
    }
    திரும்பு பொய்;
}

CharString &CharString::truncate(int32_t newLength) {
    எனில்(newLength<0) {
        newLength=0;
    }
    எனில்(newLength<len) {
        buffer[len=newLength]=0;
    }
    திரும்பு *இது;
}

CharString &CharString::append(எழுத்து c, UErrorCode &errorCode) {
    எனில்(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    திரும்பு *இது;
}

CharString &CharString::append(மாறிலி எழுத்து *s, int32_t sLength, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    எனில்(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    எனில்(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    எனில்(sLength>0) {
        எனில்(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            எனில்(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } இல்லையெனில் {
                buffer[len+=sLength]=0;
            }
        } இல்லையெனில் எனில்(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            திரும்பு append(CharString(s, sLength, errorCode), errorCode);
        } இல்லையெனில் எனில்(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    திரும்பு *இது;
}

CharString &CharString::append_எண்(int64_t number, UErrorCode &status) {
    எனில் (number < 0) {
        இது->append('-', status);
        எனில் (U_FAILURE(status)) {
            திரும்பு *இது;
        }
    }

    எனில் (number == 0) {
        இது->append('0', status);
        திரும்பு *இது;
    }

    int32_t இலக்கம்_Len = 0;
    போது (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        இது->append(std::abs(residue) + '0', status);
        இலக்கம்_Len++;
        எனில் (U_FAILURE(status)) {
            திரும்பு *இது;
        }
    }

    int32_t start = இது->length() - இலக்கம்_Len, end = இது->length() - 1;
    போது(start < end) {
        std::swap(இது->data()[start++], இது->data()[end--]);
    }

    திரும்பு *இது;
}

எழுத்து *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        resultCapacity=0;
        திரும்பு nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    எனில்(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        திரும்பு buffer.getAlias()+len;
    }
    எனில்(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        திரும்பு buffer.getAlias()+len;
    }
    resultCapacity=0;
    திரும்பு nullptr;
}

CharString &CharString::appendInvariantChars(மாறிலி UnicodeString &s, UErrorCode &errorCode) {
    திரும்பு appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(மாறிலி char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    எனில் (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        திரும்பு *இது;
    }
    எனில்(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    திரும்பு *இது;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    எனில்(capacity>buffer.getCapacity()) {
        எனில்(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        எனில்( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    எனில்(s.length()==0) {
        திரும்பு *இது;
    }
    எழுத்து c;
    எனில்(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    திரும்பு *இது;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    எழுத்து c;
    எனில்(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    திரும்பு *இது;
}

எழுத்து CharString::getDirSepChar() மாறிலி {
    எழுத்து dirSepChar = U_FILE_SEP_CHAR;
#எனில் (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    எனில்(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    திரும்பு dirSepChar;
}

U_NAMESPACE_END
