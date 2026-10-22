// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#समावेश "unicode/resbund.h"
#समावेश "unicode/uenum.h"
#समावेश "cmemory.h"
#समावेश "ustrfmt.h"
#समावेश "locutil.h"
#समावेश "charstr.h"
#समावेश "ucln_cmn.h"
#समावेश "uassert.h"
#समावेश "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
अटल icu::UInitOnce   LocaleUtilityInitOnce {};
अटल icu::Hashtable * LocaleUtility_cache = nullptr;

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
अटल UBool U_CALLCONV service_cleanup() {
    अगर (LocaleUtility_cache) {
        मिटाओ LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
    }
    वापस true;
}


अटल व्योम U_CALLCONV locale_utility_init(UErrorCode &status) {
    using namespace icu;
    U_ASSERT(LocaleUtility_cache == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = नया Hashtable(status);
    अगर (U_FAILURE(status)) {
        मिटाओ LocaleUtility_cache;
        LocaleUtility_cache = nullptr;
        वापस;
    }
    अगर (LocaleUtility_cache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    LocaleUtility_cache->set_मूल्य_Deleter(uhash_deleteHashtable);
}

U_CDECL_END

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(स्थिर UnicodeString* id, UnicodeString& परिणाम)
{
  अगर (id == nullptr) {
    परिणाम.setToBogus();
  } अन्यथा {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    परिणाम = *id;
    int32_t i = 0;
    int32_t end = परिणाम.सूचकांक_Of(AT_SIGN_CHAR);
    int32_t n = परिणाम.सूचकांक_Of(PERIOD_CHAR);
    अगर (n >= 0 && n < end) {
        end = n;
    }
    अगर (end < 0) {
        end = परिणाम.length();
    }
    n = परिणाम.सूचकांक_Of(UNDERSCORE_CHAR);
    अगर (n < 0) {
      n = end;
    }
    क्रम (; i < n; ++i) {
      char16_t c = परिणाम.charAt(i);
      अगर (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        परिणाम.setCharAt(i, c);
      }
    }
    क्रम (n = end; i < n; ++i) {
      char16_t c = परिणाम.charAt(i);
      अगर (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        परिणाम.setCharAt(i, c);
      }
    }
  }
  वापस परिणाम;

#अगर 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    परिणाम.setToBogus();
    अगर (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        अक्षर* buf = (अक्षर*) uprv_malloc(buflen);
        अक्षर* canon = (buf == 0) ? 0 : (अक्षर*) uprv_malloc(buflen);
        अगर (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UErrorCode ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            अगर (U_SUCCESS(ec)) {
                परिणाम = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    वापस परिणाम;
#पूर्ण_अगर
}

Locale&
LocaleUtility::initLocaleFromName(स्थिर UnicodeString& id, Locale& परिणाम)
{
    अगर (id.isBogus()) {
        परिणाम.setToBogus();
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
        CharString बफर;
        int32_t पूर्व, i;
        पूर्व = 0;
        UErrorCode status = U_ZERO_ERROR;
        करो {
            i = id.सूचकांक_Of(static_cast<char16_t>(0x40), पूर्व);
            अगर(i < 0) {
                // no @ between prev and the rest of the string
                बफर.appendInvariantChars(id.अस्थायी_Sub_String(पूर्व), status);
                अवरोध; // done
            } अन्यथा {
                // normal invariant-character conversion for text between @s
                बफर.appendInvariantChars(id.अस्थायी_Sub_String(पूर्व, i - पूर्व), status);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                बफर.append('@', status);
                पूर्व = i + 1;
            }
        } जबतक (U_SUCCESS(status));
        अगर (U_FAILURE(status)) {
            परिणाम.setToBogus();
        } अन्यथा {
            परिणाम = Locale::createFromName(बफर.तथ्य());
        }
    }
    वापस परिणाम;
}

UnicodeString&
LocaleUtility::initNameFromLocale(स्थिर Locale& locale, UnicodeString& परिणाम)
{
    अगर (locale.isBogus()) {
        परिणाम.setToBogus();
    } अन्यथा {
        परिणाम.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    वापस परिणाम;
}

स्थिर Hashtable*
LocaleUtility::getAvailableLocaleNames(स्थिर UnicodeString& bundleID)
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
    अगर (cache == nullptr) {
        // Catastrophic failure.
        वापस nullptr;
    }

    Hashtable* htp;
    umtx_lock(nullptr);
    htp = static_cast<Hashtable*>(cache->लो(bundleID));
    umtx_unlock(nullptr);

    अगर (htp == nullptr) {
        htp = नया Hashtable(status);
        अगर (htp && U_SUCCESS(status)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, status);
            स्थिर अक्षर* path = cbundleID.isEmpty() ? nullptr : cbundleID.तथ्य();
            icu::LocalUEnumerationPointer uenum(ures_openAvailableLocales(path, &status));
            क्रम (;;) {
                स्थिर char16_t* id = uenum_unext(uenum.getAlias(), nullptr, &status);
                अगर (id == nullptr) {
                    अवरोध;
                }
                htp->दो(UnicodeString(id), (व्योम*)htp, status);
            }
            अगर (U_FAILURE(status)) {
                मिटाओ htp;
                वापस nullptr;
            }
            umtx_lock(nullptr);
            Hashtable *t = static_cast<Hashtable *>(cache->लो(bundleID));
            अगर (t != nullptr) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(nullptr);
                मिटाओ htp;
                htp = t;
            } अन्यथा {
                cache->दो(bundleID, (व्योम*)htp, status);
                umtx_unlock(nullptr);
            }
        }
    }
    वापस htp;
}

bool
LocaleUtility::isFallbackOf(स्थिर UnicodeString& मूल, स्थिर UnicodeString& child)
{
    वापस child.सूचकांक_Of(मूल) == 0 &&
      (child.length() == मूल.length() ||
       child.charAt(मूल.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर
