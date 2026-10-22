// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/localematcher.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "locdistance.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t ಕೊನೆ_ದ_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t ದೂರ_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t ದೂರ_IS_FINAL = 0x100;
constexpr int32_t ದೂರ_IS_FINAL_OR_SKIP_SCRIPT = ದೂರ_IS_FINAL | ದೂರ_SKIP_SCRIPT;

constexpr int32_t ABOVE_ಮಿತಿ = 100;

// Indexes into array of distances.
ಎಣಿಕೆ {
    IX_DEF_LANG_ದೂರ,
    IX_DEF_SCRIPT_ದೂರ,
    IX_DEF_REGION_ದೂರ,
    IX_MIN_REGION_ದೂರ,
    IX_ಎಲ್ಲೆ
};

Locale_ದೂರ *g_Locale_ದೂರ = ಶೂನ್ಯಸೂಚಕ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ಅಳಿಸು g_Locale_ದೂರ;
    g_Locale_ದೂರ = ಶೂನ್ಯಸೂಚಕ;
    gInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV Locale_ದೂರ::init_Locale_ದೂರ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_ದೂರ == ಶೂನ್ಯಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ LikelySubtags &likely = *LikelySubtags::getSingleton(error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಸ್ಥಿರಾಂಕ Locale_ದೂರ_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ = likely.get_ದೂರ_ದತ್ತಾಂಶ();
    ಆದರೆ (ದತ್ತಾಂಶ.ದೂರ_Trie_ಬೈಟುಗಳು == ಶೂನ್ಯಸೂಚಕ ||
            ದತ್ತಾಂಶ.region_ಗಮ್ಯ_Partitions == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ.partitions == ಶೂನ್ಯಸೂಚಕ ||
            // ok if no paradigms
            ದತ್ತಾಂಶ.distances == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    g_Locale_ದೂರ = ಹೊಸ Locale_ದೂರ(ದತ್ತಾಂಶ, likely);
    ಆದರೆ (g_Locale_ದೂರ == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE_ದೂರ, cleanup);
}

ಸ್ಥಿರಾಂಕ Locale_ದೂರ *Locale_ದೂರ::getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(gInitOnce, &Locale_ದೂರ::init_Locale_ದೂರ, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ g_Locale_ದೂರ;
}

Locale_ದೂರ::Locale_ದೂರ(ಸ್ಥಿರಾಂಕ Locale_ದೂರ_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ LikelySubtags &likely) :
        likelySubtags(likely),
        trie(ದತ್ತಾಂಶ.ದೂರ_Trie_ಬೈಟುಗಳು),
        region_ಗಮ್ಯ_Partitions_ಸೂಚ್ಯಂಕ(ದತ್ತಾಂಶ.region_ಗಮ್ಯ_Partitions), ವಿಭಜನೆ_Arrays(ದತ್ತಾಂಶ.partitions),
        paradigmLSRs(ದತ್ತಾಂಶ.paradigms), paradigm_LSRs_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ.paradigms_ಉದ್ದಳತೆ),
        default_Language_ದೂರ(ದತ್ತಾಂಶ.distances[IX_DEF_LANG_ದೂರ]),
        default_Script_ದೂರ(ದತ್ತಾಂಶ.distances[IX_DEF_SCRIPT_ದೂರ]),
        default_Region_ದೂರ(ದತ್ತಾಂಶ.distances[IX_DEF_REGION_ದೂರ]),
        min_Region_ದೂರ(ದತ್ತಾಂಶ.distances[IX_MIN_REGION_ದೂರ]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ಸ್ಥಿರಾಂಕ LSR *p_enGB = &enGB;
    int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ = get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(en, &p_enGB, 1,
            ಸರಿಸು_ದೂರ(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_ಪ್ರತಿ_Desired_Locale  = get_ದೂರ_Floor(ಸೂಚ್ಯಂಕ_And_ದೂರ);
}

int32_t Locale_ದೂರ::get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
        ಸ್ಥಿರಾಂಕ LSR &desired,
        ಸ್ಥಿರಾಂಕ LSR **supportedLSRs, int32_t supported_LSRs_ಉದ್ದಳತೆ,
        int32_t shifted_ಮಿತಿ,
        ULoc_ಹೊಂದಿಕೆ_Favor_Subtag favorSubtag, ULoc_ಹೊಂದಿಕೆ_Direction direction) ಸ್ಥಿರಾಂಕ {
    ಬೈಟುಗಳು_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_ದೂರ = trie_ಮುಂದಿನ(iter, desired.language, ಸುಳ್ಳು);
    uint64_t desLangState = des_Lang_ದೂರ >= 0 && supported_LSRs_ಉದ್ದಳತೆ > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t ಉತ್ತಮ_ಸೂಚ್ಯಂಕ = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t ಉತ್ತಮ_Likely_Info = -1;
    ಗಾಗಿ (int32_t sl_ಸೂಚ್ಯಂಕ = 0; sl_ಸೂಚ್ಯಂಕ < supported_LSRs_ಉದ್ದಳತೆ; ++sl_ಸೂಚ್ಯಂಕ) {
        ಸ್ಥಿರಾಂಕ LSR &supported = *supportedLSRs[sl_ಸೂಚ್ಯಂಕ];
        ತಾರ್ಕಿಕ ನಕ್ಷತ್ರ = ಸುಳ್ಳು;
        int32_t ದೂರ = des_Lang_ದೂರ;
        ಆದರೆ (ದೂರ >= 0) {
            U_ASSERT((ದೂರ & ದೂರ_IS_FINAL) == 0);
            ಆದರೆ (sl_ಸೂಚ್ಯಂಕ != 0) {
                iter.reset_ಗಮ್ಯ_State64(desLangState);
            }
            ದೂರ = trie_ಮುಂದಿನ(iter, supported.language, ನಿಜ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        ಆದರೆ (ದೂರ >= 0) {
            flags = ದೂರ & ದೂರ_IS_FINAL_OR_SKIP_SCRIPT;
            ದೂರ &= ~ದೂರ_IS_FINAL_OR_SKIP_SCRIPT;
        } ಇಲ್ಲದಿದ್ದರೆ {  // <*, *>
            ಆದರೆ (uprv_strcmp(desired.language, supported.language) == 0) {
                ದೂರ = 0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ದೂರ = default_Language_ದೂರ;
            }
            flags = 0;
            ನಕ್ಷತ್ರ = ನಿಜ;
        }
        U_ASSERT(0 <= ದೂರ && ದೂರ <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_ಮಿತಿ = (shifted_ಮಿತಿ + ದೂರ_FRACTION_ಮುಖವಾಡ) >> ದೂರ_ಸರಿಸು;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        ಆದರೆ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            ದೂರ >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        ಆದರೆ (ದೂರ > rounded_ಮಿತಿ) {
            ಮುಂದುವರಿ;
        }

        int32_t script_ದೂರ;
        ಆದರೆ (ನಕ್ಷತ್ರ || flags != 0) {
            ಆದರೆ (uprv_strcmp(desired.script, supported.script) == 0) {
                script_ದೂರ = 0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                script_ದೂರ = default_Script_ದೂರ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            script_ದೂರ = get_Des_Supp_Script_ದೂರ(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_ದೂರ & ದೂರ_IS_FINAL;
            script_ದೂರ &= ~ದೂರ_IS_FINAL;
        }
        ದೂರ += script_ದೂರ;
        ಆದರೆ (ದೂರ > rounded_ಮಿತಿ) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಕ್ಷತ್ರ || (flags & ದೂರ_IS_FINAL) != 0) {
            ದೂರ += default_Region_ದೂರ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            int32_t ಉಳಿದ_ಮಿತಿ = rounded_ಮಿತಿ - ದೂರ;
            ಆದರೆ (min_Region_ದೂರ > ಉಳಿದ_ಮಿತಿ) {
                ಮುಂದುವರಿ;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            ದೂರ += get_Region_Partitions_ದೂರ(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    ಉಳಿದ_ಮಿತಿ);
        }
        int32_t shifted_ದೂರ = ಸರಿಸು_ದೂರ(ದೂರ);
        ಆದರೆ (shifted_ದೂರ == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_ದೂರ |= (desired.flags ^ supported.flags);
            ಆದರೆ (shifted_ದೂರ < shifted_ಮಿತಿ) {
                ಆದರೆ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ಹೊಂದಿಕೆ(supported, desired, shifted_ಮಿತಿ, favorSubtag)) {
                    ಆದರೆ (shifted_ದೂರ == 0) {
                        ಹಿಂತಿರುಗಿ sl_ಸೂಚ್ಯಂಕ << ಸೂಚ್ಯಂಕ_ಸರಿಸು;
                    }
                    ಉತ್ತಮ_ಸೂಚ್ಯಂಕ = sl_ಸೂಚ್ಯಂಕ;
                    shifted_ಮಿತಿ = shifted_ದೂರ;
                    ಉತ್ತಮ_Likely_Info = -1;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (shifted_ದೂರ < shifted_ಮಿತಿ) {
                ಆದರೆ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ಹೊಂದಿಕೆ(supported, desired, shifted_ಮಿತಿ, favorSubtag)) {
                    ಉತ್ತಮ_ಸೂಚ್ಯಂಕ = sl_ಸೂಚ್ಯಂಕ;
                    shifted_ಮಿತಿ = shifted_ದೂರ;
                    ಉತ್ತಮ_Likely_Info = -1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shifted_ದೂರ == shifted_ಮಿತಿ && ಉತ್ತಮ_ಸೂಚ್ಯಂಕ >= 0) {
                ಆದರೆ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_ಹೊಂದಿಕೆ(supported, desired, shifted_ಮಿತಿ, favorSubtag)) {
                    ಉತ್ತಮ_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[ಉತ್ತಮ_ಸೂಚ್ಯಂಕ], ಉತ್ತಮ_Likely_Info);
                    ಆದರೆ ((ಉತ್ತಮ_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        ಉತ್ತಮ_ಸೂಚ್ಯಂಕ = sl_ಸೂಚ್ಯಂಕ;
                    }
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಉತ್ತಮ_ಸೂಚ್ಯಂಕ >= 0 ?
            (ಉತ್ತಮ_ಸೂಚ್ಯಂಕ << ಸೂಚ್ಯಂಕ_ಸರಿಸು) | shifted_ಮಿತಿ :
            ಸೂಚ್ಯಂಕ_NEG_1 | ಸರಿಸು_ದೂರ(ABOVE_ಮಿತಿ);
}

int32_t Locale_ದೂರ::get_Des_Supp_Script_ದೂರ(
        ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *desired, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t ದೂರ = trie_ಮುಂದಿನ(iter, desired, ಸುಳ್ಳು);
    ಆದರೆ (ದೂರ >= 0) {
        ದೂರ = trie_ಮುಂದಿನ(iter, supported, ನಿಜ);
    }
    ಆದರೆ (ದೂರ < 0) {
        UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = iter.reset_ಗಮ್ಯ_State64(ಆರಂಭ_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ));
        ಆದರೆ (uprv_strcmp(desired, supported) == 0) {
            ದೂರ = 0;  // same script
        } ಇಲ್ಲದಿದ್ದರೆ {
            ದೂರ = iter.get_ಮೌಲ್ಯ();
            U_ASSERT(ದೂರ >= 0);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
            ದೂರ |= ದೂರ_IS_FINAL;
        }
    }
    ಹಿಂತಿರುಗಿ ದೂರ;
}

int32_t Locale_ದೂರ::get_Region_Partitions_ದೂರ(
        ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *desiredPartitions, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supportedPartitions, int32_t ಮಿತಿ) {
    ಅಕ್ಷರ desired = *desiredPartitions++;
    ಅಕ್ಷರ supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    ತಾರ್ಕಿಕ supp_ಉದ್ದಳತೆ_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    ಆದರೆ (*desiredPartitions == 0 && !supp_ಉದ್ದಳತೆ_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = iter.next(uprv_inv_Char_ಗಮ್ಯ_Ascii(desired) | ಕೊನೆ_ದ_SUBTAG);
        ಆದರೆ (USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ)) {
            ಫಲಿತಾಂಶ = iter.next(uprv_inv_Char_ಗಮ್ಯ_Ascii(supported) | ಕೊನೆ_ದ_SUBTAG);
            ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
                ಹಿಂತಿರುಗಿ iter.get_ಮೌಲ್ಯ();
            }
        }
        ಹಿಂತಿರುಗಿ get_Fallback_Region_ದೂರ(iter, ಆರಂಭ_State);
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supported_ಆರಂಭ = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_ದೂರ = 0;
    // Fall back to * only once, not for each pair of partition strings.
    ತಾರ್ಕಿಕ ನಕ್ಷತ್ರ = ಸುಳ್ಳು;
    ಗಾಗಿ (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = iter.next(uprv_inv_Char_ಗಮ್ಯ_Ascii(desired) | ಕೊನೆ_ದ_SUBTAG);
        ಆದರೆ (USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ)) {
            uint64_t desState = supp_ಉದ್ದಳತೆ_Gt1 ? iter.getState64() : 0;
            ಗಾಗಿ (;;) {
                ಫಲಿತಾಂಶ = iter.next(uprv_inv_Char_ಗಮ್ಯ_Ascii(supported) | ಕೊನೆ_ದ_SUBTAG);
                int32_t d;
                ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
                    d = iter.get_ಮೌಲ್ಯ();
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಕ್ಷತ್ರ) {
                    d = 0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    d = get_Fallback_Region_ದೂರ(iter, ಆರಂಭ_State);
                    ನಕ್ಷತ್ರ = ನಿಜ;
                }
                ಆದರೆ (d > ಮಿತಿ) {
                    ಹಿಂತಿರುಗಿ d;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (region_ದೂರ < d) {
                    region_ದೂರ = d;
                }
                ಆದರೆ ((supported = *supportedPartitions++) != 0) {
                    iter.reset_ಗಮ್ಯ_State64(desState);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ನಕ್ಷತ್ರ) {
            int32_t d = get_Fallback_Region_ದೂರ(iter, ಆರಂಭ_State);
            ಆದರೆ (d > ಮಿತಿ) {
                ಹಿಂತಿರುಗಿ d;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (region_ದೂರ < d) {
                region_ದೂರ = d;
            }
            ನಕ್ಷತ್ರ = ನಿಜ;
        }
        ಆದರೆ ((desired = *desiredPartitions++) != 0) {
            iter.reset_ಗಮ್ಯ_State64(ಆರಂಭ_State);
            supportedPartitions = supported_ಆರಂಭ;
            supported = *supportedPartitions++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ region_ದೂರ;
}

int32_t Locale_ದೂರ::get_Fallback_Region_ದೂರ(ಬೈಟುಗಳು_Trie &iter, uint64_t ಆರಂಭ_State) {
#ಆದರೆ U_DEBUG
    UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ =
#ಆದರೆ_ಅಂತ್ಯ
    iter.reset_ಗಮ್ಯ_State64(ಆರಂಭ_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ));
    int32_t ದೂರ = iter.get_ಮೌಲ್ಯ();
    U_ASSERT(ದೂರ >= 0);
    ಹಿಂತಿರುಗಿ ದೂರ;
}

int32_t Locale_ದೂರ::trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ತಾರ್ಕಿಕ want_ಮೌಲ್ಯ) {
    uint8_t c;
    ಆದರೆ ((c = *s) == 0) {
        ಹಿಂತಿರುಗಿ -1;  // no empty subtags in the distance data
    }
    ಗಾಗಿ (;;) {
        c = uprv_inv_Char_ಗಮ್ಯ_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        ಆದರೆ (next != 0) {
            ಆದರೆ (!USTRINGTRIE_HAS_ಮುಂದಿನ(iter.next(c))) {
                ಹಿಂತಿರುಗಿ -1;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // last character of this subtag
            UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = iter.next(c | ಕೊನೆ_ದ_SUBTAG);
            ಆದರೆ (want_ಮೌಲ್ಯ) {
                ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
                    int32_t value = iter.get_ಮೌಲ್ಯ();
                    ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                        value |= ದೂರ_IS_FINAL;
                    }
                    ಹಿಂತಿರುಗಿ value;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ)) {
                    ಹಿಂತಿರುಗಿ 0;
                }
            }
            ಹಿಂತಿರುಗಿ -1;
        }
        c = next;
    }
}

ತಾರ್ಕಿಕ Locale_ದೂರ::isParadigmLSR(ಸ್ಥಿರಾಂಕ LSR &lsr) ಸ್ಥಿರಾಂಕ {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_ಉದ್ದಳತೆ <= 15);
    ಗಾಗಿ (int32_t i = 0; i < paradigm_LSRs_ಉದ್ದಳತೆ; ++i) {
        ಆದರೆ (lsr.is_Equivalent_ಗಮ್ಯ(paradigmLSRs[i])) { ಹಿಂತಿರುಗಿ ನಿಜ; }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_NAMESPACE_ಕೊನೆ
