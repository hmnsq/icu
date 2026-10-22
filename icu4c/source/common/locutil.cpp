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
ساكن icu::Hashtable * LocaleUtility_cache = اشاره_تهی;

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV service_cleanup() {
    إذا (LocaleUtility_cache) {
        حذف LocaleUtility_cache;
        LocaleUtility_cache = اشاره_تهی;
    }
    أرجع درست;
}


ساكن فراغ U_CALLCONV locale_utility_init(UError_کد &وضعیت) {
    استفاده نطاق icu;
    U_ASSERT(LocaleUtility_cache == اشاره_تهی);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = جديد Hashtable(وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        حذف LocaleUtility_cache;
        LocaleUtility_cache = اشاره_تهی;
        أرجع;
    }
    إذا (LocaleUtility_cache == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    LocaleUtility_cache->مجموعه_مقدار_Deleter(uhash_deleteHashtable);
}

U_CDECL_انتها

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجه)
{
  إذا (id == اشاره_تهی) {
    نتیجه.مجموعه_مقصد_Bogus();
  } وإلا {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    نتیجه = *id;
    int32_t i = 0;
    int32_t انتها = نتیجه.شاخص_از(AT_SIGN_CHAR);
    int32_t n = نتیجه.شاخص_از(PERIOD_CHAR);
    إذا (n >= 0 && n < انتها) {
        انتها = n;
    }
    إذا (انتها < 0) {
        انتها = نتیجه.length();
    }
    n = نتیجه.شاخص_از(UNDERSCORE_CHAR);
    إذا (n < 0) {
      n = انتها;
    }
    لكل (; i < n; ++i) {
      char16_t c = نتیجه.charAt(i);
      إذا (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        نتیجه.مجموعه_Char_At(i, c);
      }
    }
    لكل (n = انتها; i < n; ++i) {
      char16_t c = نتیجه.charAt(i);
      إذا (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        نتیجه.مجموعه_Char_At(i, c);
      }
    }
  }
  أرجع نتیجه;

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
    نتیجه.مجموعه_مقصد_Bogus();
    إذا (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        حرف* buf = (حرف*) uprv_malloc(buflen);
        حرف* canon = (buf == 0) ? 0 : (حرف*) uprv_malloc(buflen);
        إذا (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_کد ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            إذا (U_SUCCESS(ec)) {
                نتیجه = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    أرجع نتیجه;
#پایان_اگر
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجه)
{
    إذا (id.isBogus()) {
        نتیجه.مجموعه_مقصد_Bogus();
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
        CharString میانگیر;
        int32_t prev, i;
        prev = 0;
        UError_کد وضعیت = U_ZERO_ERROR;
        افعل {
            i = id.شاخص_از(static_cast<char16_t>(0x40), prev);
            إذا(i < 0) {
                // no @ between prev and the rest of the string
                میانگیر.appendInvariantChars(id.موقت_Sub_String(prev), وضعیت);
                اكسر; //انجامشده
            } وإلا {
                // normal invariant-character conversion for text between @s
                میانگیر.appendInvariantChars(id.موقت_Sub_String(prev, i - prev), وضعیت);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                میانگیر.append('@', وضعیت);
                prev = i + 1;
            }
        } طالما (U_SUCCESS(وضعیت));
        إذا (U_FAILURE(وضعیت)) {
            نتیجه.مجموعه_مقصد_Bogus();
        } وإلا {
            نتیجه = Locale::createFromName(میانگیر.داده());
        }
    }
    أرجع نتیجه;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجه)
{
    إذا (locale.isBogus()) {
        نتیجه.مجموعه_مقصد_Bogus();
    } وإلا {
        نتیجه.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    أرجع نتیجه;
}

ثابت Hashtable*
LocaleUtility::get_موجود_Locale_نامها(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_کد وضعیت = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, وضعیت);
    Hashtable *cache = LocaleUtility_cache;
    إذا (cache == اشاره_تهی) {
        // Catastrophic failure.
        أرجع اشاره_تهی;
    }

    Hashtable* htp;
    umtx_قفل(اشاره_تهی);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(اشاره_تهی);

    إذا (htp == اشاره_تهی) {
        htp = جديد Hashtable(وضعیت);
        إذا (htp && U_SUCCESS(وضعیت)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, وضعیت);
            ثابت حرف* مسیر = cbundleID.is_خالی() ? اشاره_تهی : cbundleID.داده();
            icu::Local_UEnumeration_اشاره_گر uenum(ures_open_موجود_Locales(مسیر, &وضعیت));
            لكل (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), اشاره_تهی, &وضعیت);
                إذا (id == اشاره_تهی) {
                    اكسر;
                }
                htp->put(UnicodeString(id), (فراغ*)htp, وضعیت);
            }
            إذا (U_FAILURE(وضعیت)) {
                حذف htp;
                أرجع اشاره_تهی;
            }
            umtx_قفل(اشاره_تهی);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            إذا (t != اشاره_تهی) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(اشاره_تهی);
                حذف htp;
                htp = t;
            } وإلا {
                cache->put(bundleID, (فراغ*)htp, وضعیت);
                umtx_unlock(اشاره_تهی);
            }
        }
    }
    أرجع htp;
}

منطقي
LocaleUtility::is_Fallback_از(ثابت UnicodeString& ریشه, ثابت UnicodeString& فرزند)
{
    أرجع فرزند.شاخص_از(ریشه) == 0 &&
      (فرزند.length() == ریشه.length() ||
       فرزند.charAt(ریشه.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر
