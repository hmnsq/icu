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

#অন্তর্ভাৱ <cstdlib>

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "uinvchar.h"
#অন্তর্ভাৱ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ঘূৰাই *এতত্;
}

ৱর্ণ *CharString::cloneData(UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    ৱর্ণ *p = static_cast<ৱর্ণ *>(uprv_malloc(len + 1));
    যদি (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    ঘূৰাই p;
}

int32_t CharString::extract(ৱর্ণ *dest, int32_t capacity, UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই len; }
    যদি (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই len;
    }
    ধ্রুৱ ৱর্ণ *src = buffer.getAlias();
    যদি (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ঘূৰাই u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ধ্রুৱ CharString &s, UErrorCode &errorCode) {
    যদি(U_SUCCESS(errorCode) && এতত্!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    ঘূৰাই *এতত্;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    len = 0;
    append(s, errorCode);
    ঘূৰাই *এতত্;
}

int32_t CharString::lastIndexOf(ৱর্ণ c) ধ্রুৱ {
    বাবে(int32_t i=len; i>0;) {
        যদি(buffer[--i]==c) {
            ঘূৰাই i;
        }
    }
    ঘূৰাই -1;
}

তার্কিক CharString::contains(StringPiece s) ধ্রুৱ {
    যদি (s.empty()) { ঘূৰাই মিছা; }
    ধ্রুৱ ৱর্ণ *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    বাবে (int32_t i = 0; i <= lastStart; ++i) {
        যদি (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            ঘূৰাই সঁচা;
        }
    }
    ঘূৰাই মিছা;
}

CharString &CharString::truncate(int32_t newLength) {
    যদি(newLength<0) {
        newLength=0;
    }
    যদি(newLength<len) {
        buffer[len=newLength]=0;
    }
    ঘূৰাই *এতত্;
}

CharString &CharString::append(ৱর্ণ c, UErrorCode &errorCode) {
    যদি(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    ঘূৰাই *এতত্;
}

CharString &CharString::append(ধ্রুৱ ৱর্ণ *s, int32_t sLength, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    যদি(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই *এতত্;
    }
    যদি(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    যদি(sLength>0) {
        যদি(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            যদি(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } অন্যথা {
                buffer[len+=sLength]=0;
            }
        } অন্যথা যদি(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ঘূৰাই append(CharString(s, sLength, errorCode), errorCode);
        } অন্যথা যদি(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    ঘূৰাই *এতত্;
}

CharString &CharString::append_সংখ্যা(int64_t number, UErrorCode &status) {
    যদি (number < 0) {
        এতত্->append('-', status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই *এতত্;
        }
    }

    যদি (number == 0) {
        এতত্->append('0', status);
        ঘূৰাই *এতত্;
    }

    int32_t অঙ্ক_Len = 0;
    যেতিয়ালৈ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        এতত্->append(std::abs(residue) + '0', status);
        অঙ্ক_Len++;
        যদি (U_FAILURE(status)) {
            ঘূৰাই *এতত্;
        }
    }

    int32_t start = এতত্->length() - অঙ্ক_Len, end = এতত্->length() - 1;
    যেতিয়ালৈ(start < end) {
        std::swap(এতত্->data()[start++], এতত্->data()[end--]);
    }

    ঘূৰাই *এতত্;
}

ৱর্ণ *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        resultCapacity=0;
        ঘূৰাই nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    যদি(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        ঘূৰাই buffer.getAlias()+len;
    }
    যদি(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        ঘূৰাই buffer.getAlias()+len;
    }
    resultCapacity=0;
    ঘূৰাই nullptr;
}

CharString &CharString::appendInvariantChars(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) {
    ঘূৰাই appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ধ্রুৱ char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    যদি (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ঘূৰাই *এতত্;
    }
    যদি(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    ঘূৰাই *এতত্;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    যদি(capacity>buffer.getCapacity()) {
        যদি(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        যদি( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই সঁচা;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    যদি(s.length()==0) {
        ঘূৰাই *এতত্;
    }
    ৱর্ণ c;
    যদি(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ঘূৰাই *এতত্;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    ৱর্ণ c;
    যদি(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ঘূৰাই *এতত্;
}

ৱর্ণ CharString::getDirSepChar() ধ্রুৱ {
    ৱর্ণ dirSepChar = U_FILE_SEP_CHAR;
#যদি (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    যদি(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ঘূৰাই dirSepChar;
}

U_NAMESPACE_END
