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
    ᰀᰧᰌ *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    ᰀᰧ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᰀᰧᰌ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ᰀᰧᰌ p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ len; }
    ᰀᰧ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ len;
    }
    const var_na *src = buffer.getAlias();
    ᰀᰧ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ᰀᰧᰌ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ᰀᰧ(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ᰀᰧᰌ *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ᰀᰧ (U_FAILURE(errorCode)) {
        ᰀᰧᰌ *this;
    }
    len = 0;
    append(s, errorCode);
    ᰀᰧᰌ *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    for(int32_t i=len; i>0;) {
        ᰀᰧ(buffer[--i]==c) {
            ᰀᰧᰌ i;
        }
    }
    ᰀᰧᰌ -1;
}

bool CharString::contains(StringPiece s) const {
    ᰀᰧ (s.empty()) { ᰀᰧᰌ false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    for (int32_t i = 0; i <= lastStart; ++i) {
        ᰀᰧ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ᰀᰧᰌ true;
        }
    }
    ᰀᰧᰌ false;
}

CharString &CharString::truncate(int32_t newLength) {
    ᰀᰧ(newLength<0) {
        newLength=0;
    }
    ᰀᰧ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ᰀᰧᰌ *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    ᰀᰧ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ᰀᰧᰌ *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ *this;
    }
    ᰀᰧ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    ᰀᰧ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ᰀᰧ(sLength>0) {
        ᰀᰧ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ᰀᰧ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ᰀᰧᰀ {
                buffer[len+=sLength]=0;
            }
        } ᰀᰧᰀ ᰀᰧ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ᰀᰧᰌ append(CharString(s, sLength, errorCode), errorCode);
        } ᰀᰧᰀ ᰀᰧ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ᰀᰧᰌ *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ᰀᰧ (number < 0) {
        this->append('-', status);
        ᰀᰧ (U_FAILURE(status)) {
            ᰀᰧᰌ *this;
        }
    }

    ᰀᰧ (number == 0) {
        this->append('0', status);
        ᰀᰧᰌ *this;
    }

    int32_t numLen = 0;
    ᰀᰬ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ᰀᰧ (U_FAILURE(status)) {
            ᰀᰧᰌ *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ᰀᰬ(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ᰀᰧᰌ *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ᰀᰧ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ᰀᰧᰌ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ᰀᰧ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ᰀᰧᰌ buffer.getAlias()+len;
    }
    ᰀᰧ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ᰀᰧᰌ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ᰀᰧᰌ nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ᰀᰧᰌ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ *this;
    }
    ᰀᰧ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ᰀᰧᰌ *this;
    }
    ᰀᰧ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ᰀᰧᰌ *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ false;
    }
    ᰀᰧ(capacity>buffer.getCapacity()) {
        ᰀᰧ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ᰀᰧ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᰀᰧᰌ false;
        }
    }
    ᰀᰧᰌ true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ *this;
    }
    ᰀᰧ(s.length()==0) {
        ᰀᰧᰌ *this;
    }
    var_na c;
    ᰀᰧ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ᰀᰧᰌ *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    ᰀᰧ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ᰀᰧᰌ *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#ᰀᰧ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ᰀᰧ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ᰀᰧᰌ dirSepChar;
}

U_NAMESPACE_END
