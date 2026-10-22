// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "unicode/uenum.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "locutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
સ્થિર icu::UInitOnce   LocaleUtilityInitOnce {};
સ્થિર icu::Hashtable * LocaleUtility_cache = nullptr;

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
સ્થિર UBool U_CALLCONV service_cleanup() {
    જો (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    પાછા સાચું;
}


સ્થિર શૂન્ય U_CALLCONV locale_utility_init(UErrorCode &status) {
    using નામાકાશ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = નવું Hashtable(status);
    જો (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        પાછા;
    }
    જો (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    LocaleUtility_cache->set_કિંમત_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(અચલ UnicodeString* id, UnicodeString& result)
{
  જો (id == nullptr) {
    result.setToBogus();
  } નહીંતર {
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
    જો (n >= 0 && n < end) {
        end = n;
    }
    જો (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    જો (n < 0) {
      n = end;
    }
    માટે (; i < n; ++i) {
      char16_t c = result.charAt(i);
      જો (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    માટે (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      જો (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  પાછા result;

#જો 0
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
    જો (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        અક્ષર* buf = (અક્ષર*) uprv_malloc(buflen);
        અક્ષર* canon = (buf == 0) ? 0 : (અક્ષર*) uprv_malloc(buflen);
        જો (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            જો (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    પાછા result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(અચલ UnicodeString& id, Locale& result)
{
    જો (id.isBogus()) {
        result.setToBogus();
    } નહીંતર {
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
        કરો {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            જો(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                તોડો; // done
            } નહીંતર {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } જ્યારે (U_SUCCESS(status));
        જો (U_FAILURE(status)) {
            result.setToBogus();
        } નહીંતર {
            result = Locale::createFromName(buffer.data());
        }
    }
    પાછા result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(અચલ Locale& locale, UnicodeString& result)
{
    જો (locale.isBogus()) {
        result.setToBogus();
    } નહીંતર {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    પાછા result;
}

અચલ Hashtable*
LocaleUtility::getAvailableLocaleNames(અચલ UnicodeString& bundleID)
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
    જો (cache == nullptr) {
        // Catastrophic failure.
        પાછા nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    જો (htp == nullptr) {
        htp = નવું Hashtable(status);
        જો (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            અચલ અક્ષર* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            માટે (;;) {
                અચલ char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                જો (id == nullptr) {
                    તોડો;
                }
                htp->put(UnicodeString(id), (શૂન્ય*)htp, status);
            }
            જો (U_FAILURE(status)) {
                delete htp;
                પાછા nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            જો (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } નહીંતર {
                cache->put(bundleID, (શૂન્ય*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    પાછા htp;
}

તાર્કિક
LocaleUtility::isFallbackOf(અચલ UnicodeString& મૂળ, અચલ UnicodeString& child)
{
    પાછા child.indexOf(મૂળ) == 0 &&
      (child.length() == મૂળ.length() ||
       child.charAt(મૂળ.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
