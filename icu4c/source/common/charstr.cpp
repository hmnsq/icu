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

#ਅਨ੍ਤਰ੍ਭਾਵ <cstdlib>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : ਬਫਰ(std::move(src.ਬਫਰ)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::ਸੰਚਾਲਕ=(CharString&& src) noexcept {
    ਬਫਰ = std::move(src.ਬਫਰ);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ਵਾਪਸ *ਇਹ;
}

ਅੱਖਰ *CharString::clone_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਅੱਖਰ *p = static_cast<ਅੱਖਰ *>(uprv_malloc(len + 1));
    ਜੇ (p == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(p, ਬਫਰ.getAlias(), len + 1);
    ਵਾਪਸ p;
}

int32_t CharString::extract(ਅੱਖਰ *dest, int32_t capacity, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ len; }
    ਜੇ (capacity < 0 || (capacity > 0 && dest == ਖਾਲੀ_ਸੂਚਕ)) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ len;
    }
    ਅਚਲ ਅੱਖਰ *src = ਬਫਰ.getAlias();
    ਜੇ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ਵਾਪਸ u_terminateChars(dest, capacity, len, &error_ਕੋਡ);
}

CharString &CharString::copyFrom(ਅਚਲ CharString &s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && ਇਹ!=&s && ensure_ਸਮਰੱਥਾ(s.len+1, 0, error_ਕੋਡ)) {
        len=s.len;
        uprv_memcpy(ਬਫਰ.getAlias(), s.ਬਫਰ.getAlias(), len+1);
    }
    ਵਾਪਸ *ਇਹ;
}

CharString &CharString::copyFrom(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    len = 0;
    append(s, error_ਕੋਡ);
    ਵਾਪਸ *ਇਹ;
}

int32_t CharString::ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(ਅੱਖਰ c) ਅਚਲ {
    ਲਈ(int32_t i=len; i>0;) {
        ਜੇ(ਬਫਰ[--i]==c) {
            ਵਾਪਸ i;
        }
    }
    ਵਾਪਸ -1;
}

