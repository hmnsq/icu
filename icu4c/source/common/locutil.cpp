// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

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
    यदि (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    प्रत्यागम सत्य;
}


स्थिर शून्य U_CALLCONV locale_utility_init(UErrorCode &status) {
    using नामाकाश icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = नूतन Hashtable(status);
    यदि (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        प्रत्यागम;
    }
    यदि (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    LocaleUtility_cache->set_मूल्य_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ध्रुव UnicodeString* id, UnicodeString& result)
{
  यदि (id == nullptr) {
    result.setToBogus();
  } अन्यथा {
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
    यदि (n >= 0 && n < end) {
        end = n;
    }
    यदि (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    यदि (n < 0) {
      n = end;
    }
    प्रति (; i < n; ++i) {
      char16_t c = result.charAt(i);
      यदि (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    प्रति (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      यदि (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  प्रत्यागम result;

#यदि 0
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
    यदि (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        वर्ण* buf = (वर्ण*) uprv_malloc(buflen);
        वर्ण* canon = (buf == 0) ? 0 : (वर्ण*) uprv_malloc(buflen);
        यदि (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            यदि (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    प्रत्यागम result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ध्रुव UnicodeString& id, Locale& result)
{
    यदि (id.isBogus()) {
        result.setToBogus();
    } अन्यथा {
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
        कुरु {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            यदि(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                भङ्ग; // done
            } अन्यथा {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } यावत् (U_SUCCESS(status));
        यदि (U_FAILURE(status)) {
            result.setToBogus();
        } अन्यथा {
            result = Locale::createFromName(buffer.data());
        }
    }
    प्रत्यागम result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ध्रुव Locale& locale, UnicodeString& result)
{
    यदि (locale.isBogus()) {
        result.setToBogus();
    } अन्यथा {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    प्रत्यागम result;
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
    यदि (cache == nullptr) {
        // Catastrophic failure.
        प्रत्यागम nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    यदि (htp == nullptr) {
        htp = नूतन Hashtable(status);
        यदि (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ध्रुव वर्ण* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            प्रति (;;) {
                ध्रुव char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                यदि (id == nullptr) {
                    भङ्ग;
                }
                htp->put(UnicodeString(id), (शून्य*)htp, status);
            }
            यदि (U_FAILURE(status)) {
                delete htp;
                प्रत्यागम nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            यदि (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } अन्यथा {
                cache->put(bundleID, (शून्य*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    प्रत्यागम htp;
}

तार्किक
LocaleUtility::isFallbackOf(ध्रुव UnicodeString& मूल, ध्रुव UnicodeString& child)
{
    प्रत्यागम child.indexOf(मूल) == 0 &&
      (child.length() == मूल.length() ||
       child.charAt(मूल.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
