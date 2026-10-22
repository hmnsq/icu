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
constexpr int32_t پڇاڙي_جو_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t مفاصلو_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t مفاصلو_IS_FINAL = 0x100;
constexpr int32_t مفاصلو_IS_FINAL_OR_SKIP_SCRIPT = مفاصلو_IS_FINAL | مفاصلو_SKIP_SCRIPT;

constexpr int32_t ABOVE_حد = 100;

// Indexes into array of distances.
تعداد {
    IX_DEF_LANG_مفاصلو,
    IX_DEF_SCRIPT_مفاصلو,
    IX_DEF_REGION_مفاصلو,
    IX_MIN_REGION_مفاصلو,
    IX_حد_بندي
};

Locale_مفاصلو *g_Locale_مفاصلو = خالي_اشارو;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ميٽيو g_Locale_مفاصلو;
    g_Locale_مفاصلو = خالي_اشارو;
    gInitOnce.reset();
    أرجع سچ;
}

}  // namespace

فراغ U_CALLCONV Locale_مفاصلو::init_Locale_مفاصلو(UError_ڪوڊ &error_ڪوڊ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_مفاصلو == خالي_اشارو);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    ثابت Locale_مفاصلو_معلومات &معلومات = likely.get_مفاصلو_معلومات();
    إذا (معلومات.مفاصلو_Trie_بائيٽون == خالي_اشارو ||
            معلومات.region_منزل_Partitions == خالي_اشارو || معلومات.partitions == خالي_اشارو ||
            // ok if no paradigms
            معلومات.distances == خالي_اشارو) {
        error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }
    g_Locale_مفاصلو = جديد Locale_مفاصلو(معلومات, likely);
    إذا (g_Locale_مفاصلو == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE_مفاصلو, cleanup);
}

ثابت Locale_مفاصلو *Locale_مفاصلو::getSingleton(UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(gInitOnce, &Locale_مفاصلو::init_Locale_مفاصلو, error_ڪوڊ);
    أرجع g_Locale_مفاصلو;
}

