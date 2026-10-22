// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#ضمن "unicode/resbund.h"
#ضمن "unicode/uenum.h"
#ضمن "cmemory.h"
#ضمن "ustrfmt.h"
#ضمن "locutil.h"
#ضمن "charstr.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"
#ضمن "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
ساكن icu::UInitOnce   LocaleUtilityInitOnce {};
ساكن icu::Hashtable * LocaleUtility_cache = خالی_سوچک;

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV service_cleanup() {
    إذا (LocaleUtility_cache) {
        مٹاؤ LocaleUtility_cache;
        LocaleUtility_cache = خالی_سوچک;
    }
    أرجع سچ;
}


ساكن فراغ U_CALLCONV locale_utility_init(UError_کوڈ &حالت) {
    ورتوں نطاق icu;
    U_ASSERT(LocaleUtility_cache == خالی_سوچک);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = جديد Hashtable(حالت);
    إذا (U_FAILURE(حالت)) {
        مٹاؤ LocaleUtility_cache;
        LocaleUtility_cache = خالی_سوچک;
        أرجع;
    }
    إذا (LocaleUtility_cache == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    LocaleUtility_cache->سیٹ_قدر_Deleter(uhash_deleteHashtable);
}

U_CDECL_سرا

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجہ)
{
  إذا (id == خالی_سوچک) {
    نتیجہ.سیٹ_منزل_Bogus();
  } وإلا {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    نتیجہ = *id;
    int32_t i = 0;
    int32_t سرا = نتیجہ.اشاریہ_دا(AT_SIGN_CHAR);
    int32_t n = نتیجہ.اشاریہ_دا(PERIOD_CHAR);
    إذا (n >= 0 && n < سرا) {
        سرا = n;
    }
    إذا (سرا < 0) {
        سرا = نتیجہ.length();
    }
    n = نتیجہ.اشاریہ_دا(UNDERSCORE_CHAR);
    إذا (n < 0) {
      n = سرا;
    }
    لكل (; i < n; ++i) {
      char16_t c = نتیجہ.charAt(i);
      إذا (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        نتیجہ.سیٹ_Char_At(i, c);
      }
    }
    لكل (n = سرا; i < n; ++i) {
      char16_t c = نتیجہ.charAt(i);
      إذا (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        نتیجہ.سیٹ_Char_At(i, c);
      }
    }
  }
  أرجع نتیجہ;

#إذا 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    نتیجہ.سیٹ_منزل_Bogus();
    إذا (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        حرف* buf = (حرف*) uprv_malloc(buflen);
        حرف* canon = (buf == 0) ? 0 : (حرف*) uprv_malloc(buflen);
        إذا (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_کوڈ ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            إذا (U_SUCCESS(ec)) {
                نتیجہ = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    أرجع نتیجہ;
#جے_انت
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجہ)
{
    إذا (id.isBogus()) {
        نتیجہ.سیٹ_منزل_Bogus();
    } وإلا {
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
        CharString بفر;
        int32_t prev, i;
        prev = 0;
        UError_کوڈ حالت = U_ZERO_ERROR;
        افعل {
            i = id.اشاریہ_دا(static_cast<char16_t>(0x40), prev);
            إذا(i < 0) {
                // no @ between prev and the rest of the string
                بفر.appendInvariantChars(id.عارضی_Sub_String(prev), حالت);
                اكسر; //پورا
            } وإلا {
                // normal invariant-character conversion for text between @s
                بفر.appendInvariantChars(id.عارضی_Sub_String(prev, i - prev), حالت);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                بفر.append('@', حالت);
                prev = i + 1;
            }
        } طالما (U_SUCCESS(حالت));
        إذا (U_FAILURE(حالت)) {
            نتیجہ.سیٹ_منزل_Bogus();
        } وإلا {
            نتیجہ = Locale::createFromName(بفر.انکڑے());
        }
    }
    أرجع نتیجہ;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجہ)
{
    إذا (locale.isBogus()) {
        نتیجہ.سیٹ_منزل_Bogus();
    } وإلا {
        نتیجہ.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    أرجع نتیجہ;
}

ثابت Hashtable*
LocaleUtility::get_دستیاب_Locale_ناں(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_کوڈ حالت = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, حالت);
    Hashtable *cache = LocaleUtility_cache;
    إذا (cache == خالی_سوچک) {
        // Catastrophic failure.
        أرجع خالی_سوچک;
    }

    Hashtable* htp;
    umtx_جندرا(خالی_سوچک);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(خالی_سوچک);

    إذا (htp == خالی_سوچک) {
        htp = جديد Hashtable(حالت);
        إذا (htp && U_SUCCESS(حالت)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, حالت);
            ثابت حرف* رستہ = cbundleID.is_خالی_تھاواں() ? خالی_سوچک : cbundleID.انکڑے();
            icu::Local_UEnumeration_اشارہ_گر uenum(ures_open_دستیاب_Locales(رستہ, &حالت));
            لكل (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), خالی_سوچک, &حالت);
                إذا (id == خالی_سوچک) {
                    اكسر;
                }
                htp->put(UnicodeString(id), (فراغ*)htp, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                مٹاؤ htp;
                أرجع خالی_سوچک;
            }
            umtx_جندرا(خالی_سوچک);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            إذا (t != خالی_سوچک) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(خالی_سوچک);
                مٹاؤ htp;
                htp = t;
            } وإلا {
                cache->put(bundleID, (فراغ*)htp, حالت);
                umtx_unlock(خالی_سوچک);
            }
        }
    }
    أرجع htp;
}

منطقي
LocaleUtility::is_Fallback_دا(ثابت UnicodeString& جڑ, ثابت UnicodeString& بچہ)
{
    أرجع بچہ.اشاریہ_دا(جڑ) == 0 &&
      (بچہ.length() == جڑ.length() ||
       بچہ.charAt(جڑ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت
