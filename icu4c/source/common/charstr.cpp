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
        : میانگیر(std::move(src.میانگیر)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::عملگر=(CharString&& src) noexcept {
    میانگیر = std::move(src.میانگیر);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    أرجع *هذا;
}

حرف *CharString::clone_داده(UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    حرف *p = static_cast<حرف *>(uprv_malloc(len + 1));
    إذا (p == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(p, میانگیر.getAlias(), len + 1);
    أرجع p;
}

int32_t CharString::extract(حرف *dest, int32_t capacity, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع len; }
    إذا (capacity < 0 || (capacity > 0 && dest == اشاره_تهی)) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع len;
    }
    ثابت حرف *src = میانگیر.getAlias();
    إذا (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    أرجع u_terminateChars(dest, capacity, len, &error_کد);
}

CharString &CharString::copyFrom(ثابت CharString &s, UError_کد &error_کد) {
    إذا(U_SUCCESS(error_کد) && هذا!=&s && ensure_ظرفیت(s.len+1, 0, error_کد)) {
        len=s.len;
        uprv_memcpy(میانگیر.getAlias(), s.میانگیر.getAlias(), len+1);
    }
    أرجع *هذا;
}

CharString &CharString::copyFrom(StringPiece s, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    len = 0;
    append(s, error_کد);
    أرجع *هذا;
}

int32_t CharString::آخرین_شاخص_از(حرف c) ثابت {
    لكل(int32_t i=len; i>0;) {
        إذا(میانگیر[--i]==c) {
            أرجع i;
        }
    }
    أرجع -1;
}

منطقي CharString::شامل_است(StringPiece s) ثابت {
    إذا (s.empty()) { أرجع خطأ; }
    ثابت حرف *p = میانگیر.getAlias();
    int32_t آخرین_شروع = len - s.length();
    لكل (int32_t i = 0; i <= آخرین_شروع; ++i) {
        إذا (uprv_memcmp(p + i, s.داده(), s.length()) == 0) {
            أرجع درست;
        }
    }
    أرجع خطأ;
}

CharString &CharString::truncate(int32_t new_درازی) {
    إذا(new_درازی<0) {
        new_درازی=0;
    }
    إذا(new_درازی<len) {
        میانگیر[len=new_درازی]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(حرف c, UError_کد &error_کد) {
    إذا(ensure_ظرفیت(len+2, 0, error_کد)) {
        میانگیر[len++]=c;
        میانگیر[len]=0;
    }
    أرجع *هذا;
}

CharString &CharString::append(ثابت حرف *s, int32_t s_درازی, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    إذا(s_درازی<-1 || (s==اشاره_تهی && s_درازی!=0)) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    إذا(s_درازی<0) {
        s_درازی= static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(s_درازی>0) {
        إذا(s==(میانگیر.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            إذا(s_درازی>=(میانگیر.get_ظرفیت()-len)) {
                // The caller wrote too much.
                error_کد=U_داخلی_برنامه_ERROR;
            } وإلا {
                میانگیر[len+=s_درازی]=0;
            }
        } وإلا إذا(میانگیر.getAlias()<=s && s<(میانگیر.getAlias()+len) &&
                  s_درازی>=(میانگیر.get_ظرفیت()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            أرجع append(CharString(s, s_درازی, error_کد), error_کد);
        } وإلا إذا(ensure_ظرفیت(len+s_درازی+1, 0, error_کد)) {
            uprv_memcpy(میانگیر.getAlias()+len, s, s_درازی);
            میانگیر[len+=s_درازی]=0;
        }
    }
    أرجع *هذا;
}

CharString &CharString::append_عدد(int64_t number, UError_کد &وضعیت) {
    إذا (number < 0) {
        هذا->append('-', وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع *هذا;
        }
    }

    إذا (number == 0) {
        هذا->append('0', وضعیت);
        أرجع *هذا;
    }

    int32_t رقم_Len = 0;
    طالما (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        هذا->append(std::قدر_مطلق(residue) + '0', وضعیت);
        رقم_Len++;
        إذا (U_FAILURE(وضعیت)) {
            أرجع *هذا;
        }
    }

    int32_t شروع = هذا->length() - رقم_Len, انتها = هذا->length() - 1;
    طالما(شروع < انتها) {
        std::swap(هذا->داده()[شروع++], هذا->داده()[انتها--]);
    }

    أرجع *هذا;
}

حرف *CharString::get_Append_میانگیر(int32_t min_ظرفیت,
                                  int32_t desired_ظرفیت_Hint,
                                  int32_t &نتیجه_ظرفیت,
                                  UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        نتیجه_ظرفیت=0;
        أرجع اشاره_تهی;
    }
    int32_t append_ظرفیت=میانگیر.get_ظرفیت()-len-1;  // -1 for NUL
    إذا(append_ظرفیت>=min_ظرفیت) {
        نتیجه_ظرفیت=append_ظرفیت;
        أرجع میانگیر.getAlias()+len;
    }
    إذا(ensure_ظرفیت(len+min_ظرفیت+1, len+desired_ظرفیت_Hint+1, error_کد)) {
        نتیجه_ظرفیت=میانگیر.get_ظرفیت()-len-1;
        أرجع میانگیر.getAlias()+len;
    }
    نتیجه_ظرفیت=0;
    أرجع اشاره_تهی;
}

CharString &CharString::appendInvariantChars(ثابت UnicodeString &s, UError_کد &error_کد) {
    أرجع appendInvariantChars(s.get_میانگیر(), s.length(), error_کد);
}

CharString &CharString::appendInvariantChars(ثابت char16_t* uchars, int32_t ucharsLen, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    إذا (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_کد = U_INVARIANT_CONVERSION_ERROR;
        أرجع *هذا;
    }
    إذا(ensure_ظرفیت(len+ucharsLen+1, 0, error_کد)) {
        u_UChars_مقصد_Chars(uchars, میانگیر.getAlias()+len, ucharsLen);
        len += ucharsLen;
        میانگیر[len] = 0;
    }
    أرجع *هذا;
}

UBool CharString::ensure_ظرفیت(int32_t capacity,
                                 int32_t desired_ظرفیت_Hint,
                                 UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    إذا(capacity>میانگیر.get_ظرفیت()) {
        إذا(desired_ظرفیت_Hint==0) {
            desired_ظرفیت_Hint=capacity+میانگیر.get_ظرفیت();
        }
        إذا( (desired_ظرفیت_Hint<=capacity || میانگیر.resize(desired_ظرفیت_Hint, len+1)==اشاره_تهی) &&
            میانگیر.resize(capacity, len+1)==اشاره_تهی
        ) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }
    }
    أرجع درست;
}

CharString &CharString::append_مسیر_Part(StringPiece s, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    إذا(s.length()==0) {
        أرجع *هذا;
    }
    حرف c;
    إذا(len>0 && (c=میانگیر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کد);
    }
    append(s, error_کد);
    أرجع *هذا;
}

CharString &CharString::ensure_دوسر_With_File_Separator(UError_کد &error_کد) {
    حرف c;
    إذا(U_SUCCESS(error_کد) && len>0 &&
            (c=میانگیر[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_کد);
    }
    أرجع *هذا;
}

حرف CharString::getDirSepChar() ثابت {
    حرف dirSepChar = U_FILE_SEP_CHAR;
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    إذا(len>0 && !uprv_strchr(داده(), U_FILE_SEP_CHAR) && uprv_strchr(داده(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#پایان_اگر
    أرجع dirSepChar;
}

U_NAMESPACE_انتها
