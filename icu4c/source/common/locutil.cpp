// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

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
ಸ್ಥಿರ icu::Hashtable * LocaleUtility_cache = nullptr;

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
ಸ್ಥಿರ UBool U_CALLCONV service_cleanup() {
    ಯದಿ (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV locale_utility_init(UErrorCode &status) {
    using ನಾಮಾಕಾಶ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = ನೂತನ Hashtable(status);
    ಯದಿ (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    LocaleUtility_cache->set_ಮೌಲ್ಯ_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ಧ್ರುವ UnicodeString* id, UnicodeString& result)
{
  ಯದಿ (id == nullptr) {
    result.setToBogus();
  } ಅನ್ಯಥಾ {
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
    ಯದಿ (n >= 0 && n < end) {
        end = n;
    }
    ಯದಿ (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    ಯದಿ (n < 0) {
      n = end;
    }
    ಪ್ರತಿ (; i < n; ++i) {
      char16_t c = result.charAt(i);
      ಯದಿ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    ಪ್ರತಿ (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      ಯದಿ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;

#ಯದಿ 0
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
    ಯದಿ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        ವರ್ಣ* buf = (ವರ್ಣ*) uprv_malloc(buflen);
        ವರ್ಣ* canon = (buf == 0) ? 0 : (ವರ್ಣ*) uprv_malloc(buflen);
        ಯದಿ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ಯದಿ (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    ಪ್ರತ್ಯಾಗಮ result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ಧ್ರುವ UnicodeString& id, Locale& result)
{
    ಯದಿ (id.isBogus()) {
        result.setToBogus();
    } ಅನ್ಯಥಾ {
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
        ಕುರು {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            ಯದಿ(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                ಭಙ್ಗ; // done
            } ಅನ್ಯಥಾ {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } ಯಾವತ್ (U_SUCCESS(status));
        ಯದಿ (U_FAILURE(status)) {
            result.setToBogus();
        } ಅನ್ಯಥಾ {
            result = Locale::createFromName(buffer.data());
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ಧ್ರುವ Locale& locale, UnicodeString& result)
{
    ಯದಿ (locale.isBogus()) {
        result.setToBogus();
    } ಅನ್ಯಥಾ {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಧ್ರುವ Hashtable*
LocaleUtility::getAvailableLocaleNames(ಧ್ರುವ UnicodeString& bundleID)
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
    ಯದಿ (cache == nullptr) {
        // Catastrophic failure.
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    ಯದಿ (htp == nullptr) {
        htp = ನೂತನ Hashtable(status);
        ಯದಿ (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ಧ್ರುವ ವರ್ಣ* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            ಪ್ರತಿ (;;) {
                ಧ್ರುವ char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                ಯದಿ (id == nullptr) {
                    ಭಙ್ಗ;
                }
                htp->put(UnicodeString(id), (ಶೂನ್ಯ*)htp, status);
            }
            ಯದಿ (U_FAILURE(status)) {
                delete htp;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ಯದಿ (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } ಅನ್ಯಥಾ {
                cache->put(bundleID, (ಶೂನ್ಯ*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ htp;
}

ತಾರ್ಕಿಕ
LocaleUtility::isFallbackOf(ಧ್ರುವ UnicodeString& ಬೇರು, ಧ್ರುವ UnicodeString& child)
{
    ಪ್ರತ್ಯಾಗಮ child.indexOf(ಬೇರು) == 0 &&
      (child.length() == ಬೇರು.length() ||
       child.charAt(ಬೇರು.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
