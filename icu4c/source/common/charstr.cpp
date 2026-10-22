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

#অন্তর্ভাব <cstdlib>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : বাফার(std::move(src.বাফার)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::operator=(CharString&& src) noexcept {
    বাফার = std::move(src.বাফার);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ফেরত *এই;
}

অক্ষর *CharString::clone_তথ্য(UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    অক্ষর *p = static_cast<অক্ষর *>(uprv_malloc(len + 1));
    যদি (p == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(p, বাফার.getAlias(), len + 1);
    ফেরত p;
}

int32_t CharString::extract(অক্ষর *dest, int32_t capacity, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত len; }
    যদি (capacity < 0 || (capacity > 0 && dest == nullptr)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত len;
    }
    ধ্রুবক অক্ষর *src = বাফার.getAlias();
    যদি (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ফেরত u_terminateChars(dest, capacity, len, &errorCode);
}

CharString &CharString::copyFrom(ধ্রুবক CharString &s, UErrorCode &errorCode) {
    যদি(U_SUCCESS(errorCode) && এই!=&s && ensureCapacity(s.len+1, 0, errorCode)) {
        len=s.len;
        uprv_memcpy(বাফার.getAlias(), s.বাফার.getAlias(), len+1);
    }
    ফেরত *এই;
}

CharString &CharString::copyFrom(StringPiece s, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত *এই;
    }
    len = 0;
    append(s, errorCode);
    ফেরত *এই;
}

int32_t CharString::last_সূচক_Of(অক্ষর c) ধ্রুবক {
    জন্য(int32_t i=len; i>0;) {
        যদি(বাফার[--i]==c) {
            ফেরত i;
        }
    }
    ফেরত -1;
}

যৌক্তিক CharString::contains(StringPiece s) ধ্রুবক {
    যদি (s.empty()) { ফেরত মিথ্যা; }
    ধ্রুবক অক্ষর *p = বাফার.getAlias();
    int32_t lastStart = len - s.length();
    জন্য (int32_t i = 0; i <= lastStart; ++i) {
        যদি (uprv_memcmp(p + i, s.তথ্য(), s.length()) == 0) {
            ফেরত সত্য;
        }
    }
    ফেরত মিথ্যা;
}

CharString &CharString::truncate(int32_t new_দৈর্ঘ্য) {
    যদি(new_দৈর্ঘ্য<0) {
        new_দৈর্ঘ্য=0;
    }
    যদি(new_দৈর্ঘ্য<len) {
        বাফার[len=new_দৈর্ঘ্য]=0;
    }
    ফেরত *এই;
}

CharString &CharString::append(অক্ষর c, UErrorCode &errorCode) {
    যদি(ensureCapacity(len+2, 0, errorCode)) {
        বাফার[len++]=c;
        বাফার[len]=0;
    }
    ফেরত *এই;
}

