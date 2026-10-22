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

CharString& CharString::چلوونکی=(CharString&& src) noexcept {
    بفر = std::move(src.بفر);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    أرجع *هذا;
}

حرف *CharString::clone_معلومات(UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    إذا (p == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(p, بفر.getAlias(), len + 1);
    أرجع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع len; }
    إذا (capacity < 0 || (capacity > 0 && dest == تش_اشاره)) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع len;
    }
    ثابت حرف *src = بفر.getAlias();
    إذا (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    أرجع u_terminateChars(dest, capacity, len, &error_کوډ);
}

CharString &CharString::copyFrom(ثابت CharString &s, UError_کوډ &error_کوډ) {
    إذا(U_SUCCESS(error_کوډ) && هذا!=&s && ensure_ظرفیت(s.len+1, 0, error_کوډ)) {
        len=s.len;
        uprv_memcpy(بفر.getAlias(), s.بفر.getAlias(), len+1);
    }
    أرجع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    len = 0;
    append(s, error_کوډ);
    أرجع *هذا;
}

int32_t CharString::وروستی_شاخص_د(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        إذا(بفر[--i]==c) {
            أرجع i;
        }
    }
    أرجع -1;
}

منطقي CharString::لري(StringPiece s) ثابت {
    إذا (s.empty()) { أرجع خطأ; }
    ثابت حرف *p = بفر.getAlias();
    int32_t وروستی_پیل = len - s.length();
    لكل (int32_t i = 0; i <= وروستی_پیل; ++i) {
        إذا (uprv_memcmp(p + i, s.معلومات(), s.length()) == 0) {
            أرجع ریښتیا;
        }
    }
    أرجع خطأ;
}

CharString &CharString::truncate(int32_t new_اوږدتیا) {
    إذا(new_اوږدتیا<0) {
        new_اوږدتیا=0;
    }
    إذا(new_اوږدتیا<len) {
        بفر[len=new_اوږدتیا]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(حرف c, UError_کوډ &error_کوډ) {
    إذا(ensure_ظرفیت(len+2, 0, error_کوډ)) {
        بفر[len++]=c;
        بفر[len]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t s_اوږدتیا, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    إذا(s_اوږدتیا<-1 || (s==تش_اشاره && s_اوږدتیا!=0)) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا(s_اوږدتیا<0) {
        s_اوږدتیا= static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(s_اوږدتیا>0) {
        إذا(s==(بفر.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            إذا(s_اوږدتیا>=(بفر.get_ظرفیت()-len)) {
                // The caller wrote too much.
                error_کوډ=U_دننی_پروګرام_ERROR;
            } وإلا {
                بفر[len+=s_اوږدتیا]=0;
            }
        } وإلا إذا(بفر.getAlias()<=s && s<(بفر.getAlias()+len) &&
                  s_اوږدتیا>=(بفر.get_ظرفیت()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            أرجع append(CharString(s, s_اوږدتیا, error_کوډ), error_کوډ);
        } وإلا إذا(ensure_ظرفیت(len+s_اوږدتیا+1, 0, error_کوډ)) {
            uprv_memcpy(بفر.getAlias()+len, s, s_اوږدتیا);
            بفر[len+=s_اوږدتیا]=0;
        }
    }
    أرجع *هذا;
}

CharString &CharString::append_شمېره(int64_t number, UError_کوډ &حالت) {
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
        هذا->append(std::مطلق_قیمت(residue) + '0', حالت);
        عدد_Len++;
        إذا (U_FAILURE(حالت)) {
            أرجع *هذا;
        }
    }

    int32_t پیل = هذا->length() - عدد_Len, پای_ټکی = هذا->length() - 1;
    طالما(پیل < پای_ټکی) {
        std::swap(هذا->معلومات()[پیل++], هذا->معلومات()[پای_ټکی--]);
    }

    أرجع *هذا;
}

حرف *CharString::get_Append_بفر(int32_t min_ظرفیت,
                                  int32_t desired_ظرفیت_Hint,
                                  int32_t &پایله_ظرفیت,
                                  UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        پایله_ظرفیت=0;
        أرجع تش_اشاره;
    }
    int32_t append_ظرفیت=بفر.get_ظرفیت()-len-1;  // -1 for NUL
    إذا(append_ظرفیت>=min_ظرفیت) {
        پایله_ظرفیت=append_ظرفیت;
        أرجع بفر.getAlias()+len;
    }
    إذا(ensure_ظرفیت(len+min_ظرفیت+1, len+desired_ظرفیت_Hint+1, error_کوډ)) {
        پایله_ظرفیت=بفر.get_ظرفیت()-len-1;
        أرجع بفر.getAlias()+len;
    }
    پایله_ظرفیت=0;
    أرجع تش_اشاره;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UError_کوډ &error_کوډ) {
    أرجع appendInvariantChars(s.get_بفر(), s.length(), error_کوډ);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    إذا (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_کوډ = U_INVARIANT_CONVERSION_ERROR;
        أرجع *هذا;
    }
    إذا(ensure_ظرفیت(len+ucharsLen+1, 0, error_کوډ)) {
        u_UChars_منزل_Chars(uchars, بفر.getAlias()+len, ucharsLen);
        len += ucharsLen;
        بفر[len] = 0;
    }
    أرجع *هذا;
}

UBool CharString::ensure_ظرفیت(int32_t capacity,
                                 int32_t desired_ظرفیت_Hint,
                                 UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    إذا(capacity>بفر.get_ظرفیت()) {
        إذا(desired_ظرفیت_Hint==0) {
            desired_ظرفیت_Hint=capacity+بفر.get_ظرفیت();
        }
        إذا( (desired_ظرفیت_Hint<=capacity || بفر.resize(desired_ظرفیت_Hint, len+1)==تش_اشاره) &&
            بفر.resize(capacity, len+1)==تش_اشاره
        ) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}

CharString &CharString::append_لار_Part(StringPiece s, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    إذا(s.length()==0) {
        أرجع *هذا;
    }
    حرف c;
    إذا(len>0 && (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کوډ);
    }
    append(s, error_کوډ);
    أرجع *هذا;
}

CharString &CharString::ensure_پایونه_With_File_Separator(UError_کوډ &error_کوډ) {
    حرف c;
    إذا(U_SUCCESS(error_کوډ) && len>0 &&
            (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کوډ);
    }
    أرجع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    إذا(len>0 && !uprv_strchr(معلومات(), U_FILE_SEP_CHAR) && uprv_strchr(معلومات(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#د_که_پای
    أرجع dirSepChar;
}

U_NAMESPACE_پای_ټکی
