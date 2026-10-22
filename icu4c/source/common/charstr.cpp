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
    返回 *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    如果 (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回 nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    返回 p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    如果 (U_FAILURE(errorCode)) { 返回 len; }
    如果 (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        返回 len;
    }
    const char *src = buffer.getAlias();
    如果 (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    返回 u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    如果(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    返回 *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    如果 (U_FAILURE(errorCode)) {
        返回 *this;
    }
    len = 0;
    append(s, errorCode);
    返回 *this;
}

int32_t CharString::lastIndexOf(char c) const {
    为(int32_t i=len; i>0;) {
        如果(buffer[--i]==c) {
            返回 i;
        }
    }
    返回 -1;
}

bool CharString::contains(StringPiece s) const {
    如果 (s.empty()) { 返回 false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    为 (int32_t i = 0; i <= lastStart; ++i) {
        如果 (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            返回 true;
        }
    }
    返回 false;
}

CharString &CharString::truncate(int32_t newLength) {
    如果(newLength<0) {
        newLength=0;
    }
    如果(newLength<len) {
        buffer[len=newLength]=0;
    }
    返回 *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    如果(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    返回 *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        返回 *this;
    }
    如果(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回 *this;
    }
    如果(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    如果(sLength>0) {
        如果(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            如果(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } 否则 {
                buffer[len+=sLength]=0;
            }
        } 否则 如果(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            返回 append(CharString(s, sLength, errorCode), errorCode);
        } 否则 如果(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    返回 *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    如果 (number < 0) {
        this->append('-', status);
        如果 (U_FAILURE(status)) {
            返回 *this;
        }
    }

    如果 (number == 0) {
        this->append('0', status);
        返回 *this;
    }

    int32_t numLen = 0;
    当 (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        如果 (U_FAILURE(status)) {
            返回 *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    当(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    返回 *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        resultCapacity=0;
        返回 nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    如果(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        返回 buffer.getAlias()+len;
    }
    如果(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        返回 buffer.getAlias()+len;
    }
    resultCapacity=0;
    返回 nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    返回 appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        返回 *this;
    }
    如果 (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        返回 *this;
    }
    如果(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    返回 *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        返回 false;
    }
    如果(capacity>buffer.getCapacity()) {
        如果(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        如果( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            返回 false;
        }
    }
    返回 true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        返回 *this;
    }
    如果(s.length()==0) {
        返回 *this;
    }
    char c;
    如果(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    返回 *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    如果(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    返回 *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#如果 (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    如果(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    返回 dirSepChar;
}

U_NAMESPACE_END
