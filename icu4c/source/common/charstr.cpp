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

#ଅନ୍ତର୍ଭାଵ <cstdlib>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ଫେରନ୍ତୁ *ଏହି;
}

ଅକ୍ଷର *CharString::cloneData(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଅକ୍ଷର *p = static_cast<ଅକ୍ଷର *>(uprv_malloc(len + 1));
    ଯଦି (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ଫେରନ୍ତୁ p;
}

int32_t CharString::extract(ଅକ୍ଷର *dest, int32_t capacity, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ len; }
    ଯଦି (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ len;
    }
    ଅଚଳ ଅକ୍ଷର *src = buffer.getAlias();
    ଯଦି (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ଫେରନ୍ତୁ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ଅଚଳ CharString &s, UErrorCode &errorCode) {
    ଯଦି(U_SUCCESS(errorCode) && ଏହି!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    len = 0;
    append(s, errorCode);
    ଫେରନ୍ତୁ *ଏହି;
}

int32_t CharString::lastIndexOf(ଅକ୍ଷର c) ଅଚଳ {
    ପାଇଁ(int32_t i=len; i>0;) {
        ଯଦି(buffer[--i]==c) {
            ଫେରନ୍ତୁ i;
        }
    }
    ଫେରନ୍ତୁ -1;
}

ତାର୍କିକ CharString::contains(StringPiece s) ଅଚଳ {
    ଯଦି (s.empty()) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଅଚଳ ଅକ୍ଷର *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    ପାଇଁ (int32_t i = 0; i <= lastStart; ++i) {
        ଯଦି (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

CharString &CharString::truncate(int32_t newLength) {
    ଯଦି(newLength<0) {
        newLength=0;
    }
    ଯଦି(newLength<len) {
        buffer[len=newLength]=0;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

CharString &CharString::append(ଅକ୍ଷର c, UErrorCode &errorCode) {
    ଯଦି(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

CharString &CharString::append(ଅଚଳ ଅକ୍ଷର *s, int32_t sLength, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ଯଦି(sLength>0) {
        ଯଦି(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ଯଦି(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ଅନ୍ୟଥା {
                buffer[len+=sLength]=0;
            }
        } ଅନ୍ୟଥା ଯଦି(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ଫେରନ୍ତୁ append(CharString(s, sLength, errorCode), errorCode);
        } ଅନ୍ୟଥା ଯଦି(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
}

CharString &CharString::append_ସଂଖ୍ୟା(int64_t number, UErrorCode &status) {
    ଯଦି (number < 0) {
        ଏହି->append('-', status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ *ଏହି;
        }
    }

    ଯଦି (number == 0) {
        ଏହି->append('0', status);
        ଫେରନ୍ତୁ *ଏହି;
    }

    int32_t ଅଙ୍କ_Len = 0;
    ଯେତେବେଳେ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ଏହି->append(std::abs(residue) + '0', status);
        ଅଙ୍କ_Len++;
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ *ଏହି;
        }
    }

    int32_t start = ଏହି->length() - ଅଙ୍କ_Len, end = ଏହି->length() - 1;
    ଯେତେବେଳେ(start < end) {
        std::swap(ଏହି->data()[start++], ଏହି->data()[end--]);
    }

    ଫେରନ୍ତୁ *ଏହି;
}

ଅକ୍ଷର *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ଫେରନ୍ତୁ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ଯଦି(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ଫେରନ୍ତୁ buffer.getAlias()+len;
    }
    ଯଦି(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ଫେରନ୍ତୁ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ଫେରନ୍ତୁ nullptr;
}

CharString &CharString::appendInvariantChars(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) {
    ଫେରନ୍ତୁ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ଅଚଳ char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(capacity>buffer.getCapacity()) {
        ଯଦି(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ଯଦି( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(s.length()==0) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଅକ୍ଷର c;
    ଯଦି(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ଫେରନ୍ତୁ *ଏହି;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    ଅକ୍ଷର c;
    ଯଦି(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

ଅକ୍ଷର CharString::getDirSepChar() ଅଚଳ {
    ଅକ୍ଷର dirSepChar = U_FILE_SEP_CHAR;
#ଯଦି (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ଯଦି(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ଫେରନ୍ତୁ dirSepChar;
}

U_NAMESPACE_END
