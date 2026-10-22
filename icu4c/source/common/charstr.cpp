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

#ضمن <cstdlib>

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uinvchar.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    החזר *هذا;
}

حرف *CharString::cloneData(UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode)) { החזר nullptr; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    אם (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    החזר p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode)) { החזר len; }
    אם (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        החזר len;
    }
    ثابت حرف *src = buffer.getAlias();
    אם (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    החזר u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ثابت CharString &s, UErrorCode &errorCode) {
    אם(U_SUCCESS(errorCode) && هذا!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    החזר *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    אם (U_FAILURE(errorCode)) {
        החזר *هذا;
    }
    len = 0;
    append(s, errorCode);
    החזר *هذا;
}

int32_t CharString::lastIndexOf(حرف c) ثابت {
    עבור(int32_t i=len; i>0;) {
        אם(buffer[--i]==c) {
            החזר i;
        }
    }
    החזר -1;
}

منطقي CharString::contains(StringPiece s) ثابت {
    אם (s.empty()) { החזר שקר; }
    ثابت حرف *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    עבור (int32_t i = 0; i <= lastStart; ++i) {
        אם (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            החזר אמת;
        }
    }
    החזר שקר;
}

CharString &CharString::truncate(int32_t newLength) {
    אם(newLength<0) {
        newLength=0;
    }
    אם(newLength<len) {
        buffer[len=newLength]=0;
    }
    החזר *هذا;
}

CharString &CharString::append(حرف c, UErrorCode &errorCode) {
    אם(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    החזר *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t sLength, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר *هذا;
    }
    אם(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    אם(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    אם(sLength>0) {
        אם(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            אם(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } אחרת {
                buffer[len+=sLength]=0;
            }
        } אחרת אם(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            החזר append(CharString(s, sLength, errorCode), errorCode);
        } אחרת אם(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    החזר *هذا;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    אם (number < 0) {
        هذا->append('-', status);
        אם (U_FAILURE(status)) {
            החזר *هذا;
        }
    }

    אם (number == 0) {
        هذا->append('0', status);
        החזר *هذا;
    }

    int32_t numLen = 0;
    כלעוד (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::abs(residue) + '0', status);
        numLen++;
        אם (U_FAILURE(status)) {
            החזר *هذا;
        }
    }

    int32_t start = هذا->length() - numLen, end = هذا->length() - 1;
    כלעוד(start < end) {
        std::swap(هذا->data()[start++], هذا->data()[end--]);
    }

    החזר *هذا;
}

حرف *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        resultCapacity=0;
        החזר nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    אם(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        החזר buffer.getAlias()+len;
    }
    אם(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        החזר buffer.getAlias()+len;
    }
    resultCapacity=0;
    החזר nullptr;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UErrorCode &errorCode) {
    החזר appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר *هذا;
    }
    אם (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        החזר *هذا;
    }
    אם(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    החזר *هذا;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר שקר;
    }
    אם(capacity>buffer.getCapacity()) {
        אם(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        אם( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            החזר שקר;
        }
    }
    החזר אמת;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר *هذا;
    }
    אם(s.length()==0) {
        החזר *هذا;
    }
    حرف c;
    אם(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    החזר *هذا;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    حرف c;
    אם(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    החזר *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#אם (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    אם(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    החזר dirSepChar;
}

U_NAMESPACE_END
