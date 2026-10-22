// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/uenum.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "locutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
स्थिर icu::UInitOnce   LocaleUtilityInitOnce {};
स्थिर icu::Hashtable * LocaleUtility_cache = nullptr;

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
स्थिर UBool U_CALLCONV service_cleanup() {
    सचे (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    पटिनिवत्त सच्च;
}


स्थिर शून्य U_CALLCONV locale_utility_init(UErrorCode &status) {
    using नामाकाश icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = नूतन Hashtable(status);
    सचे (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        पटिनिवत्त;
    }
    सचे (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    LocaleUtility_cache->set_मानम्_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ध्रुव UnicodeString* id, UnicodeString& result)
{
  सचे (id == nullptr) {
    result.setToBogus();
  } अञ्ञथा {
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
    सचे (n >= 0 && n < end) {
        end = n;
    }
    सचे (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    सचे (n < 0) {
      n = end;
    }
    अत्थाय (; i < n; ++i) {
      char16_t c = result.charAt(i);
      सचे (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    अत्थाय (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      सचे (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  पटिनिवत्त result;

#सचे 0
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
    सचे (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        वर्ण* buf = (वर्ण*) uprv_malloc(buflen);
        वर्ण* canon = (buf == 0) ? 0 : (वर्ण*) uprv_malloc(buflen);
        सचे (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            सचे (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    पटिनिवत्त result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ध्रुव UnicodeString& id, Locale& result)
{
    सचे (id.isBogus()) {
        result.setToBogus();
    } अञ्ञथा {
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
        करोहि {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            सचे(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                भङ्ग; // done
            } अञ्ञथा {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } याव (U_SUCCESS(status));
        सचे (U_FAILURE(status)) {
            result.setToBogus();
        } अञ्ञथा {
            result = Locale::createFromName(buffer.data());
        }
    }
    पटिनिवत्त result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ध्रुव Locale& locale, UnicodeString& result)
{
    सचे (locale.isBogus()) {
        result.setToBogus();
    } अञ्ञथा {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    पटिनिवत्त result;
}

ध्रुव Hashtable*
LocaleUtility::getAvailableLocaleNames(ध्रुव UnicodeString& bundleID)
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
    सचे (cache == nullptr) {
        // Catastrophic failure.
        पटिनिवत्त nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    सचे (htp == nullptr) {
        htp = नूतन Hashtable(status);
        सचे (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ध्रुव वर्ण* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            अत्थाय (;;) {
                ध्रुव char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                सचे (id == nullptr) {
                    भङ्ग;
                }
                htp->put(UnicodeString(id), (शून्य*)htp, status);
            }
            सचे (U_FAILURE(status)) {
                delete htp;
                पटिनिवत्त nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            सचे (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } अञ्ञथा {
                cache->put(bundleID, (शून्य*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    पटिनिवत्त htp;
}

तार्किक
LocaleUtility::isFallbackOf(ध्रुव UnicodeString& मूलम्, ध्रुव UnicodeString& child)
{
    पटिनिवत्त child.indexOf(मूलम्) == 0 &&
      (child.length() == मूलम्.length() ||
       child.charAt(मूलम्.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
