// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "cstring.h"
#ضمن "locdistance.h"
#ضمن "loclikelysubtags.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uinvchar.h"
#ضمن "umutex.h"

U_NAMESPACE_BEGIN

نطاق {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t پای_ټکی_د_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t واټن_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t واټن_IS_FINAL = 0x100;
constexpr int32_t واټن_IS_FINAL_OR_SKIP_SCRIPT = واټن_IS_FINAL | واټن_SKIP_SCRIPT;

constexpr int32_t ABOVE_پوله = 100;

// Indexes into array of distances.
تعداد {
    IX_DEF_LANG_واټن,
    IX_DEF_SCRIPT_واټن,
    IX_DEF_REGION_واټن,
    IX_MIN_REGION_واټن,
    IX_حد
};

Locale_واټن *g_Locale_واټن = تش_اشاره;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ړنګول g_Locale_واټن;
    g_Locale_واټن = تش_اشاره;
    gInitOnce.reset();
    أرجع ریښتیا;
}

}  // namespace

فراغ U_CALLCONV Locale_واټن::init_Locale_واټن(UError_کوډ &error_کوډ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_واټن == تش_اشاره);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    ثابت Locale_واټن_معلومات &معلومات = likely.get_واټن_معلومات();
    إذا (معلومات.واټن_Trie_بایټونه == تش_اشاره ||
            معلومات.region_منزل_Partitions == تش_اشاره || معلومات.partitions == تش_اشاره ||
            // ok if no paradigms
            معلومات.distances == تش_اشاره) {
        error_کوډ = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }
    g_Locale_واټن = جديد Locale_واټن(معلومات, likely);
    إذا (g_Locale_واټن == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE_واټن, cleanup);
}

ثابت Locale_واټن *Locale_واټن::getSingleton(UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(gInitOnce, &Locale_واټن::init_Locale_واټن, error_کوډ);
    أرجع g_Locale_واټن;
}