Locale_مفاصلو::Locale_مفاصلو(ثابت Locale_مفاصلو_معلومات &معلومات, ثابت LikelySubtags &likely) :
        likelySubtags(likely),
        trie(معلومات.مفاصلو_Trie_بائيٽون),
        region_منزل_Partitions_اشاريو(معلومات.region_منزل_Partitions), حصا_ڪريو_Arrays(معلومات.partitions),
        paradigmLSRs(معلومات.paradigms), paradigm_LSRs_ڊيگهائي(معلومات.paradigms_ڊيگهائي),
        default_Language_مفاصلو(معلومات.distances[IX_DEF_LANG_مفاصلو]),
        default_Script_مفاصلو(معلومات.distances[IX_DEF_SCRIPT_مفاصلو]),
        default_Region_مفاصلو(معلومات.distances[IX_DEF_REGION_مفاصلو]),
        min_Region_مفاصلو(معلومات.distances[IX_MIN_REGION_مفاصلو]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ثابت LSR *p_enGB = &enGB;
    int32_t اشاريو_And_مفاصلو = get_بهترين_اشاريو_And_مفاصلو(en, &p_enGB, 1,
            سرڪايو_مفاصلو(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_في_Desired_Locale  = get_مفاصلو_Floor(اشاريو_And_مفاصلو);
}

int32_t Locale_مفاصلو::get_بهترين_اشاريو_And_مفاصلو(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supported_LSRs_ڊيگهائي,
        int32_t shifted_حد,
        ULoc_ميلاپ_Favor_Subtag favorSubtag, ULoc_ميلاپ_Direction direction) ثابت {
    بائيٽون_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_مفاصلو = trie_ايندڙ(iter, desired.language, خطأ);
    uint64_t desLangState = des_Lang_مفاصلو >= 0 && supported_LSRs_ڊيگهائي > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t بهترين_اشاريو = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t بهترين_Likely_Info = -1;
    لكل (int32_t sl_اشاريو = 0; sl_اشاريو < supported_LSRs_ڊيگهائي; ++sl_اشاريو) {
        ثابت LSR &supported = *supportedLSRs[sl_اشاريو];
        منطقي تارو = خطأ;
        int32_t مفاصلو = des_Lang_مفاصلو;
        إذا (مفاصلو >= 0) {
            U_ASSERT((مفاصلو & مفاصلو_IS_FINAL) == 0);
            إذا (sl_اشاريو != 0) {
                iter.reset_منزل_State64(desLangState);
            }
            مفاصلو = trie_ايندڙ(iter, supported.language, سچ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        إذا (مفاصلو >= 0) {
            flags = مفاصلو & مفاصلو_IS_FINAL_OR_SKIP_SCRIPT;
            مفاصلو &= ~مفاصلو_IS_FINAL_OR_SKIP_SCRIPT;
        } وإلا {  // <*, *>
            إذا (uprv_strcmp(desired.language, supported.language) == 0) {
                مفاصلو = 0;
            } وإلا {
                مفاصلو = default_Language_مفاصلو;
            }
            flags = 0;
            تارو = سچ;
        }
        U_ASSERT(0 <= مفاصلو && مفاصلو <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_حد = (shifted_حد + مفاصلو_FRACTION_نقاب) >> مفاصلو_سرڪايو;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        إذا (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            مفاصلو >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        إذا (مفاصلو > rounded_حد) {
            استمر;
        }

        int32_t script_مفاصلو;
        إذا (تارو || flags != 0) {
            إذا (uprv_strcmp(desired.script, supported.script) == 0) {
                script_مفاصلو = 0;
            } وإلا {
                script_مفاصلو = default_Script_مفاصلو;
            }
        } وإلا {
            script_مفاصلو = get_Des_Supp_Script_مفاصلو(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_مفاصلو & مفاصلو_IS_FINAL;
            script_مفاصلو &= ~مفاصلو_IS_FINAL;
        }
        مفاصلو += script_مفاصلو;
        إذا (مفاصلو > rounded_حد) {
            استمر;
        }

        إذا (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } وإلا إذا (تارو || (flags & مفاصلو_IS_FINAL) != 0) {
            مفاصلو += default_Region_مفاصلو;
        } وإلا {
            int32_t بچيل_حد = rounded_حد - مفاصلو;
            إذا (min_Region_مفاصلو > بچيل_حد) {
                استمر;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            مفاصلو += get_Region_Partitions_مفاصلو(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    بچيل_حد);
        }
        int32_t shifted_مفاصلو = سرڪايو_مفاصلو(مفاصلو);
        إذا (shifted_مفاصلو == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_مفاصلو |= (desired.flags ^ supported.flags);
            إذا (shifted_مفاصلو < shifted_حد) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ميلاپ(supported, desired, shifted_حد, favorSubtag)) {
                    إذا (shifted_مفاصلو == 0) {
                        أرجع sl_اشاريو << اشاريو_سرڪايو;
                    }
                    بهترين_اشاريو = sl_اشاريو;
                    shifted_حد = shifted_مفاصلو;
                    بهترين_Likely_Info = -1;
                }
            }
        } وإلا {
            إذا (shifted_مفاصلو < shifted_حد) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ميلاپ(supported, desired, shifted_حد, favorSubtag)) {
                    بهترين_اشاريو = sl_اشاريو;
                    shifted_حد = shifted_مفاصلو;
                    بهترين_Likely_Info = -1;
                }
            } وإلا إذا (shifted_مفاصلو == shifted_حد && بهترين_اشاريو >= 0) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ميلاپ(supported, desired, shifted_حد, favorSubtag)) {
                    بهترين_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[بهترين_اشاريو], بهترين_Likely_Info);
                    إذا ((بهترين_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        بهترين_اشاريو = sl_اشاريو;
                    }
                }
            }
        }
    }
    أرجع بهترين_اشاريو >= 0 ?
            (بهترين_اشاريو << اشاريو_سرڪايو) | shifted_حد :
            اشاريو_NEG_1 | سرڪايو_مفاصلو(ABOVE_حد);
}

int32_t Locale_مفاصلو::get_Des_Supp_Script_مفاصلو(
        بائيٽون_Trie &iter, uint64_t شروعات_State, ثابت حرف *desired, ثابت حرف *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t مفاصلو = trie_ايندڙ(iter, desired, خطأ);
    إذا (مفاصلو >= 0) {
        مفاصلو = trie_ايندڙ(iter, supported, سچ);
    }
    إذا (مفاصلو < 0) {
        UString_Trie_نتيجو نتيجو = iter.reset_منزل_State64(شروعات_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_قدر(نتيجو));
        إذا (uprv_strcmp(desired, supported) == 0) {
            مفاصلو = 0;  // same script
        } وإلا {
            مفاصلو = iter.get_قدر();
            U_ASSERT(مفاصلو >= 0);
        }
        إذا (نتيجو == USTRINGTRIE_FINAL_قدر) {
            مفاصلو |= مفاصلو_IS_FINAL;
        }
    }
    أرجع مفاصلو;
}

int32_t Locale_مفاصلو::get_Region_Partitions_مفاصلو(
        بائيٽون_Trie &iter, uint64_t شروعات_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions, int32_t حد) {
    حرف desired = *desiredPartitions++;
    حرف supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقي supp_ڊيگهائي_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    إذا (*desiredPartitions == 0 && !supp_ڊيگهائي_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_نتيجو نتيجو = iter.next(uprv_inv_Char_منزل_Ascii(desired) | پڇاڙي_جو_SUBTAG);
        إذا (USTRINGTRIE_HAS_ايندڙ(نتيجو)) {
            نتيجو = iter.next(uprv_inv_Char_منزل_Ascii(supported) | پڇاڙي_جو_SUBTAG);
            إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
                أرجع iter.get_قدر();
            }
        }
        أرجع get_Fallback_Region_مفاصلو(iter, شروعات_State);
    }

    ثابت حرف *supported_شروعات = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_مفاصلو = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقي تارو = خطأ;
    لكل (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_نتيجو نتيجو = iter.next(uprv_inv_Char_منزل_Ascii(desired) | پڇاڙي_جو_SUBTAG);
        إذا (USTRINGTRIE_HAS_ايندڙ(نتيجو)) {
            uint64_t desState = supp_ڊيگهائي_Gt1 ? iter.getState64() : 0;
            لكل (;;) {
                نتيجو = iter.next(uprv_inv_Char_منزل_Ascii(supported) | پڇاڙي_جو_SUBTAG);
                int32_t d;
                إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
                    d = iter.get_قدر();
                } وإلا إذا (تارو) {
                    d = 0;
                } وإلا {
                    d = get_Fallback_Region_مفاصلو(iter, شروعات_State);
                    تارو = سچ;
                }
                إذا (d > حد) {
                    أرجع d;
                } وإلا إذا (region_مفاصلو < d) {
                    region_مفاصلو = d;
                }
                إذا ((supported = *supportedPartitions++) != 0) {
                    iter.reset_منزل_State64(desState);
                } وإلا {
                    اكسر;
                }
            }
        } وإلا إذا (!تارو) {
            int32_t d = get_Fallback_Region_مفاصلو(iter, شروعات_State);
            إذا (d > حد) {
                أرجع d;
            } وإلا إذا (region_مفاصلو < d) {
                region_مفاصلو = d;
            }
            تارو = سچ;
        }
        إذا ((desired = *desiredPartitions++) != 0) {
            iter.reset_منزل_State64(شروعات_State);
            supportedPartitions = supported_شروعات;
            supported = *supportedPartitions++;
        } وإلا {
            اكسر;
        }
    }
    أرجع region_مفاصلو;
}

