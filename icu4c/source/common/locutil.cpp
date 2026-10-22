// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "unicode/uenum.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "locutil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
স্থির icu::UInitOnce   LocaleUtilityInitOnce {};
স্থির icu::Hashtable * LocaleUtility_cache = nullptr;

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
স্থির UBool U_CALLCONV service_cleanup() {
    যদি (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    প্রত্যাগম সত্য;
}


স্থির শূন্য U_CALLCONV locale_utility_init(UErrorCode &status) {
    using নামাকাশ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = নূতন Hashtable(status);
    যদি (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        প্রত্যাগম;
    }
    যদি (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    LocaleUtility_cache->set_মান_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ধ্রুব UnicodeString* id, UnicodeString& result)
{
  যদি (id == nullptr) {
    result.setToBogus();
  } অন্যথা {
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
    যদি (n >= 0 && n < end) {
        end = n;
    }
    যদি (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    যদি (n < 0) {
      n = end;
    }
    প্রতি (; i < n; ++i) {
      char16_t c = result.charAt(i);
      যদি (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    প্রতি (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      যদি (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  প্রত্যাগম result;

#যদি 0
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
    যদি (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        বর্ণ* buf = (বর্ণ*) uprv_malloc(buflen);
        বর্ণ* canon = (buf == 0) ? 0 : (বর্ণ*) uprv_malloc(buflen);
        যদি (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            যদি (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    প্রত্যাগম result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(ধ্রুব UnicodeString& id, Locale& result)
{
    যদি (id.isBogus()) {
        result.setToBogus();
    } অন্যথা {
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
        কুরু {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            যদি(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                ভঙ্গ; // done
            } অন্যথা {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } যাবত্ (U_SUCCESS(status));
        যদি (U_FAILURE(status)) {
            result.setToBogus();
        } অন্যথা {
            result = Locale::createFromName(buffer.data());
        }
    }
    প্রত্যাগম result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ধ্রুব Locale& locale, UnicodeString& result)
{
    যদি (locale.isBogus()) {
        result.setToBogus();
    } অন্যথা {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    প্রত্যাগম result;
}

ধ্রুব Hashtable*
LocaleUtility::getAvailableLocaleNames(ধ্রুব UnicodeString& bundleID)
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
    যদি (cache == nullptr) {
        // Catastrophic failure.
        প্রত্যাগম nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    যদি (htp == nullptr) {
        htp = নূতন Hashtable(status);
        যদি (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            ধ্রুব বর্ণ* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            প্রতি (;;) {
                ধ্রুব char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                যদি (id == nullptr) {
                    ভঙ্গ;
                }
                htp->put(UnicodeString(id), (শূন্য*)htp, status);
            }
            যদি (U_FAILURE(status)) {
                delete htp;
                প্রত্যাগম nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            যদি (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } অন্যথা {
                cache->put(bundleID, (শূন্য*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    প্রত্যাগম htp;
}

তার্কিক
LocaleUtility::isFallbackOf(ধ্রুব UnicodeString& মূল, ধ্রুব UnicodeString& child)
{
    প্রত্যাগম child.indexOf(মূল) == 0 &&
      (child.length() == মূল.length() ||
       child.charAt(মূল.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
