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
    戻る *this;
}

char *CharString::cloneData(UErrorCode &errorCode) const {
    もし (U_FAILURE(errorCode)) { 戻る nullptr; }
    char *p = static_cast<char *>(uprv_malloc(len + 1));
    もし (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        戻る nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    戻る p;
}

int32_t CharString::extract(char *dest, int32_t capacity, UErrorCode &errorCode) const {
    もし (U_FAILURE(errorCode)) { 戻る len; }
    もし (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        戻る len;
    }
    const char *src = buffer.getAlias();
    もし (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    戻る u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    もし(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    戻る *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    もし (U_FAILURE(errorCode)) {
        戻る *this;
    }
    len = 0;
    append(s, errorCode);
    戻る *this;
}

int32_t CharString::lastIndexOf(char c) const {
    ために(int32_t i=len; i>0;) {
        もし(buffer[--i]==c) {
            戻る i;
        }
    }
    戻る -1;
}

bool CharString::contains(StringPiece s) const {
    もし (s.empty()) { 戻る false; }
    const char *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    ために (int32_t i = 0; i <= lastStart; ++i) {
        もし (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            戻る true;
        }
    }
    戻る false;
}

CharString &CharString::truncate(int32_t newLength) {
    もし(newLength<0) {
        newLength=0;
    }
    もし(newLength<len) {
        buffer[len=newLength]=0;
    }
    戻る *this;
}

CharString &CharString::append(char c, UErrorCode &errorCode) {
    もし(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    戻る *this;
}

CharString &CharString::append(const char *s, int32_t sLength, UErrorCode &errorCode) {
    もし(U_FAILURE(errorCode)) {
        戻る *this;
    }
    もし(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        戻る *this;
    }
    もし(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    もし(sLength>0) {
        もし(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            もし(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } 否则 {
                buffer[len+=sLength]=0;
            }
        } 否则 もし(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            戻る append(CharString(s, sLength, errorCode), errorCode);
        } 否则 もし(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    戻る *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    もし (number < 0) {
        this->append('-', status);
        もし (U_FAILURE(status)) {
            戻る *this;
        }
    }

    もし (number == 0) {
        this->append('0', status);
        戻る *this;
    }

    int32_t numLen = 0;
    ながら (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        もし (U_FAILURE(status)) {
            戻る *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ながら(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    戻る *this;
}

char *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    もし(U_FAILURE(errorCode)) {
        resultCapacity=0;
        戻る nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    もし(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        戻る buffer.getAlias()+len;
    }
    もし(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        戻る buffer.getAlias()+len;
    }
    resultCapacity=0;
    戻る nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    戻る appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    もし(U_FAILURE(errorCode)) {
        戻る *this;
    }
    もし (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        戻る *this;
    }
    もし(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    戻る *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    もし(U_FAILURE(errorCode)) {
        戻る false;
    }
    もし(capacity>buffer.getCapacity()) {
        もし(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        もし( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            戻る false;
        }
    }
    戻る true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    もし(U_FAILURE(errorCode)) {
        戻る *this;
    }
    もし(s.length()==0) {
        戻る *this;
    }
    char c;
    もし(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    戻る *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    char c;
    もし(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    戻る *this;
}

char CharString::getDirSepChar() const {
    char dirSepChar = U_FILE_SEP_CHAR;
#もし (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    もし(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    戻る dirSepChar;
}

U_NAMESPACE_END
