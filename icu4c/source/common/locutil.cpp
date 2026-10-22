// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 2002-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */
#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION

#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "unicode/uenum.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustrfmt.h"
#అన్తర్భావ "locutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"

// see LocaleUtility::getAvailableLocaleNames
స్థిర icu::UInitOnce   LocaleUtilityInitOnce {};
స్థిర icu::Hashtable * LocaleUtility_cache = శూన్యసూచిక;

#నిర్వచించు UNDERSCORE_CHAR ((char16_t)0x005f)
#నిర్వచించు AT_SIGN_CHAR    ((char16_t)64)
#నిర్వచించు PERIOD_CHAR     ((char16_t)46)

/*
 ******************************************************************
 */

/**
 * Release all static memory held by Locale Utility.  
 */
U_CDECL_BEGIN
స్థిర UBool U_CALLCONV service_cleanup() {
    ఒకవేళ (LocaleUtility_cache) {
        తొలగించు LocaleUtility_cache;
        LocaleUtility_cache = శూన్యసూచిక;
    }
    తిరిగి నిజం;
}


స్థిర శూన్య U_CALLCONV locale_utility_init(UError_సంకేతం &స్థితి_2) {
    ఉపయోగించి నామాకాశ icu;
    U_ASSERT(LocaleUtility_cache == శూన్యసూచిక);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_SERVICE, service_cleanup);
    LocaleUtility_cache = కొత్త Hashtable(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు LocaleUtility_cache;
        LocaleUtility_cache = శూన్యసూచిక;
        తిరిగి;
    }
    ఒకవేళ (LocaleUtility_cache == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    LocaleUtility_cache->సమితి_విలువ_Deleter(uhash_deleteHashtable);
}

U_CDECL_చివర

U_NAMESPACE_BEGIN

UnicodeString&
LocaleUtility::canonicalLocaleString(స్థిరరాశి UnicodeString* id, UnicodeString& ఫలితం)
{
  ఒకవేళ (id == శూన్యసూచిక) {
    ఫలితం.సమితి_గమ్యం_Bogus();
  } లేకపోతే {
    // Fix case only (no other changes) up to the first '@' or '.' or
    // end of string, whichever comes first.  In 3.0 I changed this to
    // stop at first '@' or '.'.  It used to run out to the end of
    // string.  My fix makes the tests pass but is probably
    // structurally incorrect.  See below.  [alan 3.0]

    // TODO: Doug, you might want to revise this...
    ఫలితం = *id;
    int32_t i = 0;
    int32_t చివర = ఫలితం.సూచీ_యొక్క(AT_SIGN_CHAR);
    int32_t n = ఫలితం.సూచీ_యొక్క(PERIOD_CHAR);
    ఒకవేళ (n >= 0 && n < చివర) {
        చివర = n;
    }
    ఒకవేళ (చివర < 0) {
        చివర = ఫలితం.length();
    }
    n = ఫలితం.సూచీ_యొక్క(UNDERSCORE_CHAR);
    ఒకవేళ (n < 0) {
      n = చివర;
    }
    కొరకు (; i < n; ++i) {
      char16_t c = ఫలితం.charAt(i);
      ఒకవేళ (c >= 0x0041 && c <= 0x005a) {
        c += 0x20;
        ఫలితం.సమితి_Char_At(i, c);
      }
    }
    కొరకు (n = చివర; i < n; ++i) {
      char16_t c = ఫలితం.charAt(i);
      ఒకవేళ (c >= 0x0061 && c <= 0x007a) {
        c -= 0x20;
        ఫలితం.సమితి_Char_At(i, c);
      }
    }
  }
  తిరిగి ఫలితం;

#ఒకవేళ 0
    // This code does a proper full level 2 canonicalization of id.
    // It's nasty to go from char16_t to char to char to char16_t -- but
    // that's what you have to do to use the uloc_canonicalize
    // function on UnicodeStrings.

    // I ended up doing the alternate fix (see above) not for
    // performance reasons, although performance will certainly be
    // better, but because doing a full level 2 canonicalization
    // causes some tests to fail.  [alan 3.0]

    // TODO: Doug, you might want to revisit this...
    ఫలితం.సమితి_గమ్యం_Bogus();
    ఒకవేళ (id != 0) {
        int32_t buflen = id->length() + 8; // space for NUL
        అక్షరం* buf = (అక్షరం*) uprv_malloc(buflen);
        అక్షరం* canon = (buf == 0) ? 0 : (అక్షరం*) uprv_malloc(buflen);
        ఒకవేళ (buf != 0 && canon != 0) {
            U_ASSERT(id->extract(0, INT32_MAX, buf, buflen) < buflen);
            UError_సంకేతం ec = U_ZERO_ERROR;
            uloc_canonicalize(buf, canon, buflen, &ec);
            ఒకవేళ (U_SUCCESS(ec)) {
                ఫలితం = UnicodeString(canon);
            }
        }
        uprv_free(buf);
        uprv_free(canon);
    }
    తిరిగి ఫలితం;
#అయితే_ముగింపు
}

