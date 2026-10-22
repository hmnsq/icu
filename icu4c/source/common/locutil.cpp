// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uenum.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "locutil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
ସ୍ଥିର icu::UInitOnce   LocaleUtilityInitOnce {};
ସ୍ଥିର icu::Hashtable * LocaleUtility_cache = nullptr;

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
ସ୍ଥିର UBool U_CALLCONV service_cleanup() {
    ଯଦି (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV locale_utility_init(UErrorCode &status) {
    using ନାମାକାଶ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = ନୂତନ Hashtable(status);
    ଯଦି (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    LocaleUtility_cache->set_ମୂଲ୍ୟ_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ଅଚଳ UnicodeString* id, UnicodeString& result)
{
  ଯଦି (id == nullptr) {
    result.setToBogus();
  } ଅନ୍ୟଥା {
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
    ଯଦି (n >= 0 && n < end) {
        end = n;
    }
    ଯଦି (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    ଯଦି (n < 0) {
      n = end;
    }
    ପାଇଁ (; i < n; ++i) {
      char16_t c = result.charAt(i);
      ଯଦି (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    ପାଇଁ (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      ଯଦି (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  ଫେରନ୍ତୁ result;

#ଯଦି 0
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
    ଯଦି (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        ଅକ୍ଷର* buf = (ଅକ୍ଷର*) uprv_malloc(buflen);
        ଅକ୍ଷର* canon = (buf == 0) ? 0 : (ଅକ୍ଷର*) uprv_malloc(buflen);
        ଯଦି (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ଯଦି (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    ଫେରନ୍ତୁ result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ଅଚଳ UnicodeString& id, Locale& result)
{
    ଯଦି (id.isBogus()) {
        result.setToBogus();
    } ଅନ୍ୟଥା {
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
        କର {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            ଯଦି(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                ଭାଙ୍ଗ; // done
            } ଅନ୍ୟଥା {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } ଯେତେବେଳେ (U_SUCCESS(status));
        ଯଦି (U_FAILURE(status)) {
            result.setToBogus();
        } ଅନ୍ୟଥା {
            result = Locale::createFromName(buffer.data());
        }
    }
    ଫେରନ୍ତୁ result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ଅଚଳ Locale& locale, UnicodeString& result)
{
    ଯଦି (locale.isBogus()) {
        result.setToBogus();
    } ଅନ୍ୟଥା {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    ଫେରନ୍ତୁ result;
}

ଅଚଳ Hashtable*
LocaleUtility::getAvailableLocaleNames(ଅଚଳ UnicodeString& bundleID)
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
    ଯଦି (cache == nullptr) {
        // Catastrophic failure.
        ଫେରନ୍ତୁ nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    ଯଦି (htp == nullptr) {
        htp = ନୂତନ Hashtable(status);
        ଯଦି (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ଅଚଳ ଅକ୍ଷର* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            ପାଇଁ (;;) {
                ଅଚଳ char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                ଯଦି (id == nullptr) {
                    ଭାଙ୍ଗ;
                }
                htp->put(UnicodeString(id), (ଶୂନ୍ୟ*)htp, status);
            }
            ଯଦି (U_FAILURE(status)) {
                delete htp;
                ଫେରନ୍ତୁ nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ଯଦି (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } ଅନ୍ୟଥା {
                cache->put(bundleID, (ଶୂନ୍ୟ*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    ଫେରନ୍ତୁ htp;
}

ତାର୍କିକ
LocaleUtility::isFallbackOf(ଅଚଳ UnicodeString& ମୂଳ, ଅଚଳ UnicodeString& child)
{
    ଫେରନ୍ତୁ child.indexOf(ମୂଳ) == 0 &&
      (child.length() == ମୂଳ.length() ||
       child.charAt(ମୂଳ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
