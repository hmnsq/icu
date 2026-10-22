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
    ສົ່ງຄືນ *this;
}

var_na *CharString::cloneData(UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    var_na *p = static_cast<var_na *>(uprv_malloc(len + 1));
    ຖ້າ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ສົ່ງຄືນ p;
}

int32_t CharString::extract(var_na *dest, int32_t capacity, UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ len; }
    ຖ້າ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ len;
    }
    const var_na *src = buffer.getAlias();
    ຖ້າ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ສົ່ງຄືນ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(const CharString &s, UErrorCode &errorCode) {
    ຖ້າ(U_SUCCESS(errorCode) && this!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ສົ່ງຄືນ *this;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ *this;
    }
    len = 0;
    append(s, errorCode);
    ສົ່ງຄືນ *this;
}

int32_t CharString::lastIndexOf(var_na c) const {
    ສຳລັບ(int32_t i=len; i>0;) {
        ຖ້າ(buffer[--i]==c) {
            ສົ່ງຄືນ i;
        }
    }
    ສົ່ງຄືນ -1;
}

bool CharString::contains(StringPiece s) const {
    ຖ້າ (s.empty()) { ສົ່ງຄືນ false; }
    const var_na *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    ສຳລັບ (int32_t i = 0; i <= lastStart; ++i) {
        ຖ້າ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ສົ່ງຄືນ true;
        }
    }
    ສົ່ງຄືນ false;
}

CharString &CharString::truncate(int32_t newLength) {
    ຖ້າ(newLength<0) {
        newLength=0;
    }
    ຖ້າ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ສົ່ງຄືນ *this;
}

CharString &CharString::append(var_na c, UErrorCode &errorCode) {
    ຖ້າ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ສົ່ງຄືນ *this;
}

CharString &CharString::append(const var_na *s, int32_t sLength, UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ *this;
    }
    ຖ້າ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    ຖ້າ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ຖ້າ(sLength>0) {
        ຖ້າ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ຖ້າ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ບໍ່ດັ່ງນັ້ນ {
                buffer[len+=sLength]=0;
            }
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ສົ່ງຄືນ append(CharString(s, sLength, errorCode), errorCode);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ສົ່ງຄືນ *this;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ຖ້າ (number < 0) {
        this->append('-', status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ *this;
        }
    }

    ຖ້າ (number == 0) {
        this->append('0', status);
        ສົ່ງຄືນ *this;
    }

    int32_t numLen = 0;
    ໃນຂະນະທີ່ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        this->append(std::abs(residue) + '0', status);
        numLen++;
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ *this;
        }
    }

    int32_t start = this->length() - numLen, end = this->length() - 1;
    ໃນຂະນະທີ່(start < end) {
        std::swap(this->data()[start++], this->data()[end--]);
    }

    ສົ່ງຄືນ *this;
}

var_na *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ສົ່ງຄືນ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ຖ້າ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ສົ່ງຄືນ buffer.getAlias()+len;
    }
    ຖ້າ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ສົ່ງຄືນ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ສົ່ງຄືນ nullptr;
}

CharString &CharString::appendInvariantChars(const UnicodeString &s, UErrorCode &errorCode) {
    ສົ່ງຄືນ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(const char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ *this;
    }
    ຖ້າ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ສົ່ງຄືນ *this;
    }
    ຖ້າ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ສົ່ງຄືນ *this;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ false;
    }
    ຖ້າ(capacity>buffer.getCapacity()) {
        ຖ້າ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ຖ້າ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ false;
        }
    }
    ສົ່ງຄືນ true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ *this;
    }
    ຖ້າ(s.length()==0) {
        ສົ່ງຄືນ *this;
    }
    var_na c;
    ຖ້າ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ສົ່ງຄືນ *this;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    var_na c;
    ຖ້າ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ສົ່ງຄືນ *this;
}

var_na CharString::getDirSepChar() const {
    var_na dirSepChar = U_FILE_SEP_CHAR;
#ຖ້າ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ຖ້າ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ສົ່ງຄືນ dirSepChar;
}

U_NAMESPACE_END
