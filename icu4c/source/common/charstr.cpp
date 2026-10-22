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

#અન્તર્ભાવ <cstdlib>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    પાછા *આ;
}

અક્ષર *CharString::cloneData(UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    અક્ષર *p = static_cast<અક્ષર *>(uprv_malloc(len + 1));
    જો (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    પાછા p;
}

int32_t CharString::extract(અક્ષર *dest, int32_t capacity, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા len; }
    જો (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા len;
    }
    અચલ અક્ષર *src = buffer.getAlias();
    જો (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    પાછા u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(અચલ CharString &s, UErrorCode &errorCode) {
    જો(U_SUCCESS(errorCode) && આ!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    પાછા *આ;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    len = 0;
    append(s, errorCode);
    પાછા *આ;
}

int32_t CharString::lastIndexOf(અક્ષર c) અચલ {
    માટે(int32_t i=len; i>0;) {
        જો(buffer[--i]==c) {
            પાછા i;
        }
    }
    પાછા -1;
}

તાર્કિક CharString::contains(StringPiece s) અચલ {
    જો (s.empty()) { પાછા ખોટું; }
    અચલ અક્ષર *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    માટે (int32_t i = 0; i <= lastStart; ++i) {
        જો (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            પાછા સાચું;
        }
    }
    પાછા ખોટું;
}

CharString &CharString::truncate(int32_t newLength) {
    જો(newLength<0) {
        newLength=0;
    }
    જો(newLength<len) {
        buffer[len=newLength]=0;
    }
    પાછા *આ;
}

CharString &CharString::append(અક્ષર c, UErrorCode &errorCode) {
    જો(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    પાછા *આ;
}

CharString &CharString::append(અચલ અક્ષર *s, int32_t sLength, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    જો(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    જો(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    જો(sLength>0) {
        જો(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            જો(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } નહીંતર {
                buffer[len+=sLength]=0;
            }
        } નહીંતર જો(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            પાછા append(CharString(s, sLength, errorCode), errorCode);
        } નહીંતર જો(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    પાછા *આ;
}

CharString &CharString::append_સંખ્યા(int64_t number, UErrorCode &status) {
    જો (number < 0) {
        આ->append('-', status);
        જો (U_FAILURE(status)) {
            પાછા *આ;
        }
    }

    જો (number == 0) {
        આ->append('0', status);
        પાછા *આ;
    }

    int32_t અંક_Len = 0;
    જ્યારે (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        આ->append(std::abs(residue) + '0', status);
        અંક_Len++;
        જો (U_FAILURE(status)) {
            પાછા *આ;
        }
    }

    int32_t start = આ->length() - અંક_Len, end = આ->length() - 1;
    જ્યારે(start < end) {
        std::swap(આ->data()[start++], આ->data()[end--]);
    }

    પાછા *આ;
}

અક્ષર *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        resultCapacity=0;
        પાછા nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    જો(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        પાછા buffer.getAlias()+len;
    }
    જો(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        પાછા buffer.getAlias()+len;
    }
    resultCapacity=0;
    પાછા nullptr;
}

CharString &CharString::appendInvariantChars(અચલ UnicodeString &s, UErrorCode &errorCode) {
    પાછા appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(અચલ char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    જો (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        પાછા *આ;
    }
    જો(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    પાછા *આ;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    જો(capacity>buffer.getCapacity()) {
        જો(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        જો( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    જો(s.length()==0) {
        પાછા *આ;
    }
    અક્ષર c;
    જો(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    પાછા *આ;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    અક્ષર c;
    જો(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    પાછા *આ;
}

અક્ષર CharString::getDirSepChar() અચલ {
    અક્ષર dirSepChar = U_FILE_SEP_CHAR;
#જો (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    જો(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    પાછા dirSepChar;
}

U_NAMESPACE_END