ਤਾਰਕਿਕ CharString::ਸ਼ਾਮਲ_ਹੈ(StringPiece s) ਅਚਲ {
    ਜੇ (s.empty()) { ਵਾਪਸ ਝੂਠ; }
    ਅਚਲ ਅੱਖਰ *p = ਬਫਰ.getAlias();
    int32_t ਆਖਰੀ_ਸ਼ੁਰੂ = len - s.length();
    ਲਈ (int32_t i = 0; i <= ਆਖਰੀ_ਸ਼ੁਰੂ; ++i) {
        ਜੇ (uprv_memcmp(p + i, s.ਅੰਕੜੇ(), s.length()) == 0) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

CharString &CharString::truncate(int32_t new_ਲੰਮਾਈ) {
    ਜੇ(new_ਲੰਮਾਈ<0) {
        new_ਲੰਮਾਈ=0;
    }
    ਜੇ(new_ਲੰਮਾਈ<len) {
        ਬਫਰ[len=new_ਲੰਮਾਈ]=0;
    }
    ਵਾਪਸ *ਇਹ;
}

CharString &CharString::append(ਅੱਖਰ c, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(ensure_ਸਮਰੱਥਾ(len+2, 0, error_ਕੋਡ)) {
        ਬਫਰ[len++]=c;
        ਬਫਰ[len]=0;
    }
    ਵਾਪਸ *ਇਹ;
}

CharString &CharString::append(ਅਚਲ ਅੱਖਰ *s, int32_t s_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(s_ਲੰਮਾਈ<-1 || (s==ਖਾਲੀ_ਸੂਚਕ && s_ਲੰਮਾਈ!=0)) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(s_ਲੰਮਾਈ<0) {
        s_ਲੰਮਾਈ= static_cast<int32_t>(uprv_strlen(s));
    }
    ਜੇ(s_ਲੰਮਾਈ>0) {
        ਜੇ(s==(ਬਫਰ.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ਜੇ(s_ਲੰਮਾਈ>=(ਬਫਰ.get_ਸਮਰੱਥਾ()-len)) {
                // The caller wrote too much.
                error_ਕੋਡ=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            } ਨਹੀਂਤਾਂ {
                ਬਫਰ[len+=s_ਲੰਮਾਈ]=0;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(ਬਫਰ.getAlias()<=s && s<(ਬਫਰ.getAlias()+len) &&
                  s_ਲੰਮਾਈ>=(ਬਫਰ.get_ਸਮਰੱਥਾ()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ਵਾਪਸ append(CharString(s, s_ਲੰਮਾਈ, error_ਕੋਡ), error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(ensure_ਸਮਰੱਥਾ(len+s_ਲੰਮਾਈ+1, 0, error_ਕੋਡ)) {
            uprv_memcpy(ਬਫਰ.getAlias()+len, s, s_ਲੰਮਾਈ);
            ਬਫਰ[len+=s_ਲੰਮਾਈ]=0;
        }
    }
    ਵਾਪਸ *ਇਹ;
}

CharString &CharString::append_ਸੰਖਿਆ(int64_t number, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (number < 0) {
        ਇਹ->append('-', ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ *ਇਹ;
        }
    }

    ਜੇ (number == 0) {
        ਇਹ->append('0', ਸਥਿਤੀ);
        ਵਾਪਸ *ਇਹ;
    }

    int32_t ਅੰਕ_Len = 0;
    ਜਦੋਂ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ਇਹ->append(std::ਨਿਰਪੇਖ(residue) + '0', ਸਥਿਤੀ);
        ਅੰਕ_Len++;
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ *ਇਹ;
        }
    }

    int32_t ਸ਼ੁਰੂ = ਇਹ->length() - ਅੰਕ_Len, ਸਿਰਾ = ਇਹ->length() - 1;
    ਜਦੋਂ(ਸ਼ੁਰੂ < ਸਿਰਾ) {
        std::swap(ਇਹ->ਅੰਕੜੇ()[ਸ਼ੁਰੂ++], ਇਹ->ਅੰਕੜੇ()[ਸਿਰਾ--]);
    }

    ਵਾਪਸ *ਇਹ;
}

ਅੱਖਰ *CharString::get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ,
                                  int32_t desired_ਸਮਰੱਥਾ_Hint,
                                  int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ,
                                  UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਨਤੀਜਾ_ਸਮਰੱਥਾ=0;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    int32_t append_ਸਮਰੱਥਾ=ਬਫਰ.get_ਸਮਰੱਥਾ()-len-1;  // -1 for NUL
    ਜੇ(append_ਸਮਰੱਥਾ>=min_ਸਮਰੱਥਾ) {
        ਨਤੀਜਾ_ਸਮਰੱਥਾ=append_ਸਮਰੱਥਾ;
        ਵਾਪਸ ਬਫਰ.getAlias()+len;
    }
    ਜੇ(ensure_ਸਮਰੱਥਾ(len+min_ਸਮਰੱਥਾ+1, len+desired_ਸਮਰੱਥਾ_Hint+1, error_ਕੋਡ)) {
        ਨਤੀਜਾ_ਸਮਰੱਥਾ=ਬਫਰ.get_ਸਮਰੱਥਾ()-len-1;
        ਵਾਪਸ ਬਫਰ.getAlias()+len;
    }
    ਨਤੀਜਾ_ਸਮਰੱਥਾ=0;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

CharString &CharString::appendInvariantChars(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ appendInvariantChars(s.get_ਬਫਰ(), s.length(), error_ਕੋਡ);
}

CharString &CharString::appendInvariantChars(ਅਚਲ char16_t* uchars, int32_t ucharsLen, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_ਕੋਡ = U_INVARIANT_CONVERSION_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(ensure_ਸਮਰੱਥਾ(len+ucharsLen+1, 0, error_ਕੋਡ)) {
        u_UChars_ਮੰਜ਼ਿਲ_Chars(uchars, ਬਫਰ.getAlias()+len, ucharsLen);
        len += ucharsLen;
        ਬਫਰ[len] = 0;
    }
    ਵਾਪਸ *ਇਹ;
}

UBool CharString::ensure_ਸਮਰੱਥਾ(int32_t capacity,
                                 int32_t desired_ਸਮਰੱਥਾ_Hint,
                                 UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(capacity>ਬਫਰ.get_ਸਮਰੱਥਾ()) {
        ਜੇ(desired_ਸਮਰੱਥਾ_Hint==0) {
            desired_ਸਮਰੱਥਾ_Hint=capacity+ਬਫਰ.get_ਸਮਰੱਥਾ();
        }
        ਜੇ( (desired_ਸਮਰੱਥਾ_Hint<=capacity || ਬਫਰ.resize(desired_ਸਮਰੱਥਾ_Hint, len+1)==ਖਾਲੀ_ਸੂਚਕ) &&
            ਬਫਰ.resize(capacity, len+1)==ਖਾਲੀ_ਸੂਚਕ
        ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

CharString &CharString::append_ਰਸਤਾ_Part(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ(s.length()==0) {
        ਵਾਪਸ *ਇਹ;
    }
    ਅੱਖਰ c;
    ਜੇ(len>0 && (c=ਬਫਰ[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ਕੋਡ);
    }
    append(s, error_ਕੋਡ);
    ਵਾਪਸ *ਇਹ;
}

CharString &CharString::ensure_ਸਿਰੇ_With_File_Separator(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅੱਖਰ c;
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && len>0 &&
            (c=ਬਫਰ[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ਕੋਡ);
    }
    ਵਾਪਸ *ਇਹ;
}

ਅੱਖਰ CharString::getDirSepChar() ਅਚਲ {
    ਅੱਖਰ dirSepChar = U_FILE_SEP_CHAR;
#ਜੇ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ਜੇ(len>0 && !uprv_strchr(ਅੰਕੜੇ(), U_FILE_SEP_CHAR) && uprv_strchr(ਅੰਕੜੇ(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ dirSepChar;
}

U_NAMESPACE_ਸਿਰਾ
