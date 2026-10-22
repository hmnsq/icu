// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localematcher.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locdistance.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "loclikelysubtags.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t ਸਿਰਾ_ਦਾ_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t ਦੂਰੀ_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t ਦੂਰੀ_IS_FINAL = 0x100;
constexpr int32_t ਦੂਰੀ_IS_FINAL_OR_SKIP_SCRIPT = ਦੂਰੀ_IS_FINAL | ਦੂਰੀ_SKIP_SCRIPT;

constexpr int32_t ABOVE_ਹੱਦ = 100;

// Indexes into array of distances.
ਗਿਣਤੀ {
    IX_DEF_LANG_ਦੂਰੀ,
    IX_DEF_SCRIPT_ਦੂਰੀ,
    IX_DEF_REGION_ਦੂਰੀ,
    IX_MIN_REGION_ਦੂਰੀ,
    IX_ਹੱਦਬੰਦੀ
};

Locale_ਦੂਰੀ *g_Locale_ਦੂਰੀ = ਖਾਲੀ_ਸੂਚਕ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ਮਿਟਾਓ g_Locale_ਦੂਰੀ;
    g_Locale_ਦੂਰੀ = ਖਾਲੀ_ਸੂਚਕ;
    gInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

}  // namespace

ਖਾਲੀ U_CALLCONV Locale_ਦੂਰੀ::init_Locale_ਦੂਰੀ(UError_ਕੋਡ &error_ਕੋਡ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_ਦੂਰੀ == ਖਾਲੀ_ਸੂਚਕ);
    ਅਚਲ LikelySubtags &likely = *LikelySubtags::getSingleton(error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਅਚਲ Locale_ਦੂਰੀ_ਅੰਕੜੇ &ਅੰਕੜੇ = likely.get_ਦੂਰੀ_ਅੰਕੜੇ();
    ਜੇ (ਅੰਕੜੇ.ਦੂਰੀ_Trie_ਬਾਈਟਾਂ == ਖਾਲੀ_ਸੂਚਕ ||
            ਅੰਕੜੇ.region_ਮੰਜ਼ਿਲ_Partitions == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ.partitions == ਖਾਲੀ_ਸੂਚਕ ||
            // ok if no paradigms
            ਅੰਕੜੇ.distances == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ;
    }
    g_Locale_ਦੂਰੀ = ਨਵਾਂ Locale_ਦੂਰੀ(ਅੰਕੜੇ, likely);
    ਜੇ (g_Locale_ਦੂਰੀ == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE_ਦੂਰੀ, cleanup);
}

ਅਚਲ Locale_ਦੂਰੀ *Locale_ਦੂਰੀ::getSingleton(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(gInitOnce, &Locale_ਦੂਰੀ::init_Locale_ਦੂਰੀ, error_ਕੋਡ);
    ਵਾਪਸ g_Locale_ਦੂਰੀ;
}

Locale_ਦੂਰੀ::Locale_ਦੂਰੀ(ਅਚਲ Locale_ਦੂਰੀ_ਅੰਕੜੇ &ਅੰਕੜੇ, ਅਚਲ LikelySubtags &likely) :
        likelySubtags(likely),
        trie(ਅੰਕੜੇ.ਦੂਰੀ_Trie_ਬਾਈਟਾਂ),
        region_ਮੰਜ਼ਿਲ_Partitions_ਸੂਚਕਾਂਕ(ਅੰਕੜੇ.region_ਮੰਜ਼ਿਲ_Partitions), ਹਿੱਸੇ_ਕਰੋ_Arrays(ਅੰਕੜੇ.partitions),
        paradigmLSRs(ਅੰਕੜੇ.paradigms), paradigm_LSRs_ਲੰਮਾਈ(ਅੰਕੜੇ.paradigms_ਲੰਮਾਈ),
        default_Language_ਦੂਰੀ(ਅੰਕੜੇ.distances[IX_DEF_LANG_ਦੂਰੀ]),
        default_Script_ਦੂਰੀ(ਅੰਕੜੇ.distances[IX_DEF_SCRIPT_ਦੂਰੀ]),
        default_Region_ਦੂਰੀ(ਅੰਕੜੇ.distances[IX_DEF_REGION_ਦੂਰੀ]),
        min_Region_ਦੂਰੀ(ਅੰਕੜੇ.distances[IX_MIN_REGION_ਦੂਰੀ]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ਅਚਲ LSR *p_enGB = &enGB;
    int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ = get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(en, &p_enGB, 1,
            ਖਿਸਕਾਓ_ਦੂਰੀ(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_ਪ੍ਰਤੀ_Desired_Locale  = get_ਦੂਰੀ_Floor(ਸੂਚਕਾਂਕ_And_ਦੂਰੀ);
}

int32_t Locale_ਦੂਰੀ::get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
        ਅਚਲ LSR &desired,
        ਅਚਲ LSR **supportedLSRs, int32_t supported_LSRs_ਲੰਮਾਈ,
        int32_t shifted_ਹੱਦ,
        ULoc_ਮੇਲ_Favor_Subtag favorSubtag, ULoc_ਮੇਲ_Direction direction) ਅਚਲ {
    ਬਾਈਟਾਂ_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_ਦੂਰੀ = trie_ਅਗਲਾ(iter, desired.language, ਝੂਠ);
    uint64_t desLangState = des_Lang_ਦੂਰੀ >= 0 && supported_LSRs_ਲੰਮਾਈ > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t ਉੱਤਮ_ਸੂਚਕਾਂਕ = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t ਉੱਤਮ_Likely_Info = -1;
    ਲਈ (int32_t sl_ਸੂਚਕਾਂਕ = 0; sl_ਸੂਚਕਾਂਕ < supported_LSRs_ਲੰਮਾਈ; ++sl_ਸੂਚਕਾਂਕ) {
        ਅਚਲ LSR &supported = *supportedLSRs[sl_ਸੂਚਕਾਂਕ];
        ਤਾਰਕਿਕ ਤਾਰਾ = ਝੂਠ;
        int32_t ਦੂਰੀ = des_Lang_ਦੂਰੀ;
        ਜੇ (ਦੂਰੀ >= 0) {
            U_ASSERT((ਦੂਰੀ & ਦੂਰੀ_IS_FINAL) == 0);
            ਜੇ (sl_ਸੂਚਕਾਂਕ != 0) {
                iter.reset_ਮੰਜ਼ਿਲ_State64(desLangState);
            }
            ਦੂਰੀ = trie_ਅਗਲਾ(iter, supported.language, ਸੱਚ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        ਜੇ (ਦੂਰੀ >= 0) {
            flags = ਦੂਰੀ & ਦੂਰੀ_IS_FINAL_OR_SKIP_SCRIPT;
            ਦੂਰੀ &= ~ਦੂਰੀ_IS_FINAL_OR_SKIP_SCRIPT;
        } ਨਹੀਂਤਾਂ {  // <*, *>
            ਜੇ (uprv_strcmp(desired.language, supported.language) == 0) {
                ਦੂਰੀ = 0;
            } ਨਹੀਂਤਾਂ {
                ਦੂਰੀ = default_Language_ਦੂਰੀ;
            }
            flags = 0;
            ਤਾਰਾ = ਸੱਚ;
        }
        U_ASSERT(0 <= ਦੂਰੀ && ਦੂਰੀ <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_ਹੱਦ = (shifted_ਹੱਦ + ਦੂਰੀ_FRACTION_ਨਕਾਬ) >> ਦੂਰੀ_ਖਿਸਕਾਓ;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        ਜੇ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            ਦੂਰੀ >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        ਜੇ (ਦੂਰੀ > rounded_ਹੱਦ) {
            ਜਾਰੀ;
        }

        int32_t script_ਦੂਰੀ;
        ਜੇ (ਤਾਰਾ || flags != 0) {
            ਜੇ (uprv_strcmp(desired.script, supported.script) == 0) {
                script_ਦੂਰੀ = 0;
            } ਨਹੀਂਤਾਂ {
                script_ਦੂਰੀ = default_Script_ਦੂਰੀ;
            }
        } ਨਹੀਂਤਾਂ {
            script_ਦੂਰੀ = get_Des_Supp_Script_ਦੂਰੀ(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_ਦੂਰੀ & ਦੂਰੀ_IS_FINAL;
            script_ਦੂਰੀ &= ~ਦੂਰੀ_IS_FINAL;
        }
        ਦੂਰੀ += script_ਦੂਰੀ;
        ਜੇ (ਦੂਰੀ > rounded_ਹੱਦ) {
            ਜਾਰੀ;
        }

        ਜੇ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } ਨਹੀਂਤਾਂ ਜੇ (ਤਾਰਾ || (flags & ਦੂਰੀ_IS_FINAL) != 0) {
            ਦੂਰੀ += default_Region_ਦੂਰੀ;
        } ਨਹੀਂਤਾਂ {
            int32_t ਬਚਿਆ_ਹੱਦ = rounded_ਹੱਦ - ਦੂਰੀ;
            ਜੇ (min_Region_ਦੂਰੀ > ਬਚਿਆ_ਹੱਦ) {
                ਜਾਰੀ;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            ਦੂਰੀ += get_Region_Partitions_ਦੂਰੀ(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    ਬਚਿਆ_ਹੱਦ);
        }
        int32_t shifted_ਦੂਰੀ = ਖਿਸਕਾਓ_ਦੂਰੀ(ਦੂਰੀ);
        ਜੇ (shifted_ਦੂਰੀ == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_ਦੂਰੀ |= (desired.flags ^ supported.flags);
            ਜੇ (shifted_ਦੂਰੀ < shifted_ਹੱਦ) {
                ਜੇ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ਮੇਲ(supported, desired, shifted_ਹੱਦ, favorSubtag)) {
                    ਜੇ (shifted_ਦੂਰੀ == 0) {
                        ਵਾਪਸ sl_ਸੂਚਕਾਂਕ << ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
                    }
                    ਉੱਤਮ_ਸੂਚਕਾਂਕ = sl_ਸੂਚਕਾਂਕ;
                    shifted_ਹੱਦ = shifted_ਦੂਰੀ;
                    ਉੱਤਮ_Likely_Info = -1;
                }
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ (shifted_ਦੂਰੀ < shifted_ਹੱਦ) {
                ਜੇ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ਮੇਲ(supported, desired, shifted_ਹੱਦ, favorSubtag)) {
                    ਉੱਤਮ_ਸੂਚਕਾਂਕ = sl_ਸੂਚਕਾਂਕ;
                    shifted_ਹੱਦ = shifted_ਦੂਰੀ;
                    ਉੱਤਮ_Likely_Info = -1;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (shifted_ਦੂਰੀ == shifted_ਹੱਦ && ਉੱਤਮ_ਸੂਚਕਾਂਕ >= 0) {
                ਜੇ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ਮੇਲ(supported, desired, shifted_ਹੱਦ, favorSubtag)) {
                    ਉੱਤਮ_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[ਉੱਤਮ_ਸੂਚਕਾਂਕ], ਉੱਤਮ_Likely_Info);
                    ਜੇ ((ਉੱਤਮ_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        ਉੱਤਮ_ਸੂਚਕਾਂਕ = sl_ਸੂਚਕਾਂਕ;
                    }
                }
            }
        }
    }
    ਵਾਪਸ ਉੱਤਮ_ਸੂਚਕਾਂਕ >= 0 ?
            (ਉੱਤਮ_ਸੂਚਕਾਂਕ << ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ) | shifted_ਹੱਦ :
            ਸੂਚਕਾਂਕ_NEG_1 | ਖਿਸਕਾਓ_ਦੂਰੀ(ABOVE_ਹੱਦ);
}

int32_t Locale_ਦੂਰੀ::get_Des_Supp_Script_ਦੂਰੀ(
        ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State, ਅਚਲ ਅੱਖਰ *desired, ਅਚਲ ਅੱਖਰ *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t ਦੂਰੀ = trie_ਅਗਲਾ(iter, desired, ਝੂਠ);
    ਜੇ (ਦੂਰੀ >= 0) {
        ਦੂਰੀ = trie_ਅਗਲਾ(iter, supported, ਸੱਚ);
    }
    ਜੇ (ਦੂਰੀ < 0) {
        UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = iter.reset_ਮੰਜ਼ਿਲ_State64(ਸ਼ੁਰੂ_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ));
        ਜੇ (uprv_strcmp(desired, supported) == 0) {
            ਦੂਰੀ = 0;  // same script
        } ਨਹੀਂਤਾਂ {
            ਦੂਰੀ = iter.get_ਮੁੱਲ();
            U_ASSERT(ਦੂਰੀ >= 0);
        }
        ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_FINAL_ਮੁੱਲ) {
            ਦੂਰੀ |= ਦੂਰੀ_IS_FINAL;
        }
    }
    ਵਾਪਸ ਦੂਰੀ;
}

int32_t Locale_ਦੂਰੀ::get_Region_Partitions_ਦੂਰੀ(
        ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State,
        ਅਚਲ ਅੱਖਰ *desiredPartitions, ਅਚਲ ਅੱਖਰ *supportedPartitions, int32_t ਹੱਦ) {
    ਅੱਖਰ desired = *desiredPartitions++;
    ਅੱਖਰ supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    ਤਾਰਕਿਕ supp_ਲੰਮਾਈ_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    ਜੇ (*desiredPartitions == 0 && !supp_ਲੰਮਾਈ_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = iter.next(uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(desired) | ਸਿਰਾ_ਦਾ_SUBTAG);
        ਜੇ (USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ)) {
            ਨਤੀਜਾ = iter.next(uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(supported) | ਸਿਰਾ_ਦਾ_SUBTAG);
            ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
                ਵਾਪਸ iter.get_ਮੁੱਲ();
            }
        }
        ਵਾਪਸ get_Fallback_Region_ਦੂਰੀ(iter, ਸ਼ੁਰੂ_State);
    }

    ਅਚਲ ਅੱਖਰ *supported_ਸ਼ੁਰੂ = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_ਦੂਰੀ = 0;
    // Fall back to * only once, not for each pair of partition strings.
    ਤਾਰਕਿਕ ਤਾਰਾ = ਝੂਠ;
    ਲਈ (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = iter.next(uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(desired) | ਸਿਰਾ_ਦਾ_SUBTAG);
        ਜੇ (USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ)) {
            uint64_t desState = supp_ਲੰਮਾਈ_Gt1 ? iter.getState64() : 0;
            ਲਈ (;;) {
                ਨਤੀਜਾ = iter.next(uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(supported) | ਸਿਰਾ_ਦਾ_SUBTAG);
                int32_t d;
                ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
                    d = iter.get_ਮੁੱਲ();
                } ਨਹੀਂਤਾਂ ਜੇ (ਤਾਰਾ) {
                    d = 0;
                } ਨਹੀਂਤਾਂ {
                    d = get_Fallback_Region_ਦੂਰੀ(iter, ਸ਼ੁਰੂ_State);
                    ਤਾਰਾ = ਸੱਚ;
                }
                ਜੇ (d > ਹੱਦ) {
                    ਵਾਪਸ d;
                } ਨਹੀਂਤਾਂ ਜੇ (region_ਦੂਰੀ < d) {
                    region_ਦੂਰੀ = d;
                }
                ਜੇ ((supported = *supportedPartitions++) != 0) {
                    iter.reset_ਮੰਜ਼ਿਲ_State64(desState);
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ (!ਤਾਰਾ) {
            int32_t d = get_Fallback_Region_ਦੂਰੀ(iter, ਸ਼ੁਰੂ_State);
            ਜੇ (d > ਹੱਦ) {
                ਵਾਪਸ d;
            } ਨਹੀਂਤਾਂ ਜੇ (region_ਦੂਰੀ < d) {
                region_ਦੂਰੀ = d;
            }
            ਤਾਰਾ = ਸੱਚ;
        }
        ਜੇ ((desired = *desiredPartitions++) != 0) {
            iter.reset_ਮੰਜ਼ਿਲ_State64(ਸ਼ੁਰੂ_State);
            supportedPartitions = supported_ਸ਼ੁਰੂ;
            supported = *supportedPartitions++;
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ region_ਦੂਰੀ;
}

int32_t Locale_ਦੂਰੀ::get_Fallback_Region_ਦੂਰੀ(ਬਾਈਟਾਂ_Trie &iter, uint64_t ਸ਼ੁਰੂ_State) {
#ਜੇ U_DEBUG
    UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ =
#ਜੇ_ਅੰਤ
    iter.reset_ਮੰਜ਼ਿਲ_State64(ਸ਼ੁਰੂ_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ));
    int32_t ਦੂਰੀ = iter.get_ਮੁੱਲ();
    U_ASSERT(ਦੂਰੀ >= 0);
    ਵਾਪਸ ਦੂਰੀ;
}

int32_t Locale_ਦੂਰੀ::trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, ਅਚਲ ਅੱਖਰ *s, ਤਾਰਕਿਕ want_ਮੁੱਲ) {
    uint8_t c;
    ਜੇ ((c = *s) == 0) {
        ਵਾਪਸ -1;  // no empty subtags in the distance data
    }
    ਲਈ (;;) {
        c = uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        ਜੇ (next != 0) {
            ਜੇ (!USTRINGTRIE_HAS_ਅਗਲਾ(iter.next(c))) {
                ਵਾਪਸ -1;
            }
        } ਨਹੀਂਤਾਂ {
            // last character of this subtag
            UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = iter.next(c | ਸਿਰਾ_ਦਾ_SUBTAG);
            ਜੇ (want_ਮੁੱਲ) {
                ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
                    int32_t value = iter.get_ਮੁੱਲ();
                    ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_FINAL_ਮੁੱਲ) {
                        value |= ਦੂਰੀ_IS_FINAL;
                    }
                    ਵਾਪਸ value;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ (USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ)) {
                    ਵਾਪਸ 0;
                }
            }
            ਵਾਪਸ -1;
        }
        c = next;
    }
}

ਤਾਰਕਿਕ Locale_ਦੂਰੀ::isParadigmLSR(ਅਚਲ LSR &lsr) ਅਚਲ {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_ਲੰਮਾਈ <= 15);
    ਲਈ (int32_t i = 0; i < paradigm_LSRs_ਲੰਮਾਈ; ++i) {
        ਜੇ (lsr.is_Equivalent_ਮੰਜ਼ਿਲ(paradigmLSRs[i])) { ਵਾਪਸ ਸੱਚ; }
    }
    ਵਾਪਸ ਝੂਠ;
}

U_NAMESPACE_ਸਿਰਾ
