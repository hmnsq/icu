// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
ਸਥਿਰ icu::UInitOnce   LocaleUtilityInitOnce {};
ਸਥਿਰ icu::Hashtable * LocaleUtility_cache = ਖਾਲੀ_ਸੂਚਕ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV service_cleanup() {
    ਜੇ (LocaleUtility_cache) {
        ਮਿਟਾਓ LocaleUtility_cache;
        LocaleUtility_cache = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਸੱਚ;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV locale_utility_init(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu;
    U_ASSERT(LocaleUtility_cache == ਖਾਲੀ_ਸੂਚਕ);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = ਨਵਾਂ Hashtable(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ LocaleUtility_cache;
        LocaleUtility_cache = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ;
    }
    ਜੇ (LocaleUtility_cache == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    LocaleUtility_cache->ਸੈੱਟ_ਮੁੱਲ_Deleter(uhash_deleteHashtable);
}

U_CDECL_ਸਿਰਾ

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ਅਚਲ UnicodeString* id, UnicodeString& ਨਤੀਜਾ)
{
  ਜੇ (id == ਖਾਲੀ_ਸੂਚਕ) {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  } ਨਹੀਂਤਾਂ {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    ਨਤੀਜਾ = *id;
    int32_t i = 0;
    int32_t ਸਿਰਾ = ਨਤੀਜਾ.ਸੂਚਕਾਂਕ_ਦਾ(AT_SIGN_CHAR);
    int32_t n = ਨਤੀਜਾ.ਸੂਚਕਾਂਕ_ਦਾ(PERIOD_CHAR);
    ਜੇ (n >= 0 && n < ਸਿਰਾ) {
        ਸਿਰਾ = n;
    }
    ਜੇ (ਸਿਰਾ < 0) {
        ਸਿਰਾ = ਨਤੀਜਾ.length();
    }
    n = ਨਤੀਜਾ.ਸੂਚਕਾਂਕ_ਦਾ(UNDERSCORE_CHAR);
    ਜੇ (n < 0) {
      n = ਸਿਰਾ;
    }
    ਲਈ (; i < n; ++i) {
      char16_t c = ਨਤੀਜਾ.charAt(i);
      ਜੇ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        ਨਤੀਜਾ.ਸੈੱਟ_Char_At(i, c);
      }
    }
    ਲਈ (n = ਸਿਰਾ; i < n; ++i) {
      char16_t c = ਨਤੀਜਾ.charAt(i);
      ਜੇ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        ਨਤੀਜਾ.ਸੈੱਟ_Char_At(i, c);
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਜੇ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        ਅੱਖਰ* buf = (ਅੱਖਰ*) uprv_malloc(buflen);
        ਅੱਖਰ* canon = (buf == 0) ? 0 : (ਅੱਖਰ*) uprv_malloc(buflen);
        ਜੇ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_ਕੋਡ ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ਜੇ (U_SUCCESS(ec)) {
                ਨਤੀਜਾ = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
#ਜੇ_ਅੰਤ
}

Locale&
LocaleUtility::initLocaleFromName(ਅਚਲ UnicodeString& id, Locale& ਨਤੀਜਾ)
{
    ਜੇ (id.isBogus()) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    } ਨਹੀਂਤਾਂ {
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
        CharString ਬਫਰ;
        int32_t prev, i;
        prev = 0;
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ਕਰੋ {
            i = id.ਸੂਚਕਾਂਕ_ਦਾ(static_cast<char16_t>(0x40), prev);
            ਜੇ(i < 0) {
                // no @ between prev and the rest of the string
                ਬਫਰ.appendInvariantChars(id.ਆਰਜ਼ੀ_Sub_String(prev), ਸਥਿਤੀ);
                ਰੋਕੋ; //ਪੂਰਾ
            } ਨਹੀਂਤਾਂ {
                // normal invariant-character conversion for text between @s
                ਬਫਰ.appendInvariantChars(id.ਆਰਜ਼ੀ_Sub_String(prev, i - prev), ਸਥਿਤੀ);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                ਬਫਰ.append('@', ਸਥਿਤੀ);
                prev = i + 1;
            }
        } ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ));
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        } ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = Locale::createFromName(ਬਫਰ.ਅੰਕੜੇ());
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ਅਚਲ Locale& locale, UnicodeString& ਨਤੀਜਾ)
{
    ਜੇ (locale.isBogus()) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ Hashtable*
LocaleUtility::get_ਉਪਲਬਧ_Locale_ਨਾਂ(ਅਚਲ UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, ਸਥਿਤੀ);
    Hashtable *cache = LocaleUtility_cache;
    ਜੇ (cache == ਖਾਲੀ_ਸੂਚਕ) {
        // Catastrophic failure.
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    Hashtable* htp;
    umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);

    ਜੇ (htp == ਖਾਲੀ_ਸੂਚਕ) {
        htp = ਨਵਾਂ Hashtable(ਸਥਿਤੀ);
        ਜੇ (htp && U_SUCCESS(ਸਥਿਤੀ)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, ਸਥਿਤੀ);
            ਅਚਲ ਅੱਖਰ* ਰਸਤਾ = cbundleID.is_ਖਾਲੀ_ਥਾਵਾਂ() ? ਖਾਲੀ_ਸੂਚਕ : cbundleID.ਅੰਕੜੇ();
            icu::Local_UEnumeration_ਸੂਚਕ uenum(ures_open_ਉਪਲਬਧ_Locales(ਰਸਤਾ, &ਸਥਿਤੀ));
            ਲਈ (;;) {
                ਅਚਲ char16_t* id = uenum_unext(uenum.getAlias(), ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
                ਜੇ (id == ਖਾਲੀ_ਸੂਚਕ) {
                    ਰੋਕੋ;
                }
                htp->put(UnicodeString(id), (ਖਾਲੀ*)htp, ਸਥਿਤੀ);
            }
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ htp;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ਜੇ (t != ਖਾਲੀ_ਸੂਚਕ) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
                ਮਿਟਾਓ htp;
                htp = t;
            } ਨਹੀਂਤਾਂ {
                cache->put(bundleID, (ਖਾਲੀ*)htp, ਸਥਿਤੀ);
                umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
            }
        }
    }
    ਵਾਪਸ htp;
}

ਤਾਰਕਿਕ
LocaleUtility::is_Fallback_ਦਾ(ਅਚਲ UnicodeString& ਜੜ੍ਹ, ਅਚਲ UnicodeString& ਬੱਚਾ)
{
    ਵਾਪਸ ਬੱਚਾ.ਸੂਚਕਾਂਕ_ਦਾ(ਜੜ੍ਹ) == 0 &&
      (ਬੱਚਾ.length() == ਜੜ੍ਹ.length() ||
       ਬੱਚਾ.charAt(ਜੜ੍ਹ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ
