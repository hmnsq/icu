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
    врати *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    ако (U_FAILURE(errorCode)) { врати nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    ако (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        врати nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    врати p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    ако (U_FAILURE(errorCode)) { врати len; }
    ако (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        врати len;
    }
    const char *src = buffer.getAlias();
    ако (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    врати u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ако(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    врати *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ако (U_FAILURE(errorCode)) {
        врати *this;
    }
    len = 0;
    append(s, errorCode);
    врати *this;
}

int32_t CharString::lastIndexOf(char c) const {
    за(int32_t i=len; i>0;) {
        ако(buffer[--i]==c) {
            врати i;
        }
    }
    врати -1;
}

bool CharString::contains(StringPiece s) const {
    ако (s.empty()) { врати false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    за (int32_t i = 0; i <= lastStart; ++i) {
        ако (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            врати true;
        }
    }
    врати false;
}

CharString &CharString::truncate(int32_t newLength) {
    ако(newLength<0) {
        newLength=0;
    }
    ако(newLength<len) {
        buffer[len=newLength]=0;
    }
    врати *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    ако(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    врати *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        врати *this;
    }
    ако(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        врати *this;
    }
    ако(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ако(sLength>0) {
        ако(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ако(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } инако {
                buffer[len+=sLength]=0;
            }
        } инако ако(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            врати append(CharString(s, sLength, errorCode), errorCode);
        } инако ако(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    врати *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ако (number < 0) {
        this->append('-', status);
        ако (U_FAILURE(status)) {
            врати *this;
        }
    }

    ако (number == 0) {
        this->append('0', status);
        врати *this;
    }

    int32_t numLen = 0;
    док (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ако (U_FAILURE(status)) {
            врати *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    док(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    врати *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        resultCapacity=0;
        врати nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ако(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        врати buffer.getAlias()+len;
    }
    ако(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        врати buffer.getAlias()+len;
    }
    resultCapacity=0;
    врати nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    врати appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        врати *this;
    }
    ако (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        врати *this;
    }
    ако(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    врати *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        врати false;
    }
    ако(capacity>buffer.getCapacity()) {
        ако(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ако( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            врати false;
        }
    }
    врати true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        врати *this;
    }
    ако(s.length()==0) {
        врати *this;
    }
    char c;
    ако(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    врати *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    ако(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    врати *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#ако (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ако(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    врати dirSepChar;
}

U_NAMESPACE_END
