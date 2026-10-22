// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "unicode/uenum.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "locutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
സ്ഥിതം icu::UInitOnce   LocaleUtilityInitOnce {};
സ്ഥിതം icu::Hashtable * LocaleUtility_cache = nullptr;

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
സ്ഥിതം UBool U_CALLCONV service_cleanup() {
    എങ്കിൽ (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    തിരികെ സത്യം;
}


സ്ഥിതം ശൂന്യം U_CALLCONV locale_utility_init(UErrorCode &status) {
    using നാമാകാശ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = പുതിയ Hashtable(status);
    എങ്കിൽ (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        തിരികെ;
    }
    എങ്കിൽ (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    LocaleUtility_cache->set_മൂല്യം_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(സ്ഥിരം UnicodeString* id, UnicodeString& result)
{
  എങ്കിൽ (id == nullptr) {
    result.setToBogus();
  } അല്ലെങ്കിൽ {
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
    എങ്കിൽ (n >= 0 && n < end) {
        end = n;
    }
    എങ്കിൽ (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    എങ്കിൽ (n < 0) {
      n = end;
    }
    വേണ്ടി (; i < n; ++i) {
      char16_t c = result.charAt(i);
      എങ്കിൽ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    വേണ്ടി (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      എങ്കിൽ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  തിരികെ result;

#എങ്കിൽ 0
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
    എങ്കിൽ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        അക്ഷരം* buf = (അക്ഷരം*) uprv_malloc(buflen);
        അക്ഷരം* canon = (buf == 0) ? 0 : (അക്ഷരം*) uprv_malloc(buflen);
        എങ്കിൽ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            എങ്കിൽ (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    തിരികെ result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(സ്ഥിരം UnicodeString& id, Locale& result)
{
    എങ്കിൽ (id.isBogus()) {
        result.setToBogus();
    } അല്ലെങ്കിൽ {
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
        ചെയ്യുക {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            എങ്കിൽ(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                നിർത്തുക; // done
            } അല്ലെങ്കിൽ {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } ആകുമ്പോൾ (U_SUCCESS(status));
        എങ്കിൽ (U_FAILURE(status)) {
            result.setToBogus();
        } അല്ലെങ്കിൽ {
            result = Locale::createFromName(buffer.data());
        }
    }
    തിരികെ result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(സ്ഥിരം Locale& locale, UnicodeString& result)
{
    എങ്കിൽ (locale.isBogus()) {
        result.setToBogus();
    } അല്ലെങ്കിൽ {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    തിരികെ result;
}

സ്ഥിരം Hashtable*
LocaleUtility::getAvailableLocaleNames(സ്ഥിരം UnicodeString& bundleID)
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
    എങ്കിൽ (cache == nullptr) {
        // Catastrophic failure.
        തിരികെ nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    എങ്കിൽ (htp == nullptr) {
        htp = പുതിയ Hashtable(status);
        എങ്കിൽ (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            സ്ഥിരം അക്ഷരം* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            വേണ്ടി (;;) {
                സ്ഥിരം char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                എങ്കിൽ (id == nullptr) {
                    നിർത്തുക;
                }
                htp->put(UnicodeString(id), (ശൂന്യം*)htp, status);
            }
            എങ്കിൽ (U_FAILURE(status)) {
                delete htp;
                തിരികെ nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            എങ്കിൽ (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } അല്ലെങ്കിൽ {
                cache->put(bundleID, (ശൂന്യം*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    തിരികെ htp;
}

തർക്കം
LocaleUtility::isFallbackOf(സ്ഥിരം UnicodeString& വേര്, സ്ഥിരം UnicodeString& child)
{
    തിരികെ child.indexOf(വേര്) == 0 &&
      (child.length() == വേര്.length() ||
       child.charAt(വേര്.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
