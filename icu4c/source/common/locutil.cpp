// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#شامل "unicode/resbund.h"
#شامل "unicode/uenum.h"
#شامل "cmemory.h"
#شامل "ustrfmt.h"
#شامل "locutil.h"
#شامل "charstr.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"
#شامل "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
ساکن icu::UInitOnce   LocaleUtilityInitOnce {};
ساکن icu::Hashtable * LocaleUtility_cache = خالی_اشاریہ;

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
ساکن UBool U_CALLCONV service_cleanup() {
    اگر (LocaleUtility_cache) {
        حذف LocaleUtility_cache;
        LocaleUtility_cache = خالی_اشاریہ;
    }
    واپس سچ;
}


ساکن خالی U_CALLCONV locale_utility_init(UError_رمز &حالت_2) {
    استعمال نامخلا icu;
    U_ASSERT(LocaleUtility_cache == خالی_اشاریہ);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = نیا Hashtable(حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        حذف LocaleUtility_cache;
        LocaleUtility_cache = خالی_اشاریہ;
        واپس;
    }
    اگر (LocaleUtility_cache == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    LocaleUtility_cache->سیٹ_قدر_Deleter(uhash_deleteHashtable);
}

U_CDECL_انتہا

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجہ)
{
  اگر (id == خالی_اشاریہ) {
    نتیجہ.سیٹ_مقصد_Bogus();
  } ورنہ {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    نتیجہ = *id;
    int32_t i = 0;
    int32_t انتہا = نتیجہ.اشاریہ_کا(AT_SIGN_CHAR);
    int32_t n = نتیجہ.اشاریہ_کا(PERIOD_CHAR);
    اگر (n >= 0 && n < انتہا) {
        انتہا = n;
    }
    اگر (انتہا < 0) {
        انتہا = نتیجہ.length();
    }
    n = نتیجہ.اشاریہ_کا(UNDERSCORE_CHAR);
    اگر (n < 0) {
      n = انتہا;
    }
    برائے (; i < n; ++i) {
      char16_t c = نتیجہ.charAt(i);
      اگر (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        نتیجہ.سیٹ_Char_At(i, c);
      }
    }
    برائے (n = انتہا; i < n; ++i) {
      char16_t c = نتیجہ.charAt(i);
      اگر (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        نتیجہ.سیٹ_Char_At(i, c);
      }
    }
  }
  واپس نتیجہ;

#اگر 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    نتیجہ.سیٹ_مقصد_Bogus();
    اگر (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        حرف* buf = (حرف*) uprv_malloc(buflen);
        حرف* canon = (buf == 0) ? 0 : (حرف*) uprv_malloc(buflen);
        اگر (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_رمز ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            اگر (U_SUCCESS(ec)) {
                نتیجہ = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    واپس نتیجہ;
#اگر_ختم
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجہ)
{
    اگر (id.isBogus()) {
        نتیجہ.سیٹ_مقصد_Bogus();
    } ورنہ {
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
        int32_t پچھلا, i;
        پچھلا = 0;
        UError_رمز حالت_2 = U_ZERO_ERROR;
        کرو {
            i = id.اشاریہ_کا(static_cast<char16_t>(0x40), پچھلا);
            اگر(i < 0) {
                // no @ between prev and the rest of the string
                بفر.appendInvariantChars(id.عارضی_Sub_String(پچھلا), حالت_2);
                توڑو; //مکمل
            } ورنہ {
                // normal invariant-character conversion for text between @s
                بفر.appendInvariantChars(id.عارضی_Sub_String(پچھلا, i - پچھلا), حالت_2);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                بفر.append('@', حالت_2);
                پچھلا = i + 1;
            }
        } جب (U_SUCCESS(حالت_2));
        اگر (U_FAILURE(حالت_2)) {
            نتیجہ.سیٹ_مقصد_Bogus();
        } ورنہ {
            نتیجہ = Locale::createFromName(بفر.ڈیٹا());
        }
    }
    واپس نتیجہ;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجہ)
{
    اگر (locale.isBogus()) {
        نتیجہ.سیٹ_مقصد_Bogus();
    } ورنہ {
        نتیجہ.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    واپس نتیجہ;
}

ثابت Hashtable*
LocaleUtility::get_دستیاب_Locale_نام(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_رمز حالت_2 = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, حالت_2);
    Hashtable *cache = LocaleUtility_cache;
    اگر (cache == خالی_اشاریہ) {
        // Catastrophic failure.
        واپس خالی_اشاریہ;
    }

    Hashtable* htp;
    umtx_تالا(خالی_اشاریہ);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(خالی_اشاریہ);

    اگر (htp == خالی_اشاریہ) {
        htp = نیا Hashtable(حالت_2);
        اگر (htp && U_SUCCESS(حالت_2)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, حالت_2);
            ثابت حرف* راستہ = cbundleID.is_خالی_خانے() ? خالی_اشاریہ : cbundleID.ڈیٹا();
            icu::Local_UEnumeration_اشارہ_گر uenum(ures_open_دستیاب_Locales(راستہ, &حالت_2));
            برائے (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), خالی_اشاریہ, &حالت_2);
                اگر (id == خالی_اشاریہ) {
                    توڑو;
                }
                htp->put(UnicodeString(id), (خالی*)htp, حالت_2);
            }
            اگر (U_FAILURE(حالت_2)) {
                حذف htp;
                واپس خالی_اشاریہ;
            }
            umtx_تالا(خالی_اشاریہ);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            اگر (t != خالی_اشاریہ) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(خالی_اشاریہ);
                حذف htp;
                htp = t;
            } ورنہ {
                cache->put(bundleID, (خالی*)htp, حالت_2);
                umtx_unlock(خالی_اشاریہ);
            }
        }
    }
    واپس htp;
}

منطقی
LocaleUtility::is_Fallback_کا(ثابت UnicodeString& جڑ, ثابت UnicodeString& بچہ)
{
    واپس بچہ.اشاریہ_کا(جڑ) == 0 &&
      (بچہ.length() == جڑ.length() ||
       بچہ.charAt(جڑ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم
