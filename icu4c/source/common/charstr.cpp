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
    вернуть *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    если (U_FAILURE(errorCode)) { вернуть nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    если (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        вернуть nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    вернуть p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    если (U_FAILURE(errorCode)) { вернуть len; }
    если (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть len;
    }
    const char *src = buffer.getAlias();
    если (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    вернуть u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    если(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    вернуть *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    если (U_FAILURE(errorCode)) {
        вернуть *this;
    }
    len = 0;
    append(s, errorCode);
    вернуть *this;
}

int32_t CharString::lastIndexOf(char c) const {
    для(int32_t i=len; i>0;) {
        если(buffer[--i]==c) {
            вернуть i;
        }
    }
    вернуть -1;
}

bool CharString::contains(StringPiece s) const {
    если (s.empty()) { вернуть false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    для (int32_t i = 0; i <= lastStart; ++i) {
        если (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            вернуть true;
        }
    }
    вернуть false;
}

CharString &CharString::truncate(int32_t newLength) {
    если(newLength<0) {
        newLength=0;
    }
    если(newLength<len) {
        buffer[len=newLength]=0;
    }
    вернуть *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    если(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    вернуть *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть *this;
    }
    если(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    если(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    если(sLength>0) {
        если(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            если(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } иначе {
                buffer[len+=sLength]=0;
            }
        } иначе если(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            вернуть append(CharString(s, sLength, errorCode), errorCode);
        } иначе если(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    вернуть *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    если (number < 0) {
        this->append('-', status);
        если (U_FAILURE(status)) {
            вернуть *this;
        }
    }

    если (number == 0) {
        this->append('0', status);
        вернуть *this;
    }

    int32_t numLen = 0;
    пока (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        если (U_FAILURE(status)) {
            вернуть *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    пока(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    вернуть *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        resultCapacity=0;
        вернуть nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    если(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        вернуть buffer.getAlias()+len;
    }
    если(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        вернуть buffer.getAlias()+len;
    }
    resultCapacity=0;
    вернуть nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    вернуть appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть *this;
    }
    если (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        вернуть *this;
    }
    если(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    вернуть *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть false;
    }
    если(capacity>buffer.getCapacity()) {
        если(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        если( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            вернуть false;
        }
    }
    вернуть true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть *this;
    }
    если(s.length()==0) {
        вернуть *this;
    }
    char c;
    если(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    вернуть *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    если(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    вернуть *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#если (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    если(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    вернуть dirSepChar;
}

U_NAMESPACE_END
