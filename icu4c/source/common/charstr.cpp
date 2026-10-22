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

#ಅನ್ತರ್ಭಾವ <cstdlib>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

CharString::CharString(CharString&& src) noexcept
        : ಬಫರ್(std::move(src.ಬಫರ್)), len(src.len) {
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
}

CharString& CharString::ಕ್ರಿಯಾಕಾರಕ=(CharString&& src) noexcept {
    ಬಫರ್ = std::move(src.ಬಫರ್);
    len = src.len;
    src.len = 0;  // not strictly necessary because we make no guarantees on the source string
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಅಕ್ಷರ *CharString::clone_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಅಕ್ಷರ *p = static_cast<ಅಕ್ಷರ *>(uprv_malloc(len + 1));
    ಆದರೆ (p == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(p, ಬಫರ್.getAlias(), len + 1);
    ಹಿಂತಿರುಗಿ p;
}

int32_t CharString::extract(ಅಕ್ಷರ *dest, int32_t capacity, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ len; }
    ಆದರೆ (capacity < 0 || (capacity > 0 && dest == ಶೂನ್ಯಸೂಚಕ)) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ len;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src = ಬಫರ್.getAlias();
    ಆದರೆ (0 < len && len <= capacity && src != dest) {
        uprv_memcpy(dest, src, len);
    }
    ಹಿಂತಿರುಗಿ u_terminateChars(dest, capacity, len, &error_ಸಂಕೇತ);
}

