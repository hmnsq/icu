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

#अन्तर्भाव <cstdlib>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : buffer(std::move(src.buffer)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::चालक=(CharString&& src) noexcept {
    buffer = std::move(src.buffer);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    परत *हे;
}

अक्षर *CharString::cloneData(UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    अक्षर *p = static_cast<अक्षर *>(uprv_malloc(len + 1));
    जर (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    uprv_memcpy(p, buffer.getAlias(), len + 1);
    परत p;
}

int32_t CharString::extract(अक्षर *dest, int32_t capacity, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत len; }
    जर (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत len;
    }
    स्थिरांक अक्षर *src = buffer.getAlias();
    जर (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    परत u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(स्थिरांक CharString &s, UErrorCode &errorCode) {
    जर(U_SUCCESS(errorCode) && हे!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(buffer.getAlias(), s.buffer.getAlias(), len+1);
    }
    परत *हे;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) {
        परत *हे;
    }
    len = 0;
    append(s, errorCode);
    परत *हे;
}

int32_t CharString::lastIndexOf(अक्षर c) स्थिरांक {
    साठी(int32_t i=len; i>0;) {
        जर(buffer[--i]==c) {
            परत i;
        }
    }
    परत -1;
}

तार्किक CharString::contains(StringPiece s) स्थिरांक {
    जर (s.empty()) { परत असत्य; }
    स्थिरांक अक्षर *p = buffer.getAlias();
    int32_t lastStart = len - s.length();
    साठी (int32_t i = 0; i <= lastStart; ++i) {
        जर (uprv_memcmp(p + i, s.data(), s.length()) == 0) {
            परत सत्य;
        }
    }
    परत असत्य;
}

CharString &CharString::truncate(int32_t newLength) {
    जर(newLength<0) {
        newLength=0;
    }
    जर(newLength<len) {
        buffer[len=newLength]=0;
    }
    परत *हे;
}

CharString &CharString::append(अक्षर c, UErrorCode &errorCode) {
    जर(ensureCapacity(len+2, 0, errorCode)) {
        buffer[len++]=c;
        buffer[len]=0;
    }
    परत *हे;
}

CharString &CharString::append(स्थिरांक अक्षर *s, int32_t sLength, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत *हे;
    }
    जर(sLength<-1 || (s==nullptr && sLength!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    जर(sLength<0) {
        sLength= static_cast<int32_t>(uprv_strlen(s));
    }
    जर(sLength>0) {
        जर(s==(buffer.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            जर(sLength>=(buffer.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } अन्यथा {
                buffer[len+=sLength]=0;
            }
        } अन्यथा जर(buffer.getAlias()<=s && s<(buffer.getAlias()+len) &&
                  sLength>=(buffer.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            परत append(CharString(s, sLength, errorCode), errorCode);
        } अन्यथा जर(ensureCapacity(len+sLength+1, 0, errorCode)) {
            uprv_memcpy(buffer.getAlias()+len, s, sLength);
            buffer[len+=sLength]=0;
        }
    }
    परत *हे;
}

CharString &CharString::append_संख्या(int64_t number, UErrorCode &status) {
    जर (number < 0) {
        हे->append('-', status);
        जर (U_FAILURE(status)) {
            परत *हे;
        }
    }

    जर (number == 0) {
        हे->append('0', status);
        परत *हे;
    }

    int32_t अंक_Len = 0;
    पर्यंत (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        हे->append(std::असल(residue) + '0', status);
        अंक_Len++;
        जर (U_FAILURE(status)) {
            परत *हे;
        }
    }

    int32_t start = हे->length() - अंक_Len, end = हे->length() - 1;
    पर्यंत(start < end) {
        std::swap(हे->data()[start++], हे->data()[end--]);
    }

    परत *हे;
}

अक्षर *CharString::getAppendBuffer(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &resultCapacity,
                                  UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        resultCapacity=0;
        परत nullptr;
    }
    int32_t appendCapacity=buffer.getCapacity()-len-1;  // -1 for NUL
    जर(appendCapacity>=minCapacity) {
        resultCapacity=appendCapacity;
        परत buffer.getAlias()+len;
    }
    जर(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        resultCapacity=buffer.getCapacity()-len-1;
        परत buffer.getAlias()+len;
    }
    resultCapacity=0;
    परत nullptr;
}

CharString &CharString::appendInvariantChars(स्थिरांक UnicodeString &s, UErrorCode &errorCode) {
    परत appendInvariantChars(s.getBuffer(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(स्थिरांक char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत *हे;
    }
    जर (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        परत *हे;
    }
    जर(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, buffer.getAlias()+len, ucharsLen);
        len += ucharsLen;
        buffer[len] = 0;
    }
    परत *हे;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    जर(capacity>buffer.getCapacity()) {
        जर(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+buffer.getCapacity();
        }
        जर( (desiredCapacityHint<=capacity || buffer.resize(desiredCapacityHint, len+1)==nullptr) &&
            buffer.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत असत्य;
        }
    }
    परत सत्य;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत *हे;
    }
    जर(s.length()==0) {
        परत *हे;
    }
    अक्षर c;
    जर(len>0 && (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    परत *हे;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    अक्षर c;
    जर(U_SUCCESS(errorCode) && len>0 &&
            (c=buffer[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    परत *हे;
}

अक्षर CharString::getDirSepChar() स्थिरांक {
    अक्षर dirSepChar = U_FILE_SEP_CHAR;
#जर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    जर(len>0 && !uprv_strchr(data(), U_FILE_SEP_CHAR) && uprv_strchr(data(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    परत dirSepChar;
}

U_NAMESPACE_END
