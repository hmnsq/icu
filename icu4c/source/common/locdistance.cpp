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
constexpr int32_t سرا_دا_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t فاصلہ_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t فاصلہ_IS_FINAL = 0x100;
constexpr int32_t فاصلہ_IS_FINAL_OR_SKIP_SCRIPT = فاصلہ_IS_FINAL | فاصلہ_SKIP_SCRIPT;

constexpr int32_t ABOVE_حد = 100;

// Indexes into array of distances.
تعداد {
    IX_DEF_LANG_فاصلہ,
    IX_DEF_SCRIPT_فاصلہ,
    IX_DEF_REGION_فاصلہ,
    IX_MIN_REGION_فاصلہ,
    IX_حد_بندی
};

Locale_فاصلہ *g_Locale_فاصلہ = خالی_سوچک;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    مٹاؤ g_Locale_فاصلہ;
    g_Locale_فاصلہ = خالی_سوچک;
    gInitOnce.reset();
    أرجع سچ;
}

}  // namespace

فراغ U_CALLCONV Locale_فاصلہ::init_Locale_فاصلہ(UError_کوڈ &error_کوڈ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_فاصلہ == خالی_سوچک);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    ثابت Locale_فاصلہ_انکڑے &انکڑے = likely.get_فاصلہ_انکڑے();
    إذا (انکڑے.فاصلہ_Trie_بائٹاں == خالی_سوچک ||
            انکڑے.region_منزل_Partitions == خالی_سوچک || انکڑے.partitions == خالی_سوچک ||
            // ok if no paradigms
            انکڑے.distances == خالی_سوچک) {
        error_کوڈ = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }
    g_Locale_فاصلہ = جديد Locale_فاصلہ(انکڑے, likely);
    إذا (g_Locale_فاصلہ == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_فاصلہ, cleanup);
}

ثابت Locale_فاصلہ *Locale_فاصلہ::getSingleton(UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(gInitOnce, &Locale_فاصلہ::init_Locale_فاصلہ, error_کوڈ);
    أرجع g_Locale_فاصلہ;
}