CharString &CharString::append(ধ্রুবক অক্ষর *s, int32_t s_দৈর্ঘ্য, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত *এই;
    }
    যদি(s_দৈর্ঘ্য<-1 || (s==nullptr && s_দৈর্ঘ্য!=0)) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত *এই;
    }
    যদি(s_দৈর্ঘ্য<0) {
        s_দৈর্ঘ্য= static_cast<int32_t>(uprv_strlen(s));
    }
    যদি(s_দৈর্ঘ্য>0) {
        যদি(s==(বাফার.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            যদি(s_দৈর্ঘ্য>=(বাফার.getCapacity()-len)) {
                // The caller wrote too much.
                errorCode=U_INTERNAL_PROGRAM_ERROR;
            } অন্যথা {
                বাফার[len+=s_দৈর্ঘ্য]=0;
            }
        } অন্যথা যদি(বাফার.getAlias()<=s && s<(বাফার.getAlias()+len) &&
                  s_দৈর্ঘ্য>=(বাফার.getCapacity()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ফেরত append(CharString(s, s_দৈর্ঘ্য, errorCode), errorCode);
        } অন্যথা যদি(ensureCapacity(len+s_দৈর্ঘ্য+1, 0, errorCode)) {
            uprv_memcpy(বাফার.getAlias()+len, s, s_দৈর্ঘ্য);
            বাফার[len+=s_দৈর্ঘ্য]=0;
        }
    }
    ফেরত *এই;
}

CharString &CharString::append_সংখ্যা(int64_t number, UErrorCode &status) {
    যদি (number < 0) {
        এই->append('-', status);
        যদি (U_FAILURE(status)) {
            ফেরত *এই;
        }
    }

    যদি (number == 0) {
        এই->append('0', status);
        ফেরত *এই;
    }

    int32_t অঙ্ক_Len = 0;
    যখন (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        এই->append(std::abs(residue) + '0', status);
        অঙ্ক_Len++;
        যদি (U_FAILURE(status)) {
            ফেরত *এই;
        }
    }

    int32_t start = এই->length() - অঙ্ক_Len, end = এই->length() - 1;
    যখন(start < end) {
        std::swap(এই->তথ্য()[start++], এই->তথ্য()[end--]);
    }

    ফেরত *এই;
}

অক্ষর *CharString::get_Append_বাফার(int32_t minCapacity,
                                  int32_t desiredCapacityHint,
                                  int32_t &ফলাফল_Capacity,
                                  UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফলাফল_Capacity=0;
        ফেরত nullptr;
    }
    int32_t appendCapacity=বাফার.getCapacity()-len-1;  // -1 for NUL
    যদি(appendCapacity>=minCapacity) {
        ফলাফল_Capacity=appendCapacity;
        ফেরত বাফার.getAlias()+len;
    }
    যদি(ensureCapacity(len+minCapacity+1, len+desiredCapacityHint+1, errorCode)) {
        ফলাফল_Capacity=বাফার.getCapacity()-len-1;
        ফেরত বাফার.getAlias()+len;
    }
    ফলাফল_Capacity=0;
    ফেরত nullptr;
}

CharString &CharString::appendInvariantChars(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) {
    ফেরত appendInvariantChars(s.get_বাফার(), s.length(), errorCode);
}

CharString &CharString::appendInvariantChars(ধ্রুবক char16_t* uchars, int32_t ucharsLen, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত *এই;
    }
    যদি (!uprv_isInvariantUString(uchars, ucharsLen)) {
        errorCode = U_INVARIANT_CONVERSION_ERROR;
        ফেরত *এই;
    }
    যদি(ensureCapacity(len+ucharsLen+1, 0, errorCode)) {
        u_UCharsToChars(uchars, বাফার.getAlias()+len, ucharsLen);
        len += ucharsLen;
        বাফার[len] = 0;
    }
    ফেরত *এই;
}

UBool CharString::ensureCapacity(int32_t capacity,
                                 int32_t desiredCapacityHint,
                                 UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    যদি(capacity>বাফার.getCapacity()) {
        যদি(desiredCapacityHint==0) {
            desiredCapacityHint=capacity+বাফার.getCapacity();
        }
        যদি( (desiredCapacityHint<=capacity || বাফার.resize(desiredCapacityHint, len+1)==nullptr) &&
            বাফার.resize(capacity, len+1)==nullptr
        ) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

CharString &CharString::appendPathPart(StringPiece s, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত *এই;
    }
    যদি(s.length()==0) {
        ফেরত *এই;
    }
    অক্ষর c;
    যদি(len>0 && (c=বাফার[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    append(s, errorCode);
    ফেরত *এই;
}

CharString &CharString::ensureEndsWithFileSeparator(UErrorCode &errorCode) {
    অক্ষর c;
    যদি(U_SUCCESS(errorCode) && len>0 &&
            (c=বাফার[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), errorCode);
    }
    ফেরত *এই;
}

অক্ষর CharString::getDirSepChar() ধ্রুবক {
    অক্ষর dirSepChar = U_FILE_SEP_CHAR;
#যদি (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    যদি(len>0 && !uprv_strchr(তথ্য(), U_FILE_SEP_CHAR) && uprv_strchr(তথ্য(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#endif
    ফেরত dirSepChar;
}

U_NAMESPACE_END
