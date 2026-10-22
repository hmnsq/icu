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

#include <cstdlib>

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "uinvchar.h"
#include "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    trả_về *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    nếu (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    trả_về p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    nếu (U_FAILURE(errorCode)) { trả_về len; }
    nếu (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về len;
    }
    const char *src = buffer.getAlias();
    nếu (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    trả_về u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    nếu(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    trả_về *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    nếu (U_FAILURE(errorCode)) {
        trả_về *this;
    }
    len = 0;
    append(s, errorCode);
    trả_về *this;
}

int32_t CharString::lastIndexOf(char c) const {
    cho(int32_t i=len; i>0;) {
        nếu(buffer[--i]==c) {
            trả_về i;
        }
    }
    trả_về -1;
}

bool CharString::contains(StringPiece s) const {
    nếu (s.empty()) { trả_về false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    cho (int32_t i = 0; i <= lastStart; ++i) {
        nếu (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            trả_về true;
        }
    }
    trả_về false;
}

CharString &CharString::truncate(int32_t newLength) {
    nếu(newLength<0) {
        newLength=0;
    }
    nếu(newLength<len) {
        buffer[len=newLength]=0;
    }
    trả_về *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    nếu(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    trả_về *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về *this;
    }
    nếu(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    nếu(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    nếu(sLength>0) {
        nếu(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            nếu(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } không_thì {
                buffer[len+=sLength]=0;
            }
        } không_thì nếu(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            trả_về append(CharString(s, sLength, errorCode), errorCode);
        } không_thì nếu(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    trả_về *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    nếu (number < 0) {
        this->append('-', status);
        nếu (U_FAILURE(status)) {
            trả_về *this;
        }
    }

    nếu (number == 0) {
        this->append('0', status);
        trả_về *this;
    }

    int32_t numLen = 0;
    trong_khi (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        nếu (U_FAILURE(status)) {
            trả_về *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    trong_khi(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    trả_về *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        resultCapacity=0;
        trả_về nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    nếu(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        trả_về buffer.getAlias()+len;
    }
    nếu(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        trả_về buffer.getAlias()+len;
    }
    resultCapacity=0;
    trả_về nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    trả_về appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về *this;
    }
    nếu (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        trả_về *this;
    }
    nếu(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    trả_về *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về false;
    }
    nếu(capacity>buffer.getCapacity()) {
        nếu(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        nếu( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            trả_về false;
        }
    }
    trả_về true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về *this;
    }
    nếu(s.length()==0) {
        trả_về *this;
    }
    char c;
    nếu(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    trả_về *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    nếu(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    trả_về *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#nếu (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    nếu(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    trả_về dirSepChar;
}

U_NAMESPACE_END