Locale_فاصلہ::Locale_فاصلہ(ثابت Locale_فاصلہ_انکڑے &انکڑے, ثابت LikelySubtags &likely) :
        likelySubtags(likely),
        trie(انکڑے.فاصلہ_Trie_بائٹاں),
        region_منزل_Partitions_اشاریہ(انکڑے.region_منزل_Partitions), حصے_کرو_Arrays(انکڑے.partitions),
        paradigmLSRs(انکڑے.paradigms), paradigm_LSRs_لمائی(انکڑے.paradigms_لمائی),
        default_Language_فاصلہ(انکڑے.distances[IX_DEF_LANG_فاصلہ]),
        default_Script_فاصلہ(انکڑے.distances[IX_DEF_SCRIPT_فاصلہ]),
        default_Region_فاصلہ(انکڑے.distances[IX_DEF_REGION_فاصلہ]),
        min_Region_فاصلہ(انکڑے.distances[IX_MIN_REGION_فاصلہ]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ثابت LSR *p_enGB = &enGB;
    int32_t اشاریہ_And_فاصلہ = get_بہترین_اشاریہ_And_فاصلہ(en, &p_enGB, 1,
            کھسکاؤ_فاصلہ(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_فی_Desired_Locale  = get_فاصلہ_Floor(اشاریہ_And_فاصلہ);
}

int32_t Locale_فاصلہ::get_بہترین_اشاریہ_And_فاصلہ(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supported_LSRs_لمائی,
        int32_t shifted_حد,
        ULoc_میل_Favor_Subtag favorSubtag, ULoc_میل_Direction direction) ثابت {
    بائٹاں_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_فاصلہ = trie_اگلا(iter, desired.language, خطأ);
    uint64_t desLangState = des_Lang_فاصلہ >= 0 && supported_LSRs_لمائی > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t بہترین_اشاریہ = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t بہترین_Likely_Info = -1;
    لكل (int32_t sl_اشاریہ = 0; sl_اشاریہ < supported_LSRs_لمائی; ++sl_اشاریہ) {
        ثابت LSR &supported = *supportedLSRs[sl_اشاریہ];
        منطقي تارا = خطأ;
        int32_t فاصلہ = des_Lang_فاصلہ;
        إذا (فاصلہ >= 0) {
            U_ASSERT((فاصلہ & فاصلہ_IS_FINAL) == 0);
            إذا (sl_اشاریہ != 0) {
                iter.reset_منزل_State64(desLangState);
            }
            فاصلہ = trie_اگلا(iter, supported.language, سچ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        إذا (فاصلہ >= 0) {
            flags = فاصلہ & فاصلہ_IS_FINAL_OR_SKIP_SCRIPT;
            فاصلہ &= ~فاصلہ_IS_FINAL_OR_SKIP_SCRIPT;
        } وإلا {  // <*, *>
            إذا (uprv_strcmp(desired.language, supported.language) == 0) {
                فاصلہ = 0;
            } وإلا {
                فاصلہ = default_Language_فاصلہ;
            }
            flags = 0;
            تارا = سچ;
        }
        U_ASSERT(0 <= فاصلہ && فاصلہ <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_حد = (shifted_حد + فاصلہ_FRACTION_نقاب) >> فاصلہ_کھسکاؤ;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        إذا (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            فاصلہ >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        إذا (فاصلہ > rounded_حد) {
            استمر;
        }

        int32_t script_فاصلہ;
        إذا (تارا || flags != 0) {
            إذا (uprv_strcmp(desired.script, supported.script) == 0) {
                script_فاصلہ = 0;
            } وإلا {
                script_فاصلہ = default_Script_فاصلہ;
            }
        } وإلا {
            script_فاصلہ = get_Des_Supp_Script_فاصلہ(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_فاصلہ & فاصلہ_IS_FINAL;
            script_فاصلہ &= ~فاصلہ_IS_FINAL;
        }
        فاصلہ += script_فاصلہ;
        إذا (فاصلہ > rounded_حد) {
            استمر;
        }

        إذا (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } وإلا إذا (تارا || (flags & فاصلہ_IS_FINAL) != 0) {
            فاصلہ += default_Region_فاصلہ;
        } وإلا {
            int32_t بچیا_حد = rounded_حد - فاصلہ;
            إذا (min_Region_فاصلہ > بچیا_حد) {
                استمر;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            فاصلہ += get_Region_Partitions_فاصلہ(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    بچیا_حد);
        }
        int32_t shifted_فاصلہ = کھسکاؤ_فاصلہ(فاصلہ);
        إذا (shifted_فاصلہ == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_فاصلہ |= (desired.flags ^ supported.flags);
            إذا (shifted_فاصلہ < shifted_حد) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_میل(supported, desired, shifted_حد, favorSubtag)) {
                    إذا (shifted_فاصلہ == 0) {
                        أرجع sl_اشاریہ << اشاریہ_کھسکاؤ;
                    }
                    بہترین_اشاریہ = sl_اشاریہ;
                    shifted_حد = shifted_فاصلہ;
                    بہترین_Likely_Info = -1;
                }
            }
        } وإلا {
            إذا (shifted_فاصلہ < shifted_حد) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_میل(supported, desired, shifted_حد, favorSubtag)) {
                    بہترین_اشاریہ = sl_اشاریہ;
                    shifted_حد = shifted_فاصلہ;
                    بہترین_Likely_Info = -1;
                }
            } وإلا إذا (shifted_فاصلہ == shifted_حد && بہترین_اشاریہ >= 0) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_میل(supported, desired, shifted_حد, favorSubtag)) {
                    بہترین_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[بہترین_اشاریہ], بہترین_Likely_Info);
                    إذا ((بہترین_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        بہترین_اشاریہ = sl_اشاریہ;
                    }
                }
            }
        }
    }
    أرجع بہترین_اشاریہ >= 0 ?
            (بہترین_اشاریہ << اشاریہ_کھسکاؤ) | shifted_حد :
            اشاریہ_NEG_1 | کھسکاؤ_فاصلہ(ABOVE_حد);
}

int32_t Locale_فاصلہ::get_Des_Supp_Script_فاصلہ(
        بائٹاں_Trie &iter, uint64_t شروع_State, ثابت حرف *desired, ثابت حرف *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t فاصلہ = trie_اگلا(iter, desired, خطأ);
    إذا (فاصلہ >= 0) {
        فاصلہ = trie_اگلا(iter, supported, سچ);
    }
    إذا (فاصلہ < 0) {
        UString_Trie_نتیجہ نتیجہ = iter.reset_منزل_State64(شروع_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_قدر(نتیجہ));
        إذا (uprv_strcmp(desired, supported) == 0) {
            فاصلہ = 0;  // same script
        } وإلا {
            فاصلہ = iter.get_قدر();
            U_ASSERT(فاصلہ >= 0);
        }
        إذا (نتیجہ == USTRINGTRIE_FINAL_قدر) {
            فاصلہ |= فاصلہ_IS_FINAL;
        }
    }
    أرجع فاصلہ;
}

int32_t Locale_فاصلہ::get_Region_Partitions_فاصلہ(
        بائٹاں_Trie &iter, uint64_t شروع_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions, int32_t حد) {
    حرف desired = *desiredPartitions++;
    حرف supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقي supp_لمائی_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    إذا (*desiredPartitions == 0 && !supp_لمائی_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_نتیجہ نتیجہ = iter.next(uprv_inv_Char_منزل_Ascii(desired) | سرا_دا_SUBTAG);
        إذا (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            نتیجہ = iter.next(uprv_inv_Char_منزل_Ascii(supported) | سرا_دا_SUBTAG);
            إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                أرجع iter.get_قدر();
            }
        }
        أرجع get_Fallback_Region_فاصلہ(iter, شروع_State);
    }

    ثابت حرف *supported_شروع = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_فاصلہ = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقي تارا = خطأ;
    لكل (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_نتیجہ نتیجہ = iter.next(uprv_inv_Char_منزل_Ascii(desired) | سرا_دا_SUBTAG);
        إذا (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            uint64_t desState = supp_لمائی_Gt1 ? iter.getState64() : 0;
            لكل (;;) {
                نتیجہ = iter.next(uprv_inv_Char_منزل_Ascii(supported) | سرا_دا_SUBTAG);
                int32_t d;
                إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                    d = iter.get_قدر();
                } وإلا إذا (تارا) {
                    d = 0;
                } وإلا {
                    d = get_Fallback_Region_فاصلہ(iter, شروع_State);
                    تارا = سچ;
                }
                إذا (d > حد) {
                    أرجع d;
                } وإلا إذا (region_فاصلہ < d) {
                    region_فاصلہ = d;
                }
                إذا ((supported = *supportedPartitions++) != 0) {
                    iter.reset_منزل_State64(desState);
                } وإلا {
                    اكسر;
                }
            }
        } وإلا إذا (!تارا) {
            int32_t d = get_Fallback_Region_فاصلہ(iter, شروع_State);
            إذا (d > حد) {
                أرجع d;
            } وإلا إذا (region_فاصلہ < d) {
                region_فاصلہ = d;
            }
            تارا = سچ;
        }
        إذا ((desired = *desiredPartitions++) != 0) {
            iter.reset_منزل_State64(شروع_State);
            supportedPartitions = supported_شروع;
            supported = *supportedPartitions++;
        } وإلا {
            اكسر;
        }
    }
    أرجع region_فاصلہ;
}

