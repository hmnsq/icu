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
    ཕྱིར *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    གལ་ཏེ (U_FAILURE(errorCode)) { ཕྱིར nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    གལ་ཏེ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ཕྱིར nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ཕྱིར p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    གལ་ཏེ (U_FAILURE(errorCode)) { ཕྱིར len; }
    གལ་ཏེ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར len;
    }
    const var_na *src = buffer.getAlias();
    གལ་ཏེ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ཕྱིར u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    གལ་ཏེ(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ཕྱིར *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    གལ་ཏེ (U_FAILURE(errorCode)) {
        ཕྱིར *this;
    }
    len = 0;
    append(s, errorCode);
    ཕྱིར *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    ཆེད་དུ(int32_t i=len; i>0;) {
        གལ་ཏེ(buffer[--i]==c) {
            ཕྱིར i;
        }
    }
    ཕྱིར -1;
}

bool CharString::contains(StringPiece s) const {
    གལ་ཏེ (s.empty()) { ཕྱིར false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    ཆེད་དུ (int32_t i = 0; i <= lastStart; ++i) {
        གལ་ཏེ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ཕྱིར true;
        }
    }
    ཕྱིར false;
}

CharString &CharString::truncate(int32_t newLength) {
    གལ་ཏེ(newLength<0) {
        newLength=0;
    }
    གལ་ཏེ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ཕྱིར *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    གལ་ཏེ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ཕྱིར *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    གལ་ཏེ(U_FAILURE(errorCode)) {
        ཕྱིར *this;
    }
    གལ་ཏེ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར *this;
    }
    གལ་ཏེ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    གལ་ཏེ(sLength>0) {
        གལ་ཏེ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            གལ་ཏེ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } དེ་མིན {
                buffer[len+=sLength]=0;
            }
        } དེ་མིན གལ་ཏེ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ཕྱིར append(CharString(s, sLength, errorCode), errorCode);
        } དེ་མིན གལ་ཏེ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ཕྱིར *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    གལ་ཏེ (number < 0) {
        this->append('-', status);
        གལ་ཏེ (U_FAILURE(status)) {
            ཕྱིར *this;
        }
    }

    གལ་ཏེ (number == 0) {
        this->append('0', status);
        ཕྱིར *this;
    }

    int32_t numLen = 0;
    སྐབས (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        གལ་ཏེ (U_FAILURE(status)) {
            ཕྱིར *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    སྐབས(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ཕྱིར *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    གལ་ཏེ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ཕྱིར nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    གལ་ཏེ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ཕྱིར buffer.getAlias()+len;
    }
    གལ་ཏེ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ཕྱིར buffer.getAlias()+len;
    }
    resultCapacity=0;
    ཕྱིར nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ཕྱིར appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    གལ་ཏེ(U_FAILURE(errorCode)) {
        ཕྱིར *this;
    }
    གལ་ཏེ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ཕྱིར *this;
    }
    གལ་ཏེ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ཕྱིར *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    གལ་ཏེ(U_FAILURE(errorCode)) {
        ཕྱིར false;
    }
    གལ་ཏེ(capacity>buffer.getCapacity()) {
        གལ་ཏེ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        གལ་ཏེ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ཕྱིར false;
        }
    }
    ཕྱིར true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    གལ་ཏེ(U_FAILURE(errorCode)) {
        ཕྱིར *this;
    }
    གལ་ཏེ(s.length()==0) {
        ཕྱིར *this;
    }
    var_na c;
    གལ་ཏེ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ཕྱིར *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    གལ་ཏེ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ཕྱིར *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#གལ་ཏེ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    གལ་ཏེ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ཕྱིར dirSepChar;
}

U_NAMESPACE_END
