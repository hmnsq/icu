// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#include "unicode/utypes.h"

#如果 !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

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
    如果 (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    返回 true;
}


static void U_CALLCONV locale_utility_init(UErrorCode &status) {
    using namespace icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = new Hashtable(status);
    如果 (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        返回;
    }
    如果 (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        返回;
    }
    LocaleUtility_cache->setValueDeleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(const UnicodeString* id, UnicodeString& result)
{
  如果 (id == nullptr) {
    result.setToBogus();
  } 否则 {
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
    如果 (n >= 0 && n < end) {
        end = n;
    }
    如果 (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    如果 (n < 0) {
      n = end;
    }
    为 (; i < n; ++i) {
      char16_t c = result.charAt(i);
      如果 (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    为 (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      如果 (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  返回 result;

#如果 0
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
    如果 (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        char* buf = (char*) uprv_malloc(buflen);
        char* canon = (buf == 0) ? 0 : (char*) uprv_malloc(buflen);
        如果 (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            如果 (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    返回 result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(const UnicodeString& id, Locale& result)
{
    如果 (id.isBogus()) {
        result.setToBogus();
    } 否则 {
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
            如果(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                break; // done
            } 否则 {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } 当 (U_SUCCESS(status));
        如果 (U_FAILURE(status)) {
            result.setToBogus();
        } 否则 {
            result = Locale::createFromName(buffer.data());
        }
    }
    返回 result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(const Locale& locale, UnicodeString& result)
{
    如果 (locale.isBogus()) {
        result.setToBogus();
    } 否则 {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    返回 result;
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
    如果 (cache == nullptr) {
        // Catastrophic failure.
        返回 nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    如果 (htp == nullptr) {
        htp = new Hashtable(status);
        如果 (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            const char* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            为 (;;) {
                const char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                如果 (id == nullptr) {
                    break;
                }
                htp->put(UnicodeString(id), (void*)htp, status);
            }
            如果 (U_FAILURE(status)) {
                delete htp;
                返回 nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            如果 (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } 否则 {
                cache->put(bundleID, (void*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    返回 htp;
}

bool
LocaleUtility::isFallbackOf(const UnicodeString& root, const UnicodeString& child)
{
    返回 child.indexOf(root) == 0 &&
      (child.length() == root.length() ||
       child.charAt(root.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