CharString &CharString::copyFrom(ಸ್ಥಿರಾಂಕ CharString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && ಇದು!=&s && ensure_ಸಾಮರ್ಥ್ಯ(s.len+1, 0, error_ಸಂಕೇತ)) {
        len=s.len;
        uprv_memcpy(ಬಫರ್.getAlias(), s.ಬಫರ್.getAlias(), len+1);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

CharString &CharString::copyFrom(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    len = 0;
    append(s, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

int32_t CharString::ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(ಅಕ್ಷರ c) ಸ್ಥಿರಾಂಕ {
    ಗಾಗಿ(int32_t i=len; i>0;) {
        ಆದರೆ(ಬಫರ್[--i]==c) {
            ಹಿಂತಿರುಗಿ i;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

ತಾರ್ಕಿಕ CharString::ಒಳಗೊಂಡಿದೆ(StringPiece s) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (s.empty()) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಬಫರ್.getAlias();
    int32_t ಕೊನೆಯ_ಆರಂಭ = len - s.length();
    ಗಾಗಿ (int32_t i = 0; i <= ಕೊನೆಯ_ಆರಂಭ; ++i) {
        ಆದರೆ (uprv_memcmp(p + i, s.ದತ್ತಾಂಶ(), s.length()) == 0) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

CharString &CharString::truncate(int32_t new_ಉದ್ದಳತೆ) {
    ಆದರೆ(new_ಉದ್ದಳತೆ<0) {
        new_ಉದ್ದಳತೆ=0;
    }
    ಆದರೆ(new_ಉದ್ದಳತೆ<len) {
        ಬಫರ್[len=new_ಉದ್ದಳತೆ]=0;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

CharString &CharString::append(ಅಕ್ಷರ c, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(len+2, 0, error_ಸಂಕೇತ)) {
        ಬಫರ್[len++]=c;
        ಬಫರ್[len]=0;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

CharString &CharString::append(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t s_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(s_ಉದ್ದಳತೆ<-1 || (s==ಶೂನ್ಯಸೂಚಕ && s_ಉದ್ದಳತೆ!=0)) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(s_ಉದ್ದಳತೆ<0) {
        s_ಉದ್ದಳತೆ= static_cast<int32_t>(uprv_strlen(s));
    }
    ಆದರೆ(s_ಉದ್ದಳತೆ>0) {
        ಆದರೆ(s==(ಬಫರ್.getAlias()+len)) {
            // The caller wrote into the getAppendBuffer().
            ಆದರೆ(s_ಉದ್ದಳತೆ>=(ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ()-len)) {
                // The caller wrote too much.
                error_ಸಂಕೇತ=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಬಫರ್[len+=s_ಉದ್ದಳತೆ]=0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಬಫರ್.getAlias()<=s && s<(ಬಫರ್.getAlias()+len) &&
                  s_ಉದ್ದಳತೆ>=(ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ()-len)
        ) {
            // (Part of) this string is appended to itself which requires reallocation,
            // so we have to make a copy of the substring and append that.
            ಹಿಂತಿರುಗಿ append(CharString(s, s_ಉದ್ದಳತೆ, error_ಸಂಕೇತ), error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(len+s_ಉದ್ದಳತೆ+1, 0, error_ಸಂಕೇತ)) {
            uprv_memcpy(ಬಫರ್.getAlias()+len, s, s_ಉದ್ದಳತೆ);
            ಬಫರ್[len+=s_ಉದ್ದಳತೆ]=0;
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

CharString &CharString::append_ಸಂಖ್ಯೆ(int64_t number, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (number < 0) {
        ಇದು->append('-', ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }

    ಆದರೆ (number == 0) {
        ಇದು->append('0', ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    int32_t ಅಂಕೆ_Len = 0;
    ಆಗ (number != 0) {
        int32_t residue = number % 10;
        number /= 10;
        ಇದು->append(std::ನಿರಪೇಕ್ಷ(residue) + '0', ಸ್ಥಿತಿ_2);
        ಅಂಕೆ_Len++;
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }

    int32_t ಆರಂಭ = ಇದು->length() - ಅಂಕೆ_Len, ಕೊನೆ = ಇದು->length() - 1;
    ಆಗ(ಆರಂಭ < ಕೊನೆ) {
        std::swap(ಇದು->ದತ್ತಾಂಶ()[ಆರಂಭ++], ಇದು->ದತ್ತಾಂಶ()[ಕೊನೆ--]);
    }

    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಅಕ್ಷರ *CharString::get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ,
                                  int32_t desired_ಸಾಮರ್ಥ್ಯ_Hint,
                                  int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
                                  UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=0;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    int32_t append_ಸಾಮರ್ಥ್ಯ=ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ()-len-1;  // -1 for NUL
    ಆದರೆ(append_ಸಾಮರ್ಥ್ಯ>=min_ಸಾಮರ್ಥ್ಯ) {
        ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=append_ಸಾಮರ್ಥ್ಯ;
        ಹಿಂತಿರುಗಿ ಬಫರ್.getAlias()+len;
    }
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(len+min_ಸಾಮರ್ಥ್ಯ+1, len+desired_ಸಾಮರ್ಥ್ಯ_Hint+1, error_ಸಂಕೇತ)) {
        ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ()-len-1;
        ಹಿಂತಿರುಗಿ ಬಫರ್.getAlias()+len;
    }
    ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=0;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

CharString &CharString::appendInvariantChars(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ appendInvariantChars(s.get_ಬಫರ್(), s.length(), error_ಸಂಕೇತ);
}

CharString &CharString::appendInvariantChars(ಸ್ಥಿರಾಂಕ char16_t* uchars, int32_t ucharsLen, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (!uprv_isInvariantUString(uchars, ucharsLen)) {
        error_ಸಂಕೇತ = U_INVARIANT_CONVERSION_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(ensure_ಸಾಮರ್ಥ್ಯ(len+ucharsLen+1, 0, error_ಸಂಕೇತ)) {
        u_UChars_ಗಮ್ಯ_Chars(uchars, ಬಫರ್.getAlias()+len, ucharsLen);
        len += ucharsLen;
        ಬಫರ್[len] = 0;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool CharString::ensure_ಸಾಮರ್ಥ್ಯ(int32_t capacity,
                                 int32_t desired_ಸಾಮರ್ಥ್ಯ_Hint,
                                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(capacity>ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ()) {
        ಆದರೆ(desired_ಸಾಮರ್ಥ್ಯ_Hint==0) {
            desired_ಸಾಮರ್ಥ್ಯ_Hint=capacity+ಬಫರ್.get_ಸಾಮರ್ಥ್ಯ();
        }
        ಆದರೆ( (desired_ಸಾಮರ್ಥ್ಯ_Hint<=capacity || ಬಫರ್.resize(desired_ಸಾಮರ್ಥ್ಯ_Hint, len+1)==ಶೂನ್ಯಸೂಚಕ) &&
            ಬಫರ್.resize(capacity, len+1)==ಶೂನ್ಯಸೂಚಕ
        ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

CharString &CharString::append_ಮಾರ್ಗ_Part(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ(s.length()==0) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಅಕ್ಷರ c;
    ಆದರೆ(len>0 && (c=ಬಫರ್[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ಸಂಕೇತ);
    }
    append(s, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

CharString &CharString::ensure_ತುದಿಗಳು_With_File_Separator(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಅಕ್ಷರ c;
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && len>0 &&
            (c=ಬಫರ್[len-1])!=U_FILE_SEP_CHAR && c!=U_FILE_ALT_SEP_CHAR) {
        append(getDirSepChar(), error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಅಕ್ಷರ CharString::getDirSepChar() ಸ್ಥಿರಾಂಕ {
    ಅಕ್ಷರ dirSepChar = U_FILE_SEP_CHAR;
#ಆದರೆ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    // We may need to return a different directory separator when building for Cygwin or MSYS2.
    ಆದರೆ(len>0 && !uprv_strchr(ದತ್ತಾಂಶ(), U_FILE_SEP_CHAR) && uprv_strchr(ದತ್ತಾಂಶ(), U_FILE_ALT_SEP_CHAR))
        dirSepChar = U_FILE_ALT_SEP_CHAR;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ dirSepChar;
}

U_NAMESPACE_ಕೊನೆ