Locale&
LocaleUtility::initLocaleFromName(స్థిరరాశి UnicodeString& id, Locale& ఫలితం)
{
    ఒకవేళ (id.isBogus()) {
        ఫలితం.సమితి_గమ్యం_Bogus();
    } లేకపోతే {
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
        CharString బఫరు;
        int32_t prev, i;
        prev = 0;
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        చేయి {
            i = id.సూచీ_యొక్క(static_cast<char16_t>(0x40), prev);
            ఒకవేళ(i < 0) {
                // no @ between prev and the rest of the string
                బఫరు.appendInvariantChars(id.తాత్కాలిక_Sub_String(prev), స్థితి_2);
                ఆపు; //పూర్తి
            } లేకపోతే {
                // normal invariant-character conversion for text between @s
                బఫరు.appendInvariantChars(id.తాత్కాలిక_Sub_String(prev, i - prev), స్థితి_2);
                // manually "convert" U+0040 at id[i] into '@' at buffer[i]
                బఫరు.append('@', స్థితి_2);
                prev = i + 1;
            }
        } అయితే (U_SUCCESS(స్థితి_2));
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            ఫలితం.సమితి_గమ్యం_Bogus();
        } లేకపోతే {
            ఫలితం = Locale::createFromName(బఫరు.దత్తాంశం());
        }
    }
    తిరిగి ఫలితం;
}

UnicodeString&
LocaleUtility::initNameFromLocale(స్థిరరాశి Locale& locale, UnicodeString& ఫలితం)
{
    ఒకవేళ (locale.isBogus()) {
        ఫలితం.సమితి_గమ్యం_Bogus();
    } లేకపోతే {
        ఫలితం.append(UnicodeString(locale.getName(), -1, US_INV));
    }
    తిరిగి ఫలితం;
}

స్థిరరాశి Hashtable*
LocaleUtility::get_అందుబాటు_Locale_పేర్లు(స్థిరరాశి UnicodeString& bundleID)
{
    // LocaleUtility_cache is a hash-of-hashes.  The top-level keys
    // are path strings ('bundleID') passed to
    // ures_openAvailableLocales.  The top-level values are
    // second-level hashes.  The second-level keys are result strings
    // from ures_openAvailableLocales.  The second-level values are
    // garbage ((void*)1 or other random pointer).

    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    umtx_initOnce(LocaleUtilityInitOnce, locale_utility_init, స్థితి_2);
    Hashtable *cache = LocaleUtility_cache;
    ఒకవేళ (cache == శూన్యసూచిక) {
        // Catastrophic failure.
        తిరిగి శూన్యసూచిక;
    }

    Hashtable* htp;
    umtx_తాళం(శూన్యసూచిక);
    htp = static_cast<Hashtable*>(cache->get(bundleID));
    umtx_unlock(శూన్యసూచిక);

    ఒకవేళ (htp == శూన్యసూచిక) {
        htp = కొత్త Hashtable(స్థితి_2);
        ఒకవేళ (htp && U_SUCCESS(స్థితి_2)) {
            CharString cbundleID;
            cbundleID.appendInvariantChars(bundleID, స్థితి_2);
            స్థిరరాశి అక్షరం* మార్గం = cbundleID.is_ఖాళీలు() ? శూన్యసూచిక : cbundleID.దత్తాంశం();
            icu::Local_UEnumeration_సూచిక uenum(ures_open_అందుబాటు_Locales(మార్గం, &స్థితి_2));
            కొరకు (;;) {
                స్థిరరాశి char16_t* id = uenum_unext(uenum.getAlias(), శూన్యసూచిక, &స్థితి_2);
                ఒకవేళ (id == శూన్యసూచిక) {
                    ఆపు;
                }
                htp->put(UnicodeString(id), (శూన్య*)htp, స్థితి_2);
            }
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు htp;
                తిరిగి శూన్యసూచిక;
            }
            umtx_తాళం(శూన్యసూచిక);
            Hashtable *t = static_cast<Hashtable *>(cache->get(bundleID));
            ఒకవేళ (t != శూన్యసూచిక) {
                // Another thread raced through this code, creating the cache entry first.
                // Discard ours and return theirs.
                umtx_unlock(శూన్యసూచిక);
                తొలగించు htp;
                htp = t;
            } లేకపోతే {
                cache->put(bundleID, (శూన్య*)htp, స్థితి_2);
                umtx_unlock(శూన్యసూచిక);
            }
        }
    }
    తిరిగి htp;
}

తార్కిక
LocaleUtility::is_Fallback_యొక్క(స్థిరరాశి UnicodeString& మూలం, స్థిరరాశి UnicodeString& బిడ్డ)
{
    తిరిగి బిడ్డ.సూచీ_యొక్క(మూలం) == 0 &&
      (బిడ్డ.length() == మూలం.length() ||
       బిడ్డ.charAt(మూలం.length()) == UNDERSCORE_CHAR);
}

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు
