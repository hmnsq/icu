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
ایستا icu::UInitOnce   LocaleUtilityInitOnce {};
ایستا icu::Hashtable * LocaleUtility_cache = nullptr;

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
ایستا UBool U_CALLCONV service_cleanup() {
    اگر (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    بازگشت درست;
}


ایستا تهی U_CALLCONV locale_utility_init(UErrorCode &status) {
    using فضاینام icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = جدید Hashtable(status);
    اگر (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        بازگشت;
    }
    اگر (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    LocaleUtility_cache->set_مقدار_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& result)
{
  اگر (id == nullptr) {
    result.setToBogus();
  } وگرنه {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    result = *id;
    int32_t i = 0;
    int32_t end = result.indexOf(AT_SIGN_CHAR);
    int32_t n = result.indexOf(PERIOD_CHAR);
    اگر (n >= 0 && n < end) {
        end = n;
    }
    اگر (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    اگر (n < 0) {
      n = end;
    }
    برای (; i < n; ++i) {
      char16_t c = result.charAt(i);
      اگر (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    برای (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      اگر (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  بازگشت result;

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
    result.setToBogus();
    اگر (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        نویسه* buf = (نویسه*) uprv_malloc(buflen);
        نویسه* canon = (buf == 0) ? 0 : (نویسه*) uprv_malloc(buflen);
        اگر (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            اگر (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    بازگشت result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ثابت UnicodeString& id, Locale& result)
{
    اگر (id.isBogus()) {
        result.setToBogus();
    } وگرنه {
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
        CharString buffer;
        int32_t prev, i;
        prev = 0;
        UErrorCode status = U_ZERO_ERROR;
        انجامبده {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            اگر(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                بشکن; // done
            } وگرنه {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } وقتی (U_SUCCESS(status));
        اگر (U_FAILURE(status)) {
            result.setToBogus();
        } وگرنه {
            result = Locale::createFromName(buffer.data());
        }
    }
    بازگشت result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ثابت Locale& locale, UnicodeString& result)
{
    اگر (locale.isBogus()) {
        result.setToBogus();
    } وگرنه {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    بازگشت result;
}

ثابت Hashtable*
LocaleUtility::getAvailableLocaleNames(ثابت UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UErrorCode status = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, status);
    Hashtable *cache = LocaleUtility_cache;
    اگر (cache == nullptr) {
        // Catastrophic failure.
        بازگشت nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    اگر (htp == nullptr) {
        htp = جدید Hashtable(status);
        اگر (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ثابت نویسه* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            برای (;;) {
                ثابت char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                اگر (id == nullptr) {
                    بشکن;
                }
                htp->put(UnicodeString(id), (تهی*)htp, status);
            }
            اگر (U_FAILURE(status)) {
                delete htp;
                بازگشت nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            اگر (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } وگرنه {
                cache->put(bundleID, (تهی*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    بازگشت htp;
}

منطقی
LocaleUtility::isFallbackOf(ثابت UnicodeString& ریشه, ثابت UnicodeString& child)
{
    بازگشت child.indexOf(ریشه) == 0 &&
      (child.length() == ریشه.length() ||
       child.charAt(ریشه.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
