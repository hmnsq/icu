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
    ܦܢܐ *هذا;
}

حرف *CharString::cloneData(UErrorCode &errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ nullptr; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    ܐܢ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ܦܢܐ p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UErrorCode &errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ len; }
    ܐܢ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ len;
    }
    ثابت حرف *src = buffer.getAlias();
    ܐܢ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ܦܢܐ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ثابت CharString &s, UErrorCode &errorCode) {
    ܐܢ(U_SUCCESS(errorCode) && هذا!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ܦܢܐ *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ *هذا;
    }
    len = 0;
    append(s, errorCode);
    ܦܢܐ *هذا;
}

int32_t CharString::lastIndexOf(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        ܐܢ(buffer[--i]==c) {
            ܦܢܐ i;
        }
    }
    ܦܢܐ -1;
}

منطقي CharString::contains(StringPiece s) ثابت {
    ܐܢ (s.empty()) { ܦܢܐ ܕܓܠܐ; }
    ثابت حرف *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    لكل (int32_t i = 0; i <= lastStart; ++i) {
        ܐܢ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ܦܢܐ ܫܪܪܐ;
        }
    }
    ܦܢܐ ܕܓܠܐ;
}

CharString &CharString::truncate(int32_t newLength) {
    ܐܢ(newLength<0) {
        newLength=0;
    }
    ܐܢ(newLength<len) {
        buffer[len=newLength]=0;
    }
    ܦܢܐ *هذا;
}

CharString &CharString::append(حرف c, UErrorCode &errorCode) {
    ܐܢ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ܦܢܐ *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t sLength, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ *هذا;
    }
    ܐܢ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ *هذا;
    }
    ܐܢ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    ܐܢ(sLength>0) {
        ܐܢ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ܐܢ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } ܐܚܪܢܐ {
                buffer[len+=sLength]=0;
            }
        } ܐܚܪܢܐ ܐܢ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ܦܢܐ append(CharString(s, sLength, errorCode), errorCode);
        } ܐܚܪܢܐ ܐܢ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ܦܢܐ *هذا;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    ܐܢ (number < 0) {
        هذا->append('-', status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ *هذا;
        }
    }

    ܐܢ (number == 0) {
        هذا->append('0', status);
        ܦܢܐ *هذا;
    }

    int32_t numLen = 0;
    ܟܡܐ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::abs(residue) + '0', status);
        numLen++;
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ *هذا;
        }
    }

    int32_t start = هذا->length() - numLen, end = هذا->length() - 1;
    ܟܡܐ(start < end) {
        std::swap(هذا->data()[start++], هذا->data()[end--]);
    }

    ܦܢܐ *هذا;
}

حرف *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ܦܢܐ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    ܐܢ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ܦܢܐ buffer.getAlias()+len;
    }
    ܐܢ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ܦܢܐ buffer.getAlias()+len;
    }
    resultCapacity=0;
    ܦܢܐ nullptr;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UErrorCode &errorCode) {
    ܦܢܐ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ *هذا;
    }
    ܐܢ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ܦܢܐ *هذا;
    }
    ܐܢ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ܦܢܐ *هذا;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ(capacity>buffer.getCapacity()) {
        ܐܢ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        ܐܢ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ *هذا;
    }
    ܐܢ(s.length()==0) {
        ܦܢܐ *هذا;
    }
    حرف c;
    ܐܢ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ܦܢܐ *هذا;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    حرف c;
    ܐܢ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ܦܢܐ *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#ܐܢ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ܐܢ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ܦܢܐ dirSepChar;
}

U_NAMESPACE_END
