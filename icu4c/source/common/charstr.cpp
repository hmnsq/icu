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

#അന്തര്ഭാവ <cstdlib>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    തിരികെ *ഇത്;
}

അക്ഷരം *CharString::cloneData(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    അക്ഷരം *p = static_cast<അക്ഷരം *>(uprv_malloc(len + 1));
    എങ്കിൽ (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    തിരികെ p;
}

int32_t CharString::extract(അക്ഷരം *dest, int32_t capacity, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ len; }
    എങ്കിൽ (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ len;
    }
    സ്ഥിരം അക്ഷരം *src = buffer.getAlias();
    എങ്കിൽ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    തിരികെ u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(സ്ഥിരം CharString &s, UErrorCode &errorCode) {
    എങ്കിൽ(U_SUCCESS(errorCode) && ഇത്!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    തിരികെ *ഇത്;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    len = 0;
    append(s, errorCode);
    തിരികെ *ഇത്;
}

int32_t CharString::lastIndexOf(അക്ഷരം c) സ്ഥിരം {
    വേണ്ടി(int32_t i=len; i>0;) {
        എങ്കിൽ(buffer[--i]==c) {
            തിരികെ i;
        }
    }
    തിരികെ -1;
}

തർക്കം CharString::contains(StringPiece s) സ്ഥിരം {
    എങ്കിൽ (s.empty()) { തിരികെ അസത്യം; }
    സ്ഥിരം അക്ഷരം *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    വേണ്ടി (int32_t i = 0; i <= lastStart; ++i) {
        എങ്കിൽ (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            തിരികെ സത്യം;
        }
    }
    തിരികെ അസത്യം;
}

CharString &CharString::truncate(int32_t newLength) {
    എങ്കിൽ(newLength<0) {
        newLength=0;
    }
    എങ്കിൽ(newLength<len) {
        buffer[len=newLength]=0;
    }
    തിരികെ *ഇത്;
}

CharString &CharString::append(അക്ഷരം c, UErrorCode &errorCode) {
    എങ്കിൽ(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    തിരികെ *ഇത്;
}

CharString &CharString::append(സ്ഥിരം അക്ഷരം *s, int32_t sLength, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    എങ്കിൽ(sLength>0) {
        എങ്കിൽ(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            എങ്കിൽ(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } അല്ലെങ്കിൽ {
                buffer[len+=sLength]=0;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            തിരികെ append(CharString(s, sLength, errorCode), errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    തിരികെ *ഇത്;
}

CharString &CharString::append_സംഖ്യ(int64_t number, UErrorCode &status) {
    എങ്കിൽ (number < 0) {
        ഇത്->append('-', status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ *ഇത്;
        }
    }

    എങ്കിൽ (number == 0) {
        ഇത്->append('0', status);
        തിരികെ *ഇത്;
    }

    int32_t അക്കം_Len = 0;
    ആകുമ്പോൾ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ഇത്->append(std::abs(residue) + '0', status);
        അക്കം_Len++;
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ *ഇത്;
        }
    }

    int32_t start = ഇത്->length() - അക്കം_Len, end = ഇത്->length() - 1;
    ആകുമ്പോൾ(start < end) {
        std::swap(ഇത്->data()[start++], ഇത്->data()[end--]);
    }

    തിരികെ *ഇത്;
}

അക്ഷരം *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        resultCapacity=0;
        തിരികെ nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    എങ്കിൽ(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        തിരികെ buffer.getAlias()+len;
    }
    എങ്കിൽ(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        തിരികെ buffer.getAlias()+len;
    }
    resultCapacity=0;
    തിരികെ nullptr;
}

CharString &CharString::appendInvariantChars(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) {
    തിരികെ appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(സ്ഥിരം char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    തിരികെ *ഇത്;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(capacity>buffer.getCapacity()) {
        എങ്കിൽ(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        എങ്കിൽ( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(s.length()==0) {
        തിരികെ *ഇത്;
    }
    അക്ഷരം c;
    എങ്കിൽ(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    തിരികെ *ഇത്;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    അക്ഷരം c;
    എങ്കിൽ(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    തിരികെ *ഇത്;
}

അക്ഷരം CharString::getDirSepChar() സ്ഥിരം {
    അക്ഷരം dirSepChar = U_FILE_SEP_CHAR;
#എങ്കിൽ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    എങ്കിൽ(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    തിരികെ dirSepChar;
}

U_NAMESPACE_END
