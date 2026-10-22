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
ساكن icu::Hashtable * LocaleUtility_cache = تش_اشاره;

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV service_cleanup() {
    إذا (LocaleUtility_cache) {
        ړنګول LocaleUtility_cache;
        LocaleUtility_cache = تش_اشاره;
    }
    أرجع ریښتیا;
}


ساكن فراغ U_CALLCONV locale_utility_init(UError_کوډ &حالت) {
    کارول نطاق icu;
    U_ASSERT(LocaleUtility_cache == تش_اشاره);
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = جديد Hashtable(حالت);
    إذا (U_FAILURE(حالت)) {
        ړنګول LocaleUtility_cache;
        LocaleUtility_cache = تش_اشاره;
        أرجع;
    }
    إذا (LocaleUtility_cache == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    LocaleUtility_cache->سېټ_ارزښت_Deleter(uhash_deleteHashtable);
}

U_CDECL_پای_ټکی

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& پایله)
{
  إذا (id == تش_اشاره) {
    پایله.سېټ_منزل_Bogus();
  } وإلا {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    پایله = *id;
    int32_t i = 0;
    int32_t پای_ټکی = پایله.شاخص_د(AT_SIGN_CHAR);
    int32_t n = پایله.شاخص_د(PERIOD_CHAR);
    إذا (n >= 0 && n < پای_ټکی) {
        پای_ټکی = n;
    }
    إذا (پای_ټکی < 0) {
        پای_ټکی = پایله.length();
    }
    n = پایله.شاخص_د(UNDERSCORE_CHAR);
    إذا (n < 0) {
      n = پای_ټکی;
    }
    لكل (; i < n; ++i) {
      char16_t c = پایله.charAt(i);
      إذا (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        پایله.سېټ_Char_At(i, c);
      }
    }
    لكل (n = پای_ټکی; i < n; ++i) {
      char16_t c = پایله.charAt(i);
      إذا (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        پایله.سېټ_Char_At(i, c);
      }
    }
  }
  أرجع پایله;

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
    پایله.سېټ_منزل_Bogus();
    إذا (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        حرف* buf = (حرف*) uprv_malloc(buflen);
        حرف* canon = (buf == 0) ? 0 : (حرف*) uprv_malloc(buflen);
        إذا (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_کوډ ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            إذا (U_SUCCESS(ec)) {
                پایله = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    أرجع پایله;
#د_که_پای
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& پایله)
{
    إذا (id.isBogus()) {
        پایله.سېټ_منزل_Bogus();
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
        UError_کوډ حالت = U_ZERO_ERROR;
        افعل {
            i = id.شاخص_د(static_cast<char16_t>(0x40), prev);
            إذا(i < 0) {
                // no @ between prev and the rest of the string
                بفر.appendInvariantChars(id.لنډمهاله_Sub_String(prev), حالت);
                اكسر; //بشپړ
            } وإلا {
                // normal invariant-character conversion for text between @s
                بفر.appendInvariantChars(id.لنډمهاله_Sub_String(prev, i - prev), حالت);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                بفر.append('@', حالت);
                prev = i + 1;
            }
        } طالما (U_SUCCESS(حالت));
        إذا (U_FAILURE(حالت)) {
            پایله.سېټ_منزل_Bogus();
        } وإلا {
            پایله = Locale::createFromName(بفر.معلومات());
        }
    }
    أرجع پایله;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& پایله)
{
    إذا (locale.isBogus()) {
        پایله.سېټ_منزل_Bogus();
    } وإلا {
        پایله.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    أرجع پایله;
}

ثابت Hashtable*
LocaleUtility::get_شته_Locale_نومونه(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_کوډ حالت = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, حالت);
    Hashtable *cache = LocaleUtility_cache;
    إذا (cache == تش_اشاره) {
        // Catastrophic failure.
        أرجع تش_اشاره;
    }

    Hashtable* htp;
    umtx_قلف(تش_اشاره);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(تش_اشاره);

    إذا (htp == تش_اشاره) {
        htp = جديد Hashtable(حالت);
        إذا (htp && U_SUCCESS(حالت)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, حالت);
            ثابت حرف* لار = cbundleID.is_تشه() ? تش_اشاره : cbundleID.معلومات();
            icu::Local_UEnumeration_اشاره_ګر uenum(ures_open_شته_Locales(لار, &حالت));
            لكل (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), تش_اشاره, &حالت);
                إذا (id == تش_اشاره) {
                    اكسر;
                }
                htp->put(UnicodeString(id), (فراغ*)htp, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                ړنګول htp;
                أرجع تش_اشاره;
            }
            umtx_قلف(تش_اشاره);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            إذا (t != تش_اشاره) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(تش_اشاره);
                ړنګول htp;
                htp = t;
            } وإلا {
                cache->put(bundleID, (فراغ*)htp, حالت);
                umtx_unlock(تش_اشاره);
            }
        }
    }
    أرجع htp;
}

منطقي
LocaleUtility::is_Fallback_د(ثابت UnicodeString& ریښه, ثابت UnicodeString& ماشوم)
{
    أرجع ماشوم.شاخص_د(ریښه) == 0 &&
      (ماشوم.length() == ریښه.length() ||
       ماشوم.charAt(ریښه.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای
