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
    ᬯᬮᬶ *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    ᬬᬾᬦ᭄ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ᬯᬮᬶ p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) { ᬯᬮᬶ len; }
    ᬬᬾᬦ᭄ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ len;
    }
    const var_na *src = buffer.getAlias();
    ᬬᬾᬦ᭄ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ᬯᬮᬶ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ᬯᬮᬶ *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄ (U_FAILURE(errorCode)) {
        ᬯᬮᬶ *this;
    }
    len = 0;
    append(s, errorCode);
    ᬯᬮᬶ *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    for(int32_t i=len; i>0;) {
        ᬬᬾᬦ᭄(buffer[--i]==c) {
            ᬯᬮᬶ i;
        }
    }
    ᬯᬮᬶ -1;
}

bool CharString::contains(StringPiece s) const {
    ᬬᬾᬦ᭄ (s.empty()) { ᬯᬮᬶ false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    for (int32_t i = 0; i <= lastStart; ++i) {
        ᬬᬾᬦ᭄ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ᬯᬮᬶ true;
        }
    }
    ᬯᬮᬶ false;
}

CharString &CharString::truncate(int32_t newLength) {
    ᬬᬾᬦ᭄(newLength<0) {
        newLength=0;
    }
    ᬬᬾᬦ᭄(newLength<len) {
        buffer[len=newLength]=0;
    }
    ᬯᬮᬶ *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ᬯᬮᬶ *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ᬬᬾᬦ᭄(sLength>0) {
        ᬬᬾᬦ᭄(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ᬬᬾᬦ᭄(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ᬮᬶᬬ {
                buffer[len+=sLength]=0;
            }
        } ᬮᬶᬬ ᬬᬾᬦ᭄(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ᬯᬮᬶ append(CharString(s, sLength, errorCode), errorCode);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ᬯᬮᬶ *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ᬬᬾᬦ᭄ (number < 0) {
        this->append('-', status);
        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
            ᬯᬮᬶ *this;
        }
    }

    ᬬᬾᬦ᭄ (number == 0) {
        this->append('0', status);
        ᬯᬮᬶ *this;
    }

    int32_t numLen = 0;
    ᬲᭂᬤᭂᬮ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ᬬᬾᬦ᭄ (U_FAILURE(status)) {
            ᬯᬮᬶ *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ᬲᭂᬤᭂᬮ(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ᬯᬮᬶ *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ᬯᬮᬶ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ᬬᬾᬦ᭄(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ᬯᬮᬶ buffer.getAlias()+len;
    }
    ᬬᬾᬦ᭄(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ᬯᬮᬶ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ᬯᬮᬶ nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ᬯᬮᬶ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ᬯᬮᬶ *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ false;
    }
    ᬬᬾᬦ᭄(capacity>buffer.getCapacity()) {
        ᬬᬾᬦ᭄(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ᬬᬾᬦ᭄( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(s.length()==0) {
        ᬯᬮᬶ *this;
    }
    var_na c;
    ᬬᬾᬦ᭄(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ᬯᬮᬶ *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ᬯᬮᬶ *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#ᬬᬾᬦ᭄ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ᬬᬾᬦ᭄(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ᬯᬮᬶ dirSepChar;
}

U_NAMESPACE_END
