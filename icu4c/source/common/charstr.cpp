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
    ပြန်ပေး *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    အကယ်၍ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ပြန်ပေး nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ပြန်ပေး p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး len; }
    အကယ်၍ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး len;
    }
    const var_na *src = buffer.getAlias();
    အကယ်၍ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ပြန်ပေး u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    အကယ်၍(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ပြန်ပေး *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    အကယ်၍ (U_FAILURE(errorCode)) {
        ပြန်ပေး *this;
    }
    len = 0;
    append(s, errorCode);
    ပြန်ပေး *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    အတွက်(int32_t i=len; i>0;) {
        အကယ်၍(buffer[--i]==c) {
            ပြန်ပေး i;
        }
    }
    ပြန်ပေး -1;
}

bool CharString::contains(StringPiece s) const {
    အကယ်၍ (s.empty()) { ပြန်ပေး false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    အတွက် (int32_t i = 0; i <= lastStart; ++i) {
        အကယ်၍ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ပြန်ပေး true;
        }
    }
    ပြန်ပေး false;
}

CharString &CharString::truncate(int32_t newLength) {
    အကယ်၍(newLength<0) {
        newLength=0;
    }
    အကယ်၍(newLength<len) {
        buffer[len=newLength]=0;
    }
    ပြန်ပေး *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    အကယ်၍(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ပြန်ပေး *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး *this;
    }
    အကယ်၍(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    အကယ်၍(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    အကယ်၍(sLength>0) {
        အကယ်၍(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            အကယ်၍(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } မဟုတ်ပါက {
                buffer[len+=sLength]=0;
            }
        } မဟုတ်ပါက အကယ်၍(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ပြန်ပေး append(CharString(s, sLength, errorCode), errorCode);
        } မဟုတ်ပါက အကယ်၍(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ပြန်ပေး *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    အကယ်၍ (number < 0) {
        this->append('-', status);
        အကယ်၍ (U_FAILURE(status)) {
            ပြန်ပေး *this;
        }
    }

    အကယ်၍ (number == 0) {
        this->append('0', status);
        ပြန်ပေး *this;
    }

    int32_t numLen = 0;
    နေစဉ် (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        အကယ်၍ (U_FAILURE(status)) {
            ပြန်ပေး *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    နေစဉ်(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ပြန်ပေး *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ပြန်ပေး nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    အကယ်၍(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ပြန်ပေး buffer.getAlias()+len;
    }
    အကယ်၍(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ပြန်ပေး buffer.getAlias()+len;
    }
    resultCapacity=0;
    ပြန်ပေး nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ပြန်ပေး appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး *this;
    }
    အကယ်၍ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ပြန်ပေး *this;
    }
    အကယ်၍(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ပြန်ပေး *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး false;
    }
    အကယ်၍(capacity>buffer.getCapacity()) {
        အကယ်၍(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        အကယ်၍( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး false;
        }
    }
    ပြန်ပေး true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    အကယ်၍(U_FAILURE(errorCode)) {
        ပြန်ပေး *this;
    }
    အကယ်၍(s.length()==0) {
        ပြန်ပေး *this;
    }
    var_na c;
    အကယ်၍(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ပြန်ပေး *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    အကယ်၍(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ပြန်ပေး *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#အကယ်၍ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    အကယ်၍(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ပြန်ပေး dirSepChar;
}

U_NAMESPACE_END
