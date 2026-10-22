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
        : بفر(std::move(src.بفر)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::عامل=(CharString&& src) noexcept {
    بفر = std::move(src.بفر);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    أرجع *هذا;
}

حرف *CharString::clone_انکڑے(UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    إذا (p == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(p, بفر.getAlias(), len + 1);
    أرجع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع len; }
    إذا (capacity < 0 || (capacity > 0 && dest == خالی_سوچک)) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع len;
    }
    ثابت حرف *src = بفر.getAlias();
    إذا (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    أرجع u_terminateChars(dest, capacity, len, &error_کوڈ);
}

CharString &CharString::copyFrom(ثابت CharString &s, UError_کوڈ &error_کوڈ) {
    إذا(U_SUCCESS(error_کوڈ) && هذا!=&s && ensure_سمرتھا(s.len+1, 0, error_کوڈ)) {
        len=s.len;
        uprv_memcpy(بفر.getAlias(), s.بفر.getAlias(), len+1);
    }
    أرجع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    len = 0;
    append(s, error_کوڈ);
    أرجع *هذا;
}

int32_t CharString::آخری_اشاریہ_دا(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        إذا(بفر[--i]==c) {
            أرجع i;
        }
    }
    أرجع -1;
}

منطقي CharString::شامل_اے(StringPiece s) ثابت {
    إذا (s.empty()) { أرجع خطأ; }
    ثابت حرف *p = بفر.getAlias();
    int32_t آخری_شروع = len - s.length();
    لكل (int32_t i = 0; i <= آخری_شروع; ++i) {
        إذا (uprv_memcmp(p + i, s.انکڑے(), s.length()) == 0) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

CharString &CharString::truncate(int32_t new_لمائی) {
    إذا(new_لمائی<0) {
        new_لمائی=0;
    }
    إذا(new_لمائی<len) {
        بفر[len=new_لمائی]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(حرف c, UError_کوڈ &error_کوڈ) {
    إذا(ensure_سمرتھا(len+2, 0, error_کوڈ)) {
        بفر[len++]=c;
        بفر[len]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t s_لمائی, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    إذا(s_لمائی<-1 || (s==خالی_سوچک && s_لمائی!=0)) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا(s_لمائی<0) {
        s_لمائی= static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(s_لمائی>0) {
        إذا(s==(بفر.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            إذا(s_لمائی>=(بفر.get_سمرتھا()-len)) {
                // The caller wrote too much.
                error_کوڈ=U_اندرونی_پروگرام_ERROR;
            } وإلا {
                بفر[len+=s_لمائی]=0;
            }
        } وإلا إذا(بفر.getAlias()<=s && s<(بفر.getAlias()+len) &&
                  s_لمائی>=(بفر.get_سمرتھا()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            أرجع append(CharString(s, s_لمائی, error_کوڈ), error_کوڈ);
        } وإلا إذا(ensure_سمرتھا(len+s_لمائی+1, 0, error_کوڈ)) {
            uprv_memcpy(بفر.getAlias()+len, s, s_لمائی);
            بفر[len+=s_لمائی]=0;
        }
    }
    أرجع *هذا;
}

CharString &CharString::append_نمبر(int64_t number, UError_کوڈ &حالت) {
    إذا (number < 0) {
        هذا->append('-', حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع *هذا;
        }
    }

    إذا (number == 0) {
        هذا->append('0', حالت);
        أرجع *هذا;
    }

    int32_t عدد_Len = 0;
    طالما (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::مطلق_قدر(residue) + '0', حالت);
        عدد_Len++;
        إذا (U_FAILURE(حالت)) {
            أرجع *هذا;
        }
    }

    int32_t شروع = هذا->length() - عدد_Len, سرا = هذا->length() - 1;
    طالما(شروع < سرا) {
        std::swap(هذا->انکڑے()[شروع++], هذا->انکڑے()[سرا--]);
    }

    أرجع *هذا;
}

حرف *CharString::get_Append_بفر(int32_t min_سمرتھا,
                                  int32_t desired_سمرتھا_Hint,
                                  int32_t &نتیجہ_سمرتھا,
                                  UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        نتیجہ_سمرتھا=0;
        أرجع خالی_سوچک;
    }
    int32_t append_سمرتھا=بفر.get_سمرتھا()-len-1;  // -1 for NUL
    إذا(append_سمرتھا>=min_سمرتھا) {
        نتیجہ_سمرتھا=append_سمرتھا;
        أرجع بفر.getAlias()+len;
    }
    إذا(ensure_سمرتھا(len+min_سمرتھا+1, len+desired_سمرتھا_Hint+1, error_کوڈ)) {
        نتیجہ_سمرتھا=بفر.get_سمرتھا()-len-1;
        أرجع بفر.getAlias()+len;
    }
    نتیجہ_سمرتھا=0;
    أرجع خالی_سوچک;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) {
    أرجع appendInvariantChars(s.get_بفر(), s.length(), error_کوڈ);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    إذا (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_کوڈ = U_INVARIANT_CONVERSION_ERROR;
        أرجع *هذا;
    }
    إذا(ensure_سمرتھا(len+ucharsLen+1, 0, error_کوڈ)) {
        u_UChars_منزل_Chars(uchars, بفر.getAlias()+len, ucharsLen);
        len += ucharsLen;
        بفر[len] = 0;
    }
    أرجع *هذا;
}

UBool CharString::ensure_سمرتھا(int32_t capacity,
                                 int32_t desired_سمرتھا_Hint,
                                 UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    إذا(capacity>بفر.get_سمرتھا()) {
        إذا(desired_سمرتھا_Hint==0) {
            desired_سمرتھا_Hint=capacity+بفر.get_سمرتھا();
        }
        إذا( (desired_سمرتھا_Hint<=capacity || بفر.resize(desired_سمرتھا_Hint, len+1)==خالی_سوچک) &&
            بفر.resize(capacity, len+1)==خالی_سوچک
        ) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

CharString &CharString::append_رستہ_Part(StringPiece s, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    إذا(s.length()==0) {
        أرجع *هذا;
    }
    حرف c;
    إذا(len>0 && (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کوڈ);
    }
    append(s, error_کوڈ);
    أرجع *هذا;
}

CharString &CharString::ensure_سرے_With_File_Separator(UError_کوڈ &error_کوڈ) {
    حرف c;
    إذا(U_SUCCESS(error_کوڈ) && len>0 &&
            (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کوڈ);
    }
    أرجع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    إذا(len>0 && !uprv_strchr(انکڑے(), U_FILE_SEP_CHAR) && uprv_strchr(انکڑے(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#جے_انت
    أرجع dirSepChar;
}

U_NAMESPACE_سرا
