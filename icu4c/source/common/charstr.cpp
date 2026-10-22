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
        : بفر(std::move(src.بفر)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::عامل=(CharString&& src) noexcept {
    بفر = std::move(src.بفر);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    واپس *یہ;
}

حرف *CharString::clone_ڈیٹا(UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    اگر (p == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(p, بفر.getAlias(), len + 1);
    واپس p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس len; }
    اگر (capacity < 0 || (capacity > 0 && dest == خالی_اشاریہ)) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس len;
    }
    ثابت حرف *src = بفر.getAlias();
    اگر (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    واپس u_terminateChars(dest, capacity, len, &error_رمز);
}

CharString &CharString::copyFrom(ثابت CharString &s, UError_رمز &error_رمز) {
    اگر(U_SUCCESS(error_رمز) && یہ!=&s && ensure_استعداد(s.len+1, 0, error_رمز)) {
        len=s.len;
        uprv_memcpy(بفر.getAlias(), s.بفر.getAlias(), len+1);
    }
    واپس *یہ;
}

CharString &CharString::copyFrom(StringPiece s, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    len = 0;
    append(s, error_رمز);
    واپس *یہ;
}

int32_t CharString::آخری_اشاریہ_کا(حرف c) ثابت {
    برائے(int32_t i=len; i>0;) {
        اگر(بفر[--i]==c) {
            واپس i;
        }
    }
    واپس -1;
}

منطقی CharString::شامل_ہے(StringPiece s) ثابت {
    اگر (s.empty()) { واپس جھوٹ; }
    ثابت حرف *p = بفر.getAlias();
    int32_t آخری_آغاز = len - s.length();
    برائے (int32_t i = 0; i <= آخری_آغاز; ++i) {
        اگر (uprv_memcmp(p + i, s.ڈیٹا(), s.length()) == 0) {
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

CharString &CharString::truncate(int32_t new_لمبائی) {
    اگر(new_لمبائی<0) {
        new_لمبائی=0;
    }
    اگر(new_لمبائی<len) {
        بفر[len=new_لمبائی]=0;
    }
    واپس *یہ;
}

CharString &CharString::append(حرف c, UError_رمز &error_رمز) {
    اگر(ensure_استعداد(len+2, 0, error_رمز)) {
        بفر[len++]=c;
        بفر[len]=0;
    }
    واپس *یہ;
}

CharString &CharString::append(ثابت حرف *s, int32_t s_لمبائی, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    اگر(s_لمبائی<-1 || (s==خالی_اشاریہ && s_لمبائی!=0)) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    اگر(s_لمبائی<0) {
        s_لمبائی= static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(s_لمبائی>0) {
        اگر(s==(بفر.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            اگر(s_لمبائی>=(بفر.get_استعداد()-len)) {
                // The caller wrote too much.
                error_رمز=U_اندرونی_پروگرام_ERROR;
            } ورنہ {
                بفر[len+=s_لمبائی]=0;
            }
        } ورنہ اگر(بفر.getAlias()<=s && s<(بفر.getAlias()+len) &&
                  s_لمبائی>=(بفر.get_استعداد()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            واپس append(CharString(s, s_لمبائی, error_رمز), error_رمز);
        } ورنہ اگر(ensure_استعداد(len+s_لمبائی+1, 0, error_رمز)) {
            uprv_memcpy(بفر.getAlias()+len, s, s_لمبائی);
            بفر[len+=s_لمبائی]=0;
        }
    }
    واپس *یہ;
}

CharString &CharString::append_نمبر(int64_t number, UError_رمز &حالت_2) {
    اگر (number < 0) {
        یہ->append('-', حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس *یہ;
        }
    }

    اگر (number == 0) {
        یہ->append('0', حالت_2);
        واپس *یہ;
    }

    int32_t عدد_Len = 0;
    جب (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        یہ->append(std::مطلق_قدر(residue) + '0', حالت_2);
        عدد_Len++;
        اگر (U_FAILURE(حالت_2)) {
            واپس *یہ;
        }
    }

    int32_t آغاز = یہ->length() - عدد_Len, انتہا = یہ->length() - 1;
    جب(آغاز < انتہا) {
        std::swap(یہ->ڈیٹا()[آغاز++], یہ->ڈیٹا()[انتہا--]);
    }

    واپس *یہ;
}

حرف *CharString::get_Append_بفر(int32_t min_استعداد,
                                  int32_t desired_استعداد_Hint,
                                  int32_t &نتیجہ_استعداد,
                                  UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        نتیجہ_استعداد=0;
        واپس خالی_اشاریہ;
    }
    int32_t append_استعداد=بفر.get_استعداد()-len-1;  // -1 for NUL
    اگر(append_استعداد>=min_استعداد) {
        نتیجہ_استعداد=append_استعداد;
        واپس بفر.getAlias()+len;
    }
    اگر(ensure_استعداد(len+min_استعداد+1, len+desired_استعداد_Hint+1, error_رمز)) {
        نتیجہ_استعداد=بفر.get_استعداد()-len-1;
        واپس بفر.getAlias()+len;
    }
    نتیجہ_استعداد=0;
    واپس خالی_اشاریہ;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UError_رمز &error_رمز) {
    واپس appendInvariantChars(s.get_بفر(), s.length(), error_رمز);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    اگر (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_رمز = U_INVARIANT_CONVERSION_ERROR;
        واپس *یہ;
    }
    اگر(ensure_استعداد(len+ucharsLen+1, 0, error_رمز)) {
        u_UChars_مقصد_Chars(uchars, بفر.getAlias()+len, ucharsLen);
        len += ucharsLen;
        بفر[len] = 0;
    }
    واپس *یہ;
}

UBool CharString::ensure_استعداد(int32_t capacity,
                                 int32_t desired_استعداد_Hint,
                                 UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    اگر(capacity>بفر.get_استعداد()) {
        اگر(desired_استعداد_Hint==0) {
            desired_استعداد_Hint=capacity+بفر.get_استعداد();
        }
        اگر( (desired_استعداد_Hint<=capacity || بفر.resize(desired_استعداد_Hint, len+1)==خالی_اشاریہ) &&
            بفر.resize(capacity, len+1)==خالی_اشاریہ
        ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

CharString &CharString::append_راستہ_Part(StringPiece s, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    اگر(s.length()==0) {
        واپس *یہ;
    }
    حرف c;
    اگر(len>0 && (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_رمز);
    }
    append(s, error_رمز);
    واپس *یہ;
}

CharString &CharString::ensure_سرے_With_File_Separator(UError_رمز &error_رمز) {
    حرف c;
    اگر(U_SUCCESS(error_رمز) && len>0 &&
            (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_رمز);
    }
    واپس *یہ;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    اگر(len>0 && !uprv_strchr(ڈیٹا(), U_FILE_SEP_CHAR) && uprv_strchr(ڈیٹا(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#اگر_ختم
    واپس dirSepChar;
}

U_NAMESPACE_انتہا