int32_t Locale_فاصلہ::get_Fallback_Region_فاصلہ(بائٹاں_Trie &iter, uint64_t شروع_State) {
#إذا U_DEBUG
    UString_Trie_نتیجہ نتیجہ =
#جے_انت
    iter.reset_منزل_State64(شروع_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_قدر(نتیجہ));
    int32_t فاصلہ = iter.get_قدر();
    U_ASSERT(فاصلہ >= 0);
    أرجع فاصلہ;
}

int32_t Locale_فاصلہ::trie_اگلا(بائٹاں_Trie &iter, ثابت حرف *s, منطقي want_قدر) {
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
            إذا (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                أرجع -1;
            }
        } وإلا {
            // last character of this subtag
            UString_Trie_نتیجہ نتیجہ = iter.next(c | سرا_دا_SUBTAG);
            إذا (want_قدر) {
                إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                    int32_t value = iter.get_قدر();
                    إذا (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                        value |= فاصلہ_IS_FINAL;
                    }
                    أرجع value;
                }
            } وإلا {
                إذا (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
                    أرجع 0;
                }
            }
            أرجع -1;
        }
        c = next;
    }
}

منطقي Locale_فاصلہ::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_لمائی <= 15);
    لكل (int32_t i = 0; i < paradigm_LSRs_لمائی; ++i) {
        إذا (lsr.is_Equivalent_منزل(paradigmLSRs[i])) { أرجع سچ; }
    }
    أرجع خطأ;
}

U_NAMESPACE_سرا