int32_t Locale_مفاصلو::get_Fallback_Region_مفاصلو(بائيٽون_Trie &iter, uint64_t شروعات_State) {
#إذا U_DEBUG
    UString_Trie_نتيجو نتيجو =
#جيڪڏهن_پورو
    iter.reset_منزل_State64(شروعات_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_قدر(نتيجو));
    int32_t مفاصلو = iter.get_قدر();
    U_ASSERT(مفاصلو >= 0);
    أرجع مفاصلو;
}

int32_t Locale_مفاصلو::trie_ايندڙ(بائيٽون_Trie &iter, ثابت حرف *s, منطقي want_قدر) {
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
            إذا (!USTRINGTRIE_HAS_ايندڙ(iter.next(c))) {
                أرجع -1;
            }
        } وإلا {
            // last character of this subtag
            UString_Trie_نتيجو نتيجو = iter.next(c | پڇاڙي_جو_SUBTAG);
            إذا (want_قدر) {
                إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
                    int32_t value = iter.get_قدر();
                    إذا (نتيجو == USTRINGTRIE_FINAL_قدر) {
                        value |= مفاصلو_IS_FINAL;
                    }
                    أرجع value;
                }
            } وإلا {
                إذا (USTRINGTRIE_HAS_ايندڙ(نتيجو)) {
                    أرجع 0;
                }
            }
            أرجع -1;
        }
        c = next;
    }
}

منطقي Locale_مفاصلو::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_ڊيگهائي <= 15);
    لكل (int32_t i = 0; i < paradigm_LSRs_ڊيگهائي; ++i) {
        إذا (lsr.is_Equivalent_منزل(paradigmLSRs[i])) { أرجع سچ; }
    }
    أرجع خطأ;
}

U_NAMESPACE_پڇاڙي
