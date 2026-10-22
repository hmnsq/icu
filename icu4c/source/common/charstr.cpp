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
    повернути *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    якщо (U_FAILURE(errorCode)) { повернути nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    якщо (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        повернути nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    повернути p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    якщо (U_FAILURE(errorCode)) { повернути len; }
    якщо (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        повернути len;
    }
    const char *src = buffer.getAlias();
    якщо (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    повернути u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    якщо(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    повернути *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    якщо (U_FAILURE(errorCode)) {
        повернути *this;
    }
    len = 0;
    append(s, errorCode);
    повернути *this;
}

int32_t CharString::lastIndexOf(char c) const {
    для(int32_t i=len; i>0;) {
        якщо(buffer[--i]==c) {
            повернути i;
        }
    }
    повернути -1;
}

bool CharString::contains(StringPiece s) const {
    якщо (s.empty()) { повернути false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    для (int32_t i = 0; i <= lastStart; ++i) {
        якщо (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            повернути true;
        }
    }
    повернути false;
}

CharString &CharString::truncate(int32_t newLength) {
    якщо(newLength<0) {
        newLength=0;
    }
    якщо(newLength<len) {
        buffer[len=newLength]=0;
    }
    повернути *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    якщо(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    повернути *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    якщо(U_FAILURE(errorCode)) {
        повернути *this;
    }
    якщо(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        повернути *this;
    }
    якщо(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    якщо(sLength>0) {
        якщо(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            якщо(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } інакше {
                buffer[len+=sLength]=0;
            }
        } інакше якщо(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            повернути append(CharString(s, sLength, errorCode), errorCode);
        } інакше якщо(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    повернути *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    якщо (number < 0) {
        this->append('-', status);
        якщо (U_FAILURE(status)) {
            повернути *this;
        }
    }

    якщо (number == 0) {
        this->append('0', status);
        повернути *this;
    }

    int32_t numLen = 0;
    поки (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        якщо (U_FAILURE(status)) {
            повернути *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    поки(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    повернути *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    якщо(U_FAILURE(errorCode)) {
        resultCapacity=0;
        повернути nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    якщо(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        повернути buffer.getAlias()+len;
    }
    якщо(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        повернути buffer.getAlias()+len;
    }
    resultCapacity=0;
    повернути nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    повернути appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    якщо(U_FAILURE(errorCode)) {
        повернути *this;
    }
    якщо (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        повернути *this;
    }
    якщо(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    повернути *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    якщо(U_FAILURE(errorCode)) {
        повернути false;
    }
    якщо(capacity>buffer.getCapacity()) {
        якщо(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        якщо( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            повернути false;
        }
    }
    повернути true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    якщо(U_FAILURE(errorCode)) {
        повернути *this;
    }
    якщо(s.length()==0) {
        повернути *this;
    }
    char c;
    якщо(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    повернути *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    якщо(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    повернути *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#якщо (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    якщо(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    повернути dirSepChar;
}

U_NAMESPACE_END
