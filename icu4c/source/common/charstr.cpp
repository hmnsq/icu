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
    ᯀᯮᯞ *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    ᯀᯮᯝ (U_FAILURE(errorCode)) { ᯀᯮᯞ nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    ᯀᯮᯝ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ᯀᯮᯞ p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    ᯀᯮᯝ (U_FAILURE(errorCode)) { ᯀᯮᯞ len; }
    ᯀᯮᯝ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ len;
    }
    const var_na *src = buffer.getAlias();
    ᯀᯮᯝ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ᯀᯮᯞ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ᯀᯮᯝ(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ᯀᯮᯞ *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ᯀᯮᯝ (U_FAILURE(errorCode)) {
        ᯀᯮᯞ *this;
    }
    len = 0;
    append(s, errorCode);
    ᯀᯮᯞ *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    for(int32_t i=len; i>0;) {
        ᯀᯮᯝ(buffer[--i]==c) {
            ᯀᯮᯞ i;
        }
    }
    ᯀᯮᯞ -1;
}

bool CharString::contains(StringPiece s) const {
    ᯀᯮᯝ (s.empty()) { ᯀᯮᯞ false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    for (int32_t i = 0; i <= lastStart; ++i) {
        ᯀᯮᯝ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ᯀᯮᯞ true;
        }
    }
    ᯀᯮᯞ false;
}

CharString &CharString::truncate(int32_t newLength) {
    ᯀᯮᯝ(newLength<0) {
        newLength=0;
    }
    ᯀᯮᯝ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ᯀᯮᯞ *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    ᯀᯮᯝ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ᯀᯮᯞ *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        ᯀᯮᯞ *this;
    }
    ᯀᯮᯝ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ *this;
    }
    ᯀᯮᯝ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ᯀᯮᯝ(sLength>0) {
        ᯀᯮᯝ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ᯀᯮᯝ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ᯀᯮᯝᯂᯪ {
                buffer[len+=sLength]=0;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ᯀᯮᯞ append(CharString(s, sLength, errorCode), errorCode);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ᯀᯮᯞ *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ᯀᯮᯝ (number < 0) {
        this->append('-', status);
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ *this;
        }
    }

    ᯀᯮᯝ (number == 0) {
        this->append('0', status);
        ᯀᯮᯞ *this;
    }

    int32_t numLen = 0;
    ᯀᯬᯝ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ᯀᯬᯝ(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ᯀᯮᯞ *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ᯀᯮᯞ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ᯀᯮᯝ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ᯀᯮᯞ buffer.getAlias()+len;
    }
    ᯀᯮᯝ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ᯀᯮᯞ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ᯀᯮᯞ nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ᯀᯮᯞ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        ᯀᯮᯞ *this;
    }
    ᯀᯮᯝ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ᯀᯮᯞ *this;
    }
    ᯀᯮᯝ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ᯀᯮᯞ *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        ᯀᯮᯞ false;
    }
    ᯀᯮᯝ(capacity>buffer.getCapacity()) {
        ᯀᯮᯝ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ᯀᯮᯝ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᯀᯮᯞ false;
        }
    }
    ᯀᯮᯞ true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ᯀᯮᯝ(U_FAILURE(errorCode)) {
        ᯀᯮᯞ *this;
    }
    ᯀᯮᯝ(s.length()==0) {
        ᯀᯮᯞ *this;
    }
    var_na c;
    ᯀᯮᯝ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ᯀᯮᯞ *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    ᯀᯮᯝ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ᯀᯮᯞ *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#ᯀᯮᯝ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ᯀᯮᯝ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ᯀᯮᯞ dirSepChar;
}

U_NAMESPACE_END
