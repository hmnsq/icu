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
    إرجاع *هذا;
}

حرف *CharString::cloneData(UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { إرجاع nullptr; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    إذا (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    إرجاع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { إرجاع len; }
    إذا (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع len;
    }
    ثابت حرف *src = buffer.getAlias();
    إذا (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    إرجاع u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ثابت CharString &s, UErrorCode &errorCode) {
    إذا(U_SUCCESS(errorCode) && هذا!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    إرجاع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) {
        إرجاع *هذا;
    }
    len = 0;
    append(s, errorCode);
    إرجاع *هذا;
}

int32_t CharString::lastIndexOf(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        إذا(buffer[--i]==c) {
            إرجاع i;
        }
    }
    إرجاع -1;
}

منطقي CharString::contains(StringPiece s) ثابت {
    إذا (s.empty()) { إرجاع خطأ; }
    ثابت حرف *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    لكل (int32_t i = 0; i <= lastStart; ++i) {
        إذا (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            إرجاع true;
        }
    }
    إرجاع خطأ;
}

CharString &CharString::truncate(int32_t newLength) {
    إذا(newLength<0) {
        newLength=0;
    }
    إذا(newLength<len) {
        buffer[len=newLength]=0;
    }
    إرجاع *هذا;
}

CharString &CharString::append(حرف c, UErrorCode &errorCode) {
    إذا(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    إرجاع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t sLength, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع *هذا;
    }
    إذا(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع *هذا;
    }
    إذا(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(sLength>0) {
        إذا(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            إذا(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } وإلا {
                buffer[len+=sLength]=0;
            }
        } وإلا إذا(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            إرجاع append(CharString(s, sLength, errorCode), errorCode);
        } وإلا إذا(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    إرجاع *هذا;
}

CharString &CharString::append_العدد(int64_t number, UErrorCode &status) {
    إذا (number < 0) {
        هذا->append('-', status);
        إذا (U_FAILURE(status)) {
            إرجاع *هذا;
        }
    }

    إذا (number == 0) {
        هذا->append('0', status);
        إرجاع *هذا;
    }

    int32_t رقم_Len = 0;
    بينما (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::abs(residue) + '0', status);
        رقم_Len++;
        إذا (U_FAILURE(status)) {
            إرجاع *هذا;
        }
    }

    int32_t start = هذا->length() - رقم_Len, end = هذا->length() - 1;
    بينما(start < end) {
        std::swap(هذا->data()[start++], هذا->data()[end--]);
    }

    إرجاع *هذا;
}

حرف *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        resultCapacity=0;
        إرجاع nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    إذا(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        إرجاع buffer.getAlias()+len;
    }
    إذا(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        إرجاع buffer.getAlias()+len;
    }
    resultCapacity=0;
    إرجاع nullptr;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UErrorCode &errorCode) {
    إرجاع appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع *هذا;
    }
    إذا (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        إرجاع *هذا;
    }
    إذا(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    إرجاع *هذا;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع خطأ;
    }
    إذا(capacity>buffer.getCapacity()) {
        إذا(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        إذا( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            إرجاع خطأ;
        }
    }
    إرجاع true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع *هذا;
    }
    إذا(s.length()==0) {
        إرجاع *هذا;
    }
    حرف c;
    إذا(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    إرجاع *هذا;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    حرف c;
    إذا(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    إرجاع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    إذا(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    إرجاع dirSepChar;
}

U_NAMESPACE_END
