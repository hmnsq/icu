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
स्थिर icu::Hashtable * LocaleUtility_cache = शून्यसूचकः;

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
स्थिर UBool U_CALLCONV service_cleanup() {
    यदि (LocaleUtility_cache) {
        विलोपय LocaleUtility_cache;
        LocaleUtility_cache = शून्यसूचकः;
    }
    प्रत्यागम सत्य;
}


स्थिर शून्य U_CALLCONV locale_utility_init(UError_संहिता &स्थितिः) {
    उपयुज्य नामाकाश icu;
    U_ASSERT(LocaleUtility_cache == शून्यसूचकः);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = नूतन Hashtable(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय LocaleUtility_cache;
        LocaleUtility_cache = शून्यसूचकः;
        प्रत्यागम;
    }
    यदि (LocaleUtility_cache == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    LocaleUtility_cache->समुच्चयः_मानम्_Deleter(uhash_deleteHashtable);
}

U_CDECL_अन्तः

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(ध्रुव UnicodeString* id, UnicodeString& परिणाम)
{
  यदि (id == शून्यसूचकः) {
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
  } अन्यथा {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    परिणाम = *id;
    int32_t i = 0;
    int32_t अन्तः = परिणाम.सूचकांक_सम्बन्धि(AT_SIGN_CHAR);
    int32_t n = परिणाम.सूचकांक_सम्बन्धि(PERIOD_CHAR);
    यदि (n >= 0 && n < अन्तः) {
        अन्तः = n;
    }
    यदि (अन्तः < 0) {
        अन्तः = परिणाम.length();
    }
    n = परिणाम.सूचकांक_सम्बन्धि(UNDERSCORE_CHAR);
    यदि (n < 0) {
      n = अन्तः;
    }
    प्रति (; i < n; ++i) {
      char16_t c = परिणाम.charAt(i);
      यदि (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        परिणाम.समुच्चयः_Char_At(i, c);
      }
    }
    प्रति (n = अन्तः; i < n; ++i) {
      char16_t c = परिणाम.charAt(i);
      यदि (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        परिणाम.समुच्चयः_Char_At(i, c);
      }
    }
  }
  प्रत्यागम परिणाम;

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
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    यदि (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        वर्ण* buf = (वर्ण*) uprv_malloc(buflen);
        वर्ण* canon = (buf == 0) ? 0 : (वर्ण*) uprv_malloc(buflen);
        यदि (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_संहिता ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            यदि (U_SUCCESS(ec)) {
                परिणाम = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    प्रत्यागम परिणाम;
#यदि_अन्तः
}

Locale&
LocaleUtility::initLocaleFromName(ध्रुव UnicodeString& id, Locale& परिणाम)
{
    यदि (id.isBogus()) {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
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
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        कुरु {
            i = id.सूचकांक_सम्बन्धि(static_cast<char16_t>(0x40), पूर्व);
            यदि(i < 0) {
                // no @ between prev and the rest of the string
                बफर.appendInvariantChars(id.अस्थायी_Sub_String(पूर्व), स्थितिः);
                भङ्ग; //समाप्तम्
            } अन्यथा {
                // normal invariant-character conversion for text between @s
                बफर.appendInvariantChars(id.अस्थायी_Sub_String(पूर्व, i - पूर्व), स्थितिः);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                बफर.append('@', स्थितिः);
                पूर्व = i + 1;
            }
        } यावत् (U_SUCCESS(स्थितिः));
        यदि (U_FAILURE(स्थितिः)) {
            परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
        } अन्यथा {
            परिणाम = Locale::createFromName(बफर.दत्त());
        }
    }
    प्रत्यागम परिणाम;
}

UnicodeString&
LocaleUtility::initNameFromLocale(ध्रुव Locale& locale, UnicodeString& परिणाम)
{
    यदि (locale.isBogus()) {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    } अन्यथा {
        परिणाम.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    प्रत्यागम परिणाम;
}

ध्रुव Hashtable*
LocaleUtility::get_उपलब्धम्_Locale_नामानि(ध्रुव UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_संहिता स्थितिः = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, स्थितिः);
    Hashtable *cache = LocaleUtility_cache;
    यदि (cache == शून्यसूचकः) {
        // Catastrophic failure.
        प्रत्यागम शून्यसूचकः;
    }

    Hashtable* htp;
    umtx_तालकम्(शून्यसूचकः);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(शून्यसूचकः);

    यदि (htp == शून्यसूचकः) {
        htp = नूतन Hashtable(स्थितिः);
        यदि (htp && U_SUCCESS(स्थितिः)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, स्थितिः);
            ध्रुव वर्ण* मार्गः = cbundleID.is_रिक्तानि() ? शून्यसूचकः : cbundleID.दत्त();
            icu::LocalUEnumerationPointer uenum(ures_open_उपलब्धम्_Locales(मार्गः, &स्थितिः));
            प्रति (;;) {
                ध्रुव char16_t* id = uenum_unext(uenum.getAlias(), शून्यसूचकः, &स्थितिः);
                यदि (id == शून्यसूचकः) {
                    भङ्ग;
                }
                htp->put(UnicodeString(id), (शून्य*)htp, स्थितिः);
            }
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय htp;
                प्रत्यागम शून्यसूचकः;
            }
            umtx_तालकम्(शून्यसूचकः);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            यदि (t != शून्यसूचकः) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(शून्यसूचकः);
                विलोपय htp;
                htp = t;
            } अन्यथा {
                cache->put(bundleID, (शून्य*)htp, स्थितिः);
                umtx_unlock(शून्यसूचकः);
            }
        }
    }
    प्रत्यागम htp;
}

तार्किक
LocaleUtility::is_Fallback_सम्बन्धि(ध्रुव UnicodeString& मूलम्, ध्रुव UnicodeString& child)
{
    प्रत्यागम child.सूचकांक_सम्बन्धि(मूलम्) == 0 &&
      (child.length() == मूलम्.length() ||
       child.charAt(मूलम्.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः
