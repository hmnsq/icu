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

#شامل <cstdlib>

#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uinvchar.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    بازگشت *این;
}

نویسه *CharString::cloneData(UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    نویسه *p = static_cast<نویسه *>(uprv_malloc(len + 1));
    اگر (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    بازگشت p;
}

int32_t CharString::extract(نویسه *dest, int32_t capacity, UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت len; }
    اگر (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت len;
    }
    ثابت نویسه *src = buffer.getAlias();
    اگر (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    بازگشت u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ثابت CharString &s, UErrorCode &errorCode) {
    اگر(U_SUCCESS(errorCode) && این!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    بازگشت *این;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) {
        بازگشت *این;
    }
    len = 0;
    append(s, errorCode);
    بازگشت *این;
}

int32_t CharString::lastIndexOf(نویسه c) ثابت {
    برای(int32_t i=len; i>0;) {
        اگر(buffer[--i]==c) {
            بازگشت i;
        }
    }
    بازگشت -1;
}

منطقی CharString::contains(StringPiece s) ثابت {
    اگر (s.empty()) { بازگشت نادرست; }
    ثابت نویسه *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    برای (int32_t i = 0; i <= lastStart; ++i) {
        اگر (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            بازگشت درست;
        }
    }
    بازگشت نادرست;
}

CharString &CharString::truncate(int32_t newLength) {
    اگر(newLength<0) {
        newLength=0;
    }
    اگر(newLength<len) {
        buffer[len=newLength]=0;
    }
    بازگشت *این;
}

CharString &CharString::append(نویسه c, UErrorCode &errorCode) {
    اگر(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    بازگشت *این;
}

CharString &CharString::append(ثابت نویسه *s, int32_t sLength, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت *این;
    }
    اگر(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت *این;
    }
    اگر(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(sLength>0) {
        اگر(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            اگر(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } وگرنه {
                buffer[len+=sLength]=0;
            }
        } وگرنه اگر(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            بازگشت append(CharString(s, sLength, errorCode), errorCode);
        } وگرنه اگر(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    بازگشت *این;
}

CharString &CharString::append_عدد(int64_t number, UErrorCode &status) {
    اگر (number < 0) {
        این->append('-', status);
        اگر (U_FAILURE(status)) {
            بازگشت *این;
        }
    }

    اگر (number == 0) {
        این->append('0', status);
        بازگشت *این;
    }

    int32_t رقم_Len = 0;
    وقتی (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        این->append(std::abs(residue) + '0', status);
        رقم_Len++;
        اگر (U_FAILURE(status)) {
            بازگشت *این;
        }
    }

    int32_t start = این->length() - رقم_Len, end = این->length() - 1;
    وقتی(start < end) {
        std::swap(این->data()[start++], این->data()[end--]);
    }

    بازگشت *این;
}

نویسه *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        resultCapacity=0;
        بازگشت nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    اگر(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        بازگشت buffer.getAlias()+len;
    }
    اگر(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        بازگشت buffer.getAlias()+len;
    }
    resultCapacity=0;
    بازگشت nullptr;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UErrorCode &errorCode) {
    بازگشت appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت *این;
    }
    اگر (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        بازگشت *این;
    }
    اگر(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    بازگشت *این;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    اگر(capacity>buffer.getCapacity()) {
        اگر(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        اگر( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت *این;
    }
    اگر(s.length()==0) {
        بازگشت *این;
    }
    نویسه c;
    اگر(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    بازگشت *این;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    نویسه c;
    اگر(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    بازگشت *این;
}

نویسه CharString::getDirSepChar() ثابت {
    نویسه dirSepChar = U_FILE_SEP_CHAR;
#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    اگر(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    بازگشت dirSepChar;
}

U_NAMESPACE_END
