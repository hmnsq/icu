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
    أرجع *هذا;
}

حرف *CharString::cloneData(UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع nullptr; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    𐤀𐤌 (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    أرجع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع len; }
    𐤀𐤌 (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع len;
    }
    ثابت حرف *src = buffer.getAlias();
    𐤀𐤌 (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    أرجع u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ثابت CharString &s, UErrorCode &errorCode) {
    𐤀𐤌(U_SUCCESS(errorCode) && هذا!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    أرجع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع *هذا;
    }
    len = 0;
    append(s, errorCode);
    أرجع *هذا;
}

int32_t CharString::lastIndexOf(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        𐤀𐤌(buffer[--i]==c) {
            أرجع i;
        }
    }
    أرجع -1;
}

منطقي CharString::contains(StringPiece s) ثابت {
    𐤀𐤌 (s.empty()) { أرجع خطأ; }
    ثابت حرف *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    لكل (int32_t i = 0; i <= lastStart; ++i) {
        𐤀𐤌 (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            أرجع true;
        }
    }
    أرجع خطأ;
}

CharString &CharString::truncate(int32_t newLength) {
    𐤀𐤌(newLength<0) {
        newLength=0;
    }
    𐤀𐤌(newLength<len) {
        buffer[len=newLength]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(حرف c, UErrorCode &errorCode) {
    𐤀𐤌(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t sLength, UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع *هذا;
    }
    𐤀𐤌(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    𐤀𐤌(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    𐤀𐤌(sLength>0) {
        𐤀𐤌(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            𐤀𐤌(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } وإلا {
                buffer[len+=sLength]=0;
            }
        } وإلا 𐤀𐤌(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            أرجع append(CharString(s, sLength, errorCode), errorCode);
        } وإلا 𐤀𐤌(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    أرجع *هذا;
}

CharString &CharString::appendNumber(int64_t number, UErrorCode &status) {
    𐤀𐤌 (number < 0) {
        هذا->append('-', status);
        𐤀𐤌 (U_FAILURE(status)) {
            أرجع *هذا;
        }
    }

    𐤀𐤌 (number == 0) {
        هذا->append('0', status);
        أرجع *هذا;
    }

    int32_t numLen = 0;
    طالما (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::abs(residue) + '0', status);
        numLen++;
        𐤀𐤌 (U_FAILURE(status)) {
            أرجع *هذا;
        }
    }

    int32_t start = هذا->length() - numLen, end = هذا->length() - 1;
    طالما(start < end) {
        std::swap(هذا->data()[start++], هذا->data()[end--]);
    }

    أرجع *هذا;
}

حرف *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        resultCapacity=0;
        أرجع nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    𐤀𐤌(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        أرجع buffer.getAlias()+len;
    }
    𐤀𐤌(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        أرجع buffer.getAlias()+len;
    }
    resultCapacity=0;
    أرجع nullptr;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UErrorCode &errorCode) {
    أرجع appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع *هذا;
    }
    𐤀𐤌 (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        أرجع *هذا;
    }
    𐤀𐤌(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    أرجع *هذا;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    𐤀𐤌(capacity>buffer.getCapacity()) {
        𐤀𐤌(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        𐤀𐤌( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع خطأ;
        }
    }
    أرجع true;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع *هذا;
    }
    𐤀𐤌(s.length()==0) {
        أرجع *هذا;
    }
    حرف c;
    𐤀𐤌(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    أرجع *هذا;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    حرف c;
    𐤀𐤌(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    أرجع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#𐤀𐤌 (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    𐤀𐤌(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    أرجع dirSepChar;
}

U_NAMESPACE_END