Locale_واټن::Locale_واټن(ثابت Locale_واټن_معلومات &معلومات, ثابت LikelySubtags &likely) :
        likelySubtags(likely),
        trie(معلومات.واټن_Trie_بایټونه),
        region_منزل_Partitions_شاخص(معلومات.region_منزل_Partitions), برخه_کول_Arrays(معلومات.partitions),
        paradigmLSRs(معلومات.paradigms), paradigm_LSRs_اوږدتیا(معلومات.paradigms_اوږدتیا),
        default_Language_واټن(معلومات.distances[IX_DEF_LANG_واټن]),
        default_Script_واټن(معلومات.distances[IX_DEF_SCRIPT_واټن]),
        default_Region_واټن(معلومات.distances[IX_DEF_REGION_واټن]),
        min_Region_واټن(معلومات.distances[IX_MIN_REGION_واټن]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ثابت LSR *p_enGB = &enGB;
    int32_t شاخص_And_واټن = get_غوره_شاخص_And_واټن(en, &p_enGB, 1,
            ښويدنه_واټن(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_فی_Desired_Locale  = get_واټن_Floor(شاخص_And_واټن);
}

int32_t Locale_واټن::get_غوره_شاخص_And_واټن(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supported_LSRs_اوږدتیا,
        int32_t shifted_پوله,
        ULoc_سمون_Favor_Subtag favorSubtag, ULoc_سمون_Direction direction) ثابت {
    بایټونه_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_واټن = trie_بل(iter, desired.language, خطأ);
    uint64_t desLangState = des_Lang_واټن >= 0 && supported_LSRs_اوږدتیا > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t غوره_شاخص = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t غوره_Likely_Info = -1;
    لكل (int32_t sl_شاخص = 0; sl_شاخص < supported_LSRs_اوږدتیا; ++sl_شاخص) {
        ثابت LSR &supported = *supportedLSRs[sl_شاخص];
        منطقي ستوری = خطأ;
        int32_t واټن = des_Lang_واټن;
        إذا (واټن >= 0) {
            U_ASSERT((واټن & واټن_IS_FINAL) == 0);
            إذا (sl_شاخص != 0) {
                iter.reset_منزل_State64(desLangState);
            }
            واټن = trie_بل(iter, supported.language, ریښتیا);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        إذا (واټن >= 0) {
            flags = واټن & واټن_IS_FINAL_OR_SKIP_SCRIPT;
            واټن &= ~واټن_IS_FINAL_OR_SKIP_SCRIPT;
        } وإلا {  // <*, *>
            إذا (uprv_strcmp(desired.language, supported.language) == 0) {
                واټن = 0;
            } وإلا {
                واټن = default_Language_واټن;
            }
            flags = 0;
            ستوری = ریښتیا;
        }
        U_ASSERT(0 <= واټن && واټن <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_پوله = (shifted_پوله + واټن_FRACTION_ماسک) >> واټن_ښويدنه;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        إذا (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            واټن >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        إذا (واټن > rounded_پوله) {
            استمر;
        }

        int32_t script_واټن;
        إذا (ستوری || flags != 0) {
            إذا (uprv_strcmp(desired.script, supported.script) == 0) {
                script_واټن = 0;
            } وإلا {
                script_واټن = default_Script_واټن;
            }
        } وإلا {
            script_واټن = get_Des_Supp_Script_واټن(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_واټن & واټن_IS_FINAL;
            script_واټن &= ~واټن_IS_FINAL;
        }
        واټن += script_واټن;
        إذا (واټن > rounded_پوله) {
            استمر;
        }

        إذا (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } وإلا إذا (ستوری || (flags & واټن_IS_FINAL) != 0) {
            واټن += default_Region_واټن;
        } وإلا {
            int32_t پاتې_شوی_پوله = rounded_پوله - واټن;
            إذا (min_Region_واټن > پاتې_شوی_پوله) {
                استمر;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            واټن += get_Region_Partitions_واټن(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    پاتې_شوی_پوله);
        }
        int32_t shifted_واټن = ښويدنه_واټن(واټن);
        إذا (shifted_واټن == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_واټن |= (desired.flags ^ supported.flags);
            إذا (shifted_واټن < shifted_پوله) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_سمون(supported, desired, shifted_پوله, favorSubtag)) {
                    إذا (shifted_واټن == 0) {
                        أرجع sl_شاخص << شاخص_ښويدنه;
                    }
                    غوره_شاخص = sl_شاخص;
                    shifted_پوله = shifted_واټن;
                    غوره_Likely_Info = -1;
                }
            }
        } وإلا {
            إذا (shifted_واټن < shifted_پوله) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_سمون(supported, desired, shifted_پوله, favorSubtag)) {
                    غوره_شاخص = sl_شاخص;
                    shifted_پوله = shifted_واټن;
                    غوره_Likely_Info = -1;
                }
            } وإلا إذا (shifted_واټن == shifted_پوله && غوره_شاخص >= 0) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_سمون(supported, desired, shifted_پوله, favorSubtag)) {
                    غوره_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[غوره_شاخص], غوره_Likely_Info);
                    إذا ((غوره_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        غوره_شاخص = sl_شاخص;
                    }
                }
            }
        }
    }
    أرجع غوره_شاخص >= 0 ?
            (غوره_شاخص << شاخص_ښويدنه) | shifted_پوله :
            شاخص_NEG_1 | ښويدنه_واټن(ABOVE_پوله);
}

int32_t Locale_واټن::get_Des_Supp_Script_واټن(
        بایټونه_Trie &iter, uint64_t پیل_State, ثابت حرف *desired, ثابت حرف *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t واټن = trie_بل(iter, desired, خطأ);
    إذا (واټن >= 0) {
        واټن = trie_بل(iter, supported, ریښتیا);
    }
    إذا (واټن < 0) {
        UString_Trie_پایله پایله = iter.reset_منزل_State64(پیل_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_ارزښت(پایله));
        إذا (uprv_strcmp(desired, supported) == 0) {
            واټن = 0;  // same script
        } وإلا {
            واټن = iter.get_ارزښت();
            U_ASSERT(واټن >= 0);
        }
        إذا (پایله == USTRINGTRIE_FINAL_ارزښت) {
            واټن |= واټن_IS_FINAL;
        }
    }
    أرجع واټن;
}

int32_t Locale_واټن::get_Region_Partitions_واټن(
        بایټونه_Trie &iter, uint64_t پیل_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions, int32_t پوله) {
    حرف desired = *desiredPartitions++;
    حرف supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقي supp_اوږدتیا_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    إذا (*desiredPartitions == 0 && !supp_اوږدتیا_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_پایله پایله = iter.next(uprv_inv_Char_منزل_Ascii(desired) | پای_ټکی_د_SUBTAG);
        إذا (USTRINGTRIE_HAS_بل(پایله)) {
            پایله = iter.next(uprv_inv_Char_منزل_Ascii(supported) | پای_ټکی_د_SUBTAG);
            إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
                أرجع iter.get_ارزښت();
            }
        }
        أرجع get_Fallback_Region_واټن(iter, پیل_State);
    }

    ثابت حرف *supported_پیل = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_واټن = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقي ستوری = خطأ;
    لكل (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_پایله پایله = iter.next(uprv_inv_Char_منزل_Ascii(desired) | پای_ټکی_د_SUBTAG);
        إذا (USTRINGTRIE_HAS_بل(پایله)) {
            uint64_t desState = supp_اوږدتیا_Gt1 ? iter.getState64() : 0;
            لكل (;;) {
                پایله = iter.next(uprv_inv_Char_منزل_Ascii(supported) | پای_ټکی_د_SUBTAG);
                int32_t d;
                إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
                    d = iter.get_ارزښت();
                } وإلا إذا (ستوری) {
                    d = 0;
                } وإلا {
                    d = get_Fallback_Region_واټن(iter, پیل_State);
                    ستوری = ریښتیا;
                }
                إذا (d > پوله) {
                    أرجع d;
                } وإلا إذا (region_واټن < d) {
                    region_واټن = d;
                }
                إذا ((supported = *supportedPartitions++) != 0) {
                    iter.reset_منزل_State64(desState);
                } وإلا {
                    اكسر;
                }
            }
        } وإلا إذا (!ستوری) {
            int32_t d = get_Fallback_Region_واټن(iter, پیل_State);
            إذا (d > پوله) {
                أرجع d;
            } وإلا إذا (region_واټن < d) {
                region_واټن = d;
            }
            ستوری = ریښتیا;
        }
        إذا ((desired = *desiredPartitions++) != 0) {
            iter.reset_منزل_State64(پیل_State);
            supportedPartitions = supported_پیل;
            supported = *supportedPartitions++;
        } وإلا {
            اكسر;
        }
    }
    أرجع region_واټن;
}

