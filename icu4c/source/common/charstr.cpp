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

#ಅನ್ತರ್ಭಾವ <cstdlib>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ವರ್ಣ *CharString::cloneData(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ವರ್ಣ *p = static_cast<ವರ್ಣ *>(uprv_malloc(len + 1));
    ಯದಿ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ಪ್ರತ್ಯಾಗಮ p;
}

int32_t CharString::extract(ವರ್ಣ *dest, int32_t capacity, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ len; }
    ಯದಿ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ len;
    }
    ಧ್ರುವ ವರ್ಣ *src = buffer.getAlias();
    ಯದಿ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ಪ್ರತ್ಯಾಗಮ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ಧ್ರುವ CharString &s, UErrorCode &errorCode) {
    ಯದಿ(U_SUCCESS(errorCode) && ಏತತ್!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    len = 0;
    append(s, errorCode);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

int32_t CharString::lastIndexOf(ವರ್ಣ c) ಧ್ರುವ {
    ಪ್ರತಿ(int32_t i=len; i>0;) {
        ಯದಿ(buffer[--i]==c) {
            ಪ್ರತ್ಯಾಗಮ i;
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

ತಾರ್ಕಿಕ CharString::contains(StringPiece s) ಧ್ರುವ {
    ಯದಿ (s.empty()) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಧ್ರುವ ವರ್ಣ *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    ಪ್ರತಿ (int32_t i = 0; i <= lastStart; ++i) {
        ಯದಿ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

CharString &CharString::truncate(int32_t newLength) {
    ಯದಿ(newLength<0) {
        newLength=0;
    }
    ಯದಿ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

CharString &CharString::append(ವರ್ಣ c, UErrorCode &errorCode) {
    ಯದಿ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

CharString &CharString::append(ಧ್ರುವ ವರ್ಣ *s, int32_t sLength, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ಯದಿ(sLength>0) {
        ಯದಿ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ಯದಿ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ಅನ್ಯಥಾ {
                buffer[len+=sLength]=0;
            }
        } ಅನ್ಯಥಾ ಯದಿ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ಪ್ರತ್ಯಾಗಮ append(CharString(s, sLength, errorCode), errorCode);
        } ಅನ್ಯಥಾ ಯದಿ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

CharString &CharString::append_ಸಂಖ್ಯೆ(int64_t number, UErrorCode &status) {
    ಯದಿ (number < 0) {
        ಏತತ್->append('-', status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }

    ಯದಿ (number == 0) {
        ಏತತ್->append('0', status);
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    int32_t ಅಂಕೆ_Len = 0;
    ಯಾವತ್ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ಏತತ್->append(std::abs(residue) + '0', status);
        ಅಂಕೆ_Len++;
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }

    int32_t start = ಏತತ್->length() - ಅಂಕೆ_Len, end = ಏತತ್->length() - 1;
    ಯಾವತ್(start < end) {
        std::swap(ಏತತ್->data()[start++], ಏತತ್->data()[end--]);
    }

    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ವರ್ಣ *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ಯದಿ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ಪ್ರತ್ಯಾಗಮ buffer.getAlias()+len;
    }
    ಯದಿ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ಪ್ರತ್ಯಾಗಮ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

CharString &CharString::appendInvariantChars(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) {
    ಪ್ರತ್ಯಾಗಮ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ಧ್ರುವ char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(capacity>buffer.getCapacity()) {
        ಯದಿ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ಯದಿ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(s.length()==0) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ವರ್ಣ c;
    ಯದಿ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    ವರ್ಣ c;
    ಯದಿ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ವರ್ಣ CharString::getDirSepChar() ಧ್ರುವ {
    ವರ್ಣ dirSepChar = U_FILE_SEP_CHAR;
#ಯದಿ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ಯದಿ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ಪ್ರತ್ಯಾಗಮ dirSepChar;
}

U_NAMESPACE_END
