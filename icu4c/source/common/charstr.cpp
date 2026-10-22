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
    คืน *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    ถ้า (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    คืน p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    ถ้า (U_FAILURE(errorCode)) { คืน len; }
    ถ้า (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน len;
    }
    const var_na *src = buffer.getAlias();
    ถ้า (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    คืน u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ถ้า(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    คืน *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ถ้า (U_FAILURE(errorCode)) {
        คืน *this;
    }
    len = 0;
    append(s, errorCode);
    คืน *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    สำหรับ(int32_t i=len; i>0;) {
        ถ้า(buffer[--i]==c) {
            คืน i;
        }
    }
    คืน -1;
}

bool CharString::contains(StringPiece s) const {
    ถ้า (s.empty()) { คืน false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    สำหรับ (int32_t i = 0; i <= lastStart; ++i) {
        ถ้า (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            คืน true;
        }
    }
    คืน false;
}

CharString &CharString::truncate(int32_t newLength) {
    ถ้า(newLength<0) {
        newLength=0;
    }
    ถ้า(newLength<len) {
        buffer[len=newLength]=0;
    }
    คืน *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    ถ้า(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    คืน *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    ถ้า(U_FAILURE(errorCode)) {
        คืน *this;
    }
    ถ้า(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        คืน *this;
    }
    ถ้า(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ถ้า(sLength>0) {
        ถ้า(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ถ้า(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } มิฉะนั้น {
                buffer[len+=sLength]=0;
            }
        } มิฉะนั้น ถ้า(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            คืน append(CharString(s, sLength, errorCode), errorCode);
        } มิฉะนั้น ถ้า(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    คืน *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ถ้า (number < 0) {
        this->append('-', status);
        ถ้า (U_FAILURE(status)) {
            คืน *this;
        }
    }

    ถ้า (number == 0) {
        this->append('0', status);
        คืน *this;
    }

    int32_t numLen = 0;
    ขณะ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ถ้า (U_FAILURE(status)) {
            คืน *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ขณะ(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    คืน *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ถ้า(U_FAILURE(errorCode)) {
        resultCapacity=0;
        คืน nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ถ้า(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        คืน buffer.getAlias()+len;
    }
    ถ้า(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        คืน buffer.getAlias()+len;
    }
    resultCapacity=0;
    คืน nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    คืน appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ถ้า(U_FAILURE(errorCode)) {
        คืน *this;
    }
    ถ้า (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        คืน *this;
    }
    ถ้า(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    คืน *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ถ้า(U_FAILURE(errorCode)) {
        คืน false;
    }
    ถ้า(capacity>buffer.getCapacity()) {
        ถ้า(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ถ้า( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            คืน false;
        }
    }
    คืน true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ถ้า(U_FAILURE(errorCode)) {
        คืน *this;
    }
    ถ้า(s.length()==0) {
        คืน *this;
    }
    var_na c;
    ถ้า(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    คืน *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    ถ้า(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    คืน *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#ถ้า (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ถ้า(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    คืน dirSepChar;
}

U_NAMESPACE_END