int32_t Locale_واټن::get_Fallback_Region_واټن(بایټونه_Trie &iter, uint64_t پیل_State) {
#إذا U_DEBUG
    UString_Trie_پایله پایله =
#د_که_پای
    iter.reset_منزل_State64(پیل_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_ارزښت(پایله));
    int32_t واټن = iter.get_ارزښت();
    U_ASSERT(واټن >= 0);
    أرجع واټن;
}

int32_t Locale_واټن::trie_بل(بایټونه_Trie &iter, ثابت حرف *s, منطقي want_ارزښت) {
    uint8_t c;
    إذا ((c = *s) == 0) {
        أرجع -1;  // no empty subtags in the distance data
    }
    لكل (;;) {
        c = uprv_inv_Char_منزل_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        إذا (next != 0) {
            إذا (!USTRINGTRIE_HAS_بل(iter.next(c))) {
                أرجع -1;
            }
        } وإلا {
            // last character of this subtag
            UString_Trie_پایله پایله = iter.next(c | پای_ټکی_د_SUBTAG);
            إذا (want_ارزښت) {
                إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
                    int32_t value = iter.get_ارزښت();
                    إذا (پایله == USTRINGTRIE_FINAL_ارزښت) {
                        value |= واټن_IS_FINAL;
                    }
                    أرجع value;
                }
            } وإلا {
                إذا (USTRINGTRIE_HAS_بل(پایله)) {
                    أرجع 0;
                }
            }
            أرجع -1;
        }
        c = next;
    }
}

منطقي Locale_واټن::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_اوږدتیا <= 15);
    لكل (int32_t i = 0; i < paradigm_LSRs_اوږدتیا; ++i) {
        إذا (lsr.is_Equivalent_منزل(paradigmLSRs[i])) { أرجع ریښتیا; }
    }
    أرجع خطأ;
}

U_NAMESPACE_پای_ټکی
