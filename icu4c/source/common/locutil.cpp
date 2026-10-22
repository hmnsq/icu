// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "locutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
ಸ್ಥಿರ icu::UInitOnce   LocaleUtilityInitOnce {};
ಸ್ಥಿರ icu::Hashtable * LocaleUtility_cache = ಶೂನ್ಯಸೂಚಕ;

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV service_cleanup() {
    ಆದರೆ (LocaleUtility_cache) {
        ಅಳಿಸು LocaleUtility_cache;
        LocaleUtility_cache = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV locale_utility_init(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಬಳಸಿ ನಾಮಾಕಾಶ icu;
    U_ASSERT(LocaleUtility_cache == ಶೂನ್ಯಸೂಚಕ);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = ಹೊಸ Hashtable(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು LocaleUtility_cache;
        LocaleUtility_cache = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (LocaleUtility_cache == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    LocaleUtility_cache->ಗಣ_ಮೌಲ್ಯ_Deleter(uhash_deleteHashtable);
}

U_CDECL_ಕೊನೆ

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ಸ್ಥಿರಾಂಕ UnicodeString* id, UnicodeString& ಫಲಿತಾಂಶ)
{
  ಆದರೆ (id == ಶೂನ್ಯಸೂಚಕ) {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
  } ಇಲ್ಲದಿದ್ದರೆ {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    ಫಲಿತಾಂಶ = *id;
    int32_t i = 0;
    int32_t ಕೊನೆ = ಫಲಿತಾಂಶ.ಸೂಚ್ಯಂಕ_ದ(AT_SIGN_CHAR);
    int32_t n = ಫಲಿತಾಂಶ.ಸೂಚ್ಯಂಕ_ದ(PERIOD_CHAR);
    ಆದರೆ (n >= 0 && n < ಕೊನೆ) {
        ಕೊನೆ = n;
    }
    ಆದರೆ (ಕೊನೆ < 0) {
        ಕೊನೆ = ಫಲಿತಾಂಶ.length();
    }
    n = ಫಲಿತಾಂಶ.ಸೂಚ್ಯಂಕ_ದ(UNDERSCORE_CHAR);
    ಆದರೆ (n < 0) {
      n = ಕೊನೆ;
    }
    ಗಾಗಿ (; i < n; ++i) {
      char16_t c = ಫಲಿತಾಂಶ.charAt(i);
      ಆದರೆ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        ಫಲಿತಾಂಶ.ಗಣ_Char_At(i, c);
      }
    }
    ಗಾಗಿ (n = ಕೊನೆ; i < n; ++i) {
      char16_t c = ಫಲಿತಾಂಶ.charAt(i);
      ಆದರೆ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        ಫಲಿತಾಂಶ.ಗಣ_Char_At(i, c);
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ಆದರೆ 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    ಆದರೆ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        ಅಕ್ಷರ* buf = (ಅಕ್ಷರ*) uprv_malloc(buflen);
        ಅಕ್ಷರ* canon = (buf == 0) ? 0 : (ಅಕ್ಷರ*) uprv_malloc(buflen);
        ಆದರೆ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ಆದರೆ (U_SUCCESS(ec)) {
                ಫಲಿತಾಂಶ = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

Locale&
LocaleUtility::initLocaleFromName(ಸ್ಥಿರಾಂಕ UnicodeString& id, Locale& ಫಲಿತಾಂಶ)
{
    ಆದರೆ (id.isBogus()) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * We need to convert from a UnicodeString to char * in order to
         * create a Locale.
         *
         * Problem: Locale ID strings may contain '@' which is a variant
         * character and cannot be handled by invariant-character conversion.
         *
         * Hack: Since ICU code can handle locale IDs with multiple encodings
         * of '@' (at least for EBCDIC; it's not known to be a problem for
         * ASCII-based systems),
         * we use regular invariant-character conversion for everything else
         * and manually convert U+0040 into a compiler-char-constant '@'.
         * While this compilation-time constant may not match the runtime
         * encoding of '@', it should be one of the encodings which ICU
         * recognizes.
         *
         * There should be only at most one '@' in a locale ID.
         */
        CharString ಬಫರ್;
        int32_t prev, i;
        prev = 0;
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ಮಾಡು {
            i = id.ಸೂಚ್ಯಂಕ_ದ(static_cast<char16_t>(0x40), prev);
            ಆದರೆ(i < 0) {
                // no @ between prev and the rest of the string
                ಬಫರ್.appendInvariantChars(id.ತಾತ್ಕಾಲಿಕ_Sub_String(prev), ಸ್ಥಿತಿ_2);
                ನಿಲ್ಲಿಸು; //ಮುಗಿದ
            } ಇಲ್ಲದಿದ್ದರೆ {
                // normal invariant-character conversion for text between @s
                ಬಫರ್.appendInvariantChars(id.ತಾತ್ಕಾಲಿಕ_Sub_String(prev, i - prev), ಸ್ಥಿತಿ_2);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                ಬಫರ್.append('@', ಸ್ಥಿತಿ_2);
                prev = i + 1;
            }
        } ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2));
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = Locale::createFromName(ಬಫರ್.ದತ್ತಾಂಶ());
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ಸ್ಥಿರಾಂಕ Locale& locale, UnicodeString& ಫಲಿತಾಂಶ)
{
    ಆದರೆ (locale.isBogus()) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ Hashtable*
LocaleUtility::get_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, ಸ್ಥಿತಿ_2);
    Hashtable *cache = LocaleUtility_cache;
    ಆದರೆ (cache == ಶೂನ್ಯಸೂಚಕ) {
        // Catastrophic failure.
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    Hashtable* htp;
    umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(ಶೂನ್ಯಸೂಚಕ);

    ಆದರೆ (htp == ಶೂನ್ಯಸೂಚಕ) {
        htp = ಹೊಸ Hashtable(ಸ್ಥಿತಿ_2);
        ಆದರೆ (htp && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, ಸ್ಥಿತಿ_2);
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ = cbundleID.is_ಖಾಲಿಗಳು() ? ಶೂನ್ಯಸೂಚಕ : cbundleID.ದತ್ತಾಂಶ();
            icu::Local_UEnumeration_ಸೂಚಕ uenum(ures_open_ಲಭ್ಯ_Locales(ಮಾರ್ಗ, &ಸ್ಥಿತಿ_2));
            ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ char16_t* id = uenum_unext(uenum.getAlias(), ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
                ಆದರೆ (id == ಶೂನ್ಯಸೂಚಕ) {
                    ನಿಲ್ಲಿಸು;
                }
                htp->put(UnicodeString(id), (ಶೂನ್ಯ*)htp, ಸ್ಥಿತಿ_2);
            }
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು htp;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ಆದರೆ (t != ಶೂನ್ಯಸೂಚಕ) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
                ಅಳಿಸು htp;
                htp = t;
            } ಇಲ್ಲದಿದ್ದರೆ {
                cache->put(bundleID, (ಶೂನ್ಯ*)htp, ಸ್ಥಿತಿ_2);
                umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
            }
        }
    }
    ಹಿಂತಿರುಗಿ htp;
}

ತಾರ್ಕಿಕ
LocaleUtility::is_Fallback_ದ(ಸ್ಥಿರಾಂಕ UnicodeString& ಬೇರು, ಸ್ಥಿರಾಂಕ UnicodeString& ಮಗು)
{
    ಹಿಂತಿರುಗಿ ಮಗು.ಸೂಚ್ಯಂಕ_ದ(ಬೇರು) == 0 &&
      (ಮಗು.length() == ಬೇರು.length() ||
       ಮಗು.charAt(ಬೇರು.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ
