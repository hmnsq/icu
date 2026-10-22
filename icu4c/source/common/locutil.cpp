// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#include "unicode/utypes.h"

#ᱡᱩᱫᱤ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#include "unicode/resbund.h"
#include "unicode/uenum.h"
#include "cmemory.h"
#include "ustrfmt.h"
#include "locutil.h"
#include "charstr.h"
#include "ucln_cmn.h"
#include "uassert.h"
#include "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
static icu::UInitOnce   LocaleUtilityInitOnce {};
static icu::Hashtable * LocaleUtility_cache = nullptr;

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
static UBool U_CALLCONV service_cleanup() {
    ᱡᱩᱫᱤ (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    ᱨᱩᱣᱟᱹᱲ true;
}


static void U_CALLCONV locale_utility_init(UErrorCode &status) {
    using namespace icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = new Hashtable(status);
    ᱡᱩᱫᱤ (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        ᱨᱩᱣᱟᱹᱲ;
    }
    ᱡᱩᱫᱤ (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ᱨᱩᱣᱟᱹᱲ;
    }
    LocaleUtility_cache->setValueDeleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(const UnicodeString* id, UnicodeString& result)
{
  ᱡᱩᱫᱤ (id == nullptr) {
    result.setToBogus();
  } ᱵᱟᱝ {
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
    ᱡᱩᱫᱤ (n >= 0 && n < end) {
        end = n;
    }
    ᱡᱩᱫᱤ (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    ᱡᱩᱫᱤ (n < 0) {
      n = end;
    }
    for (; i < n; ++i) {
      char16_t c = result.charAt(i);
      ᱡᱩᱫᱤ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    for (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      ᱡᱩᱫᱤ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  ᱨᱩᱣᱟᱹᱲ result;

#ᱡᱩᱫᱤ 0
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
    ᱡᱩᱫᱤ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        var_na* buf = (var_na*) uprv_malloc(buflen);
        var_na* canon = (buf == 0) ? 0 : (var_na*) uprv_malloc(buflen);
        ᱡᱩᱫᱤ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ᱡᱩᱫᱤ (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    ᱨᱩᱣᱟᱹᱲ result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(const UnicodeString& id, Locale& result)
{
    ᱡᱩᱫᱤ (id.isBogus()) {
        result.setToBogus();
    } ᱵᱟᱝ {
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
        do {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            ᱡᱩᱫᱤ(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                bha_nga; // done
            } ᱵᱟᱝ {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } ᱡᱚᱠᱷᱚᱱ (U_SUCCESS(status));
        ᱡᱩᱫᱤ (U_FAILURE(status)) {
            result.setToBogus();
        } ᱵᱟᱝ {
            result = Locale::createFromName(buffer.data());
        }
    }
    ᱨᱩᱣᱟᱹᱲ result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(const Locale& locale, UnicodeString& result)
{
    ᱡᱩᱫᱤ (locale.isBogus()) {
        result.setToBogus();
    } ᱵᱟᱝ {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    ᱨᱩᱣᱟᱹᱲ result;
}

const Hashtable*
LocaleUtility::getAvailableLocaleNames(const UnicodeString& bundleID)
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
    ᱡᱩᱫᱤ (cache == nullptr) {
        // Catastrophic failure.
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    ᱡᱩᱫᱤ (htp == nullptr) {
        htp = new Hashtable(status);
        ᱡᱩᱫᱤ (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            const var_na* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            for (;;) {
                const char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                ᱡᱩᱫᱤ (id == nullptr) {
                    bha_nga;
                }
                htp->put(UnicodeString(id), (void*)htp, status);
            }
            ᱡᱩᱫᱤ (U_FAILURE(status)) {
                delete htp;
                ᱨᱩᱣᱟᱹᱲ nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ᱡᱩᱫᱤ (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } ᱵᱟᱝ {
                cache->put(bundleID, (void*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    ᱨᱩᱣᱟᱹᱲ htp;
}

bool
LocaleUtility::isFallbackOf(const UnicodeString& root, const UnicodeString& child)
{
    ᱨᱩᱣᱟᱹᱲ child.indexOf(root) == 0 &&
      (child.length() == root.length() ||
       child.charAt(root.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
