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

حرف *CharString::clone_معلومات(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    إذا (p == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(p, بفر.getAlias(), len + 1);
    أرجع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع len; }
    إذا (capacity < 0 || (capacity > 0 && dest == خالي_اشارو)) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع len;
    }
    ثابت حرف *src = بفر.getAlias();
    إذا (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    أرجع u_terminateChars(dest, capacity, len, &error_ڪوڊ);
}

CharString &CharString::copyFrom(ثابت CharString &s, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_SUCCESS(error_ڪوڊ) && هذا!=&s && ensure_صلاحيت(s.len+1, 0, error_ڪوڊ)) {
        len=s.len;
        uprv_memcpy(بفر.getAlias(), s.بفر.getAlias(), len+1);
    }
    أرجع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    len = 0;
    append(s, error_ڪوڊ);
    أرجع *هذا;
}

int32_t CharString::آخري_اشاريو_جو(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        إذا(بفر[--i]==c) {
            أرجع i;
        }
    }
    أرجع -1;
}

منطقي CharString::شامل_آهي(StringPiece s) ثابت {
    إذا (s.empty()) { أرجع خطأ; }
    ثابت حرف *p = بفر.getAlias();
    int32_t آخري_شروعات = len - s.length();
    لكل (int32_t i = 0; i <= آخري_شروعات; ++i) {
        إذا (uprv_memcmp(p + i, s.معلومات(), s.length()) == 0) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

CharString &CharString::truncate(int32_t new_ڊيگهائي) {
    إذا(new_ڊيگهائي<0) {
        new_ڊيگهائي=0;
    }
    إذا(new_ڊيگهائي<len) {
        بفر[len=new_ڊيگهائي]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(حرف c, UError_ڪوڊ &error_ڪوڊ) {
    إذا(ensure_صلاحيت(len+2, 0, error_ڪوڊ)) {
        بفر[len++]=c;
        بفر[len]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t s_ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    إذا(s_ڊيگهائي<-1 || (s==خالي_اشارو && s_ڊيگهائي!=0)) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع *هذا;
    }
    إذا(s_ڊيگهائي<0) {
        s_ڊيگهائي= static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(s_ڊيگهائي>0) {
        إذا(s==(بفر.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            إذا(s_ڊيگهائي>=(بفر.get_صلاحيت()-len)) {
                // The caller wrote too much.
                error_ڪوڊ=U_اندروني_پروگرام_ERROR;
            } وإلا {
                بفر[len+=s_ڊيگهائي]=0;
            }
        } وإلا إذا(بفر.getAlias()<=s && s<(بفر.getAlias()+len) &&
                  s_ڊيگهائي>=(بفر.get_صلاحيت()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            أرجع append(CharString(s, s_ڊيگهائي, error_ڪوڊ), error_ڪوڊ);
        } وإلا إذا(ensure_صلاحيت(len+s_ڊيگهائي+1, 0, error_ڪوڊ)) {
            uprv_memcpy(بفر.getAlias()+len, s, s_ڊيگهائي);
            بفر[len+=s_ڊيگهائي]=0;
        }
    }
    أرجع *هذا;
}

CharString &CharString::append_عدد(int64_t number, UError_ڪوڊ &حالت) {
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

    int32_t انگ_Len = 0;
    طالما (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::مطلق_قدر(residue) + '0', حالت);
        انگ_Len++;
        إذا (U_FAILURE(حالت)) {
            أرجع *هذا;
        }
    }

    int32_t شروعات = هذا->length() - انگ_Len, پڇاڙي = هذا->length() - 1;
    طالما(شروعات < پڇاڙي) {
        std::swap(هذا->معلومات()[شروعات++], هذا->معلومات()[پڇاڙي--]);
    }

    أرجع *هذا;
}

حرف *CharString::get_Append_بفر(int32_t min_صلاحيت,
                                  int32_t desired_صلاحيت_Hint,
                                  int32_t &نتيجو_صلاحيت,
                                  UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        نتيجو_صلاحيت=0;
        أرجع خالي_اشارو;
    }
    int32_t append_صلاحيت=بفر.get_صلاحيت()-len-1;  // -1 for NUL
    إذا(append_صلاحيت>=min_صلاحيت) {
        نتيجو_صلاحيت=append_صلاحيت;
        أرجع بفر.getAlias()+len;
    }
    إذا(ensure_صلاحيت(len+min_صلاحيت+1, len+desired_صلاحيت_Hint+1, error_ڪوڊ)) {
        نتيجو_صلاحيت=بفر.get_صلاحيت()-len-1;
        أرجع بفر.getAlias()+len;
    }
    نتيجو_صلاحيت=0;
    أرجع خالي_اشارو;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) {
    أرجع appendInvariantChars(s.get_بفر(), s.length(), error_ڪوڊ);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    إذا (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_ڪوڊ = U_INVARIANT_CONVERSION_ERROR;
        أرجع *هذا;
    }
    إذا(ensure_صلاحيت(len+ucharsLen+1, 0, error_ڪوڊ)) {
        u_UChars_منزل_Chars(uchars, بفر.getAlias()+len, ucharsLen);
        len += ucharsLen;
        بفر[len] = 0;
    }
    أرجع *هذا;
}

UBool CharString::ensure_صلاحيت(int32_t capacity,
                                 int32_t desired_صلاحيت_Hint,
                                 UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    إذا(capacity>بفر.get_صلاحيت()) {
        إذا(desired_صلاحيت_Hint==0) {
            desired_صلاحيت_Hint=capacity+بفر.get_صلاحيت();
        }
        إذا( (desired_صلاحيت_Hint<=capacity || بفر.resize(desired_صلاحيت_Hint, len+1)==خالي_اشارو) &&
            بفر.resize(capacity, len+1)==خالي_اشارو
        ) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

CharString &CharString::append_رستو_Part(StringPiece s, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    إذا(s.length()==0) {
        أرجع *هذا;
    }
    حرف c;
    إذا(len>0 && (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ڪوڊ);
    }
    append(s, error_ڪوڊ);
    أرجع *هذا;
}

CharString &CharString::ensure_پڇاڙيون_With_File_Separator(UError_ڪوڊ &error_ڪوڊ) {
    حرف c;
    إذا(U_SUCCESS(error_ڪوڊ) && len>0 &&
            (c=بفر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ڪوڊ);
    }
    أرجع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    إذا(len>0 && !uprv_strchr(معلومات(), U_FILE_SEP_CHAR) && uprv_strchr(معلومات(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#جيڪڏهن_پورو
    أرجع dirSepChar;
}

U_NAMESPACE_پڇاڙي
