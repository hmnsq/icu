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
ساكن icu::Hashtable * LocaleUtility_cache = خالي_اشارو;

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV service_cleanup() {
    إذا (LocaleUtility_cache) {
        ميٽيو LocaleUtility_cache;
        LocaleUtility_cache = خالي_اشارو;
    }
    أرجع سچ;
}


ساكن فراغ U_CALLCONV locale_utility_init(UError_ڪوڊ &حالت) {
    استعمال نطاق icu;
    U_ASSERT(LocaleUtility_cache == خالي_اشارو);
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = جديد Hashtable(حالت);
    إذا (U_FAILURE(حالت)) {
        ميٽيو LocaleUtility_cache;
        LocaleUtility_cache = خالي_اشارو;
        أرجع;
    }
    إذا (LocaleUtility_cache == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    LocaleUtility_cache->سيٽ_قدر_Deleter(uhash_deleteHashtable);
}

U_CDECL_پڇاڙي

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتيجو)
{
  إذا (id == خالي_اشارو) {
    نتيجو.سيٽ_منزل_Bogus();
  } وإلا {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    نتيجو = *id;
    int32_t i = 0;
    int32_t پڇاڙي = نتيجو.اشاريو_جو(AT_SIGN_CHAR);
    int32_t n = نتيجو.اشاريو_جو(PERIOD_CHAR);
    إذا (n >= 0 && n < پڇاڙي) {
        پڇاڙي = n;
    }
    إذا (پڇاڙي < 0) {
        پڇاڙي = نتيجو.length();
    }
    n = نتيجو.اشاريو_جو(UNDERSCORE_CHAR);
    إذا (n < 0) {
      n = پڇاڙي;
    }
    لكل (; i < n; ++i) {
      char16_t c = نتيجو.charAt(i);
      إذا (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        نتيجو.سيٽ_Char_At(i, c);
      }
    }
    لكل (n = پڇاڙي; i < n; ++i) {
      char16_t c = نتيجو.charAt(i);
      إذا (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        نتيجو.سيٽ_Char_At(i, c);
      }
    }
  }
  أرجع نتيجو;

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
    نتيجو.سيٽ_منزل_Bogus();
    إذا (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        حرف* buf = (حرف*) uprv_malloc(buflen);
        حرف* canon = (buf == 0) ? 0 : (حرف*) uprv_malloc(buflen);
        إذا (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_ڪوڊ ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            إذا (U_SUCCESS(ec)) {
                نتيجو = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    أرجع نتيجو;
#جيڪڏهن_پورو
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& نتيجو)
{
    إذا (id.isBogus()) {
        نتيجو.سيٽ_منزل_Bogus();
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
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        افعل {
            i = id.اشاريو_جو(static_cast<char16_t>(0x40), prev);
            إذا(i < 0) {
                // no @ between prev and the rest of the string
                بفر.appendInvariantChars(id.عارضي_Sub_String(prev), حالت);
                اكسر; //پورو
            } وإلا {
                // normal invariant-character conversion for text between @s
                بفر.appendInvariantChars(id.عارضي_Sub_String(prev, i - prev), حالت);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                بفر.append('@', حالت);
                prev = i + 1;
            }
        } طالما (U_SUCCESS(حالت));
        إذا (U_FAILURE(حالت)) {
            نتيجو.سيٽ_منزل_Bogus();
        } وإلا {
            نتيجو = Locale::createFromName(بفر.معلومات());
        }
    }
    أرجع نتيجو;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& نتيجو)
{
    إذا (locale.isBogus()) {
        نتيجو.سيٽ_منزل_Bogus();
    } وإلا {
        نتيجو.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    أرجع نتيجو;
}

ثابت Hashtable*
LocaleUtility::get_موجود_Locale_نالا(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_ڪوڊ حالت = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, حالت);
    Hashtable *cache = LocaleUtility_cache;
    إذا (cache == خالي_اشارو) {
        // Catastrophic failure.
        أرجع خالي_اشارو;
    }

    Hashtable* htp;
    umtx_تالو(خالي_اشارو);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(خالي_اشارو);

    إذا (htp == خالي_اشارو) {
        htp = جديد Hashtable(حالت);
        إذا (htp && U_SUCCESS(حالت)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, حالت);
            ثابت حرف* رستو = cbundleID.is_خالي_جايون() ? خالي_اشارو : cbundleID.معلومات();
            icu::Local_UEnumeration_اشارو uenum(ures_open_موجود_Locales(رستو, &حالت));
            لكل (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), خالي_اشارو, &حالت);
                إذا (id == خالي_اشارو) {
                    اكسر;
                }
                htp->put(UnicodeString(id), (فراغ*)htp, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                ميٽيو htp;
                أرجع خالي_اشارو;
            }
            umtx_تالو(خالي_اشارو);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            إذا (t != خالي_اشارو) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(خالي_اشارو);
                ميٽيو htp;
                htp = t;
            } وإلا {
                cache->put(bundleID, (فراغ*)htp, حالت);
                umtx_unlock(خالي_اشارو);
            }
        }
    }
    أرجع htp;
}

منطقي
LocaleUtility::is_Fallback_جو(ثابت UnicodeString& پاڙ, ثابت UnicodeString& ٻار)
{
    أرجع ٻار.اشاريو_جو(پاڙ) == 0 &&
      (ٻار.length() == پاڙ.length() ||
       ٻار.charAt(پاڙ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو
