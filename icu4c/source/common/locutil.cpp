// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "unicode/uenum.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "locutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
நிலையான icu::UInitOnce   LocaleUtilityInitOnce {};
நிலையான icu::Hashtable * LocaleUtility_cache = nullptr;

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
நிலையான UBool U_CALLCONV service_cleanup() {
    எனில் (LocaleUtility_cache) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    திரும்பு உண்மை;
}


நிலையான வெறுமை U_CALLCONV locale_utility_init(UErrorCode &status) {
    using நாமாகாஶ icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = புதிய Hashtable(status);
    எனில் (U_FAILURE(status)) {
        delete LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        திரும்பு;
    }
    எனில் (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    LocaleUtility_cache->set_மதிப்பு_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(மாறிலி UnicodeString* id, UnicodeString& result)
{
  எனில் (id == nullptr) {
    result.setToBogus();
  } இல்லையெனில் {
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
    எனில் (n >= 0 && n < end) {
        end = n;
    }
    எனில் (end < 0) {
        end = result.length();
    }
    n = result.indexOf(UNDERSCORE_CHAR);
    எனில் (n < 0) {
      n = end;
    }
    க்கு (; i < n; ++i) {
      char16_t c = result.charAt(i);
      எனில் (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        result.setCharAt(i, c);
      }
    }
    க்கு (n = end; i < n; ++i) {
      char16_t c = result.charAt(i);
      எனில் (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        result.setCharAt(i, c);
      }
    }
  }
  திரும்பு result;

#எனில் 0
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
    எனில் (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        எழுத்து* buf = (எழுத்து*) uprv_malloc(buflen);
        எழுத்து* canon = (buf == 0) ? 0 : (எழுத்து*) uprv_malloc(buflen);
        எனில் (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            எனில் (U_SUCCESS(ec)) {
                result = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    திரும்பு result;
#endif
}

Locale&
LocaleUtility::initLocaleFromName(மாறிலி UnicodeString& id, Locale& result)
{
    எனில் (id.isBogus()) {
        result.setToBogus();
    } இல்லையெனில் {
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
        செய் {
            i = id.indexOf(static_cast<char16_t>(0x40), prev);
            எனில்(i < 0) {
                // no @ between prev and the rest of the string
                buffer.appendInvariantChars(id.tempSubString(prev), status);
                நிறுத்து; // done
            } இல்லையெனில் {
                // normal invariant-character conversion for text between @s
                buffer.appendInvariantChars(id.tempSubString(prev, i - prev), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                buffer.append('@', status);
                prev = i + 1;
            }
        } போது (U_SUCCESS(status));
        எனில் (U_FAILURE(status)) {
            result.setToBogus();
        } இல்லையெனில் {
            result = Locale::createFromName(buffer.data());
        }
    }
    திரும்பு result;
}

UnicodeString&
LocaleUtility::initNameFromLocale(மாறிலி Locale& locale, UnicodeString& result)
{
    எனில் (locale.isBogus()) {
        result.setToBogus();
    } இல்லையெனில் {
        result.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    திரும்பு result;
}

மாறிலி Hashtable*
LocaleUtility::getAvailableLocaleNames(மாறிலி UnicodeString& bundleID)
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
    எனில் (cache == nullptr) {
        // Catastrophic failure.
        திரும்பு nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(nullptr);

    எனில் (htp == nullptr) {
        htp = புதிய Hashtable(status);
        எனில் (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            மாறிலி எழுத்து* path = cbundleID.isEmpty() ? nullptr : cbundleID.data();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            க்கு (;;) {
                மாறிலி char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                எனில் (id == nullptr) {
                    நிறுத்து;
                }
                htp->put(UnicodeString(id), (வெறுமை*)htp, status);
            }
            எனில் (U_FAILURE(status)) {
                delete htp;
                திரும்பு nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            எனில் (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                delete htp;
                htp = t;
            } இல்லையெனில் {
                cache->put(bundleID, (வெறுமை*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    திரும்பு htp;
}

தருக்க
LocaleUtility::isFallbackOf(மாறிலி UnicodeString& வேர், மாறிலி UnicodeString& child)
{
    திரும்பு child.indexOf(வேர்) == 0 &&
      (child.length() == வேர்.length() ||
       child.charAt(வேர்.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif
