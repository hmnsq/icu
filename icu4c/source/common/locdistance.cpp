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
constexpr int32_t END_OF_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t DISTANCE_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t DISTANCE_IS_FINAL = 0x100;
constexpr int32_t DISTANCE_IS_FINAL_OR_SKIP_SCRIPT = DISTANCE_IS_FINAL | DISTANCE_SKIP_SCRIPT;

constexpr int32_t ABOVE_THRESHOLD = 100;

// Indexes into array of distances.
ಗಣನಾ {
    IX_DEF_LANG_DISTANCE,
    IX_DEF_SCRIPT_DISTANCE,
    IX_DEF_REGION_DISTANCE,
    IX_MIN_REGION_DISTANCE,
    IX_LIMIT
};

LocaleDistance *gLocaleDistance = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    delete gLocaleDistance;
    gLocaleDistance = nullptr;
    gInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    ಧ್ರುವ LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಧ್ರುವ LocaleDistanceData &data = likely.getDistanceData();
    ಯದಿ (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    gLocaleDistance = ನೂತನ LocaleDistance(data, likely);
    ಯದಿ (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ಧ್ರುವ LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    ಪ್ರತ್ಯಾಗಮ gLocaleDistance;
}

LocaleDistance::LocaleDistance(ಧ್ರುವ LocaleDistanceData &data, ಧ್ರುವ LikelySubtags &likely) :
        likelySubtags(likely),
        trie(data.distanceTrieBytes),
        regionToPartitionsIndex(data.regionToPartitions), partitionArrays(data.partitions),
        paradigmLSRs(data.paradigms), paradigmLSRsLength(data.paradigmsLength),
        defaultLanguageDistance(data.distances[IX_DEF_LANG_DISTANCE]),
        defaultScriptDistance(data.distances[IX_DEF_SCRIPT_DISTANCE]),
        defaultRegionDistance(data.distances[IX_DEF_REGION_DISTANCE]),
        minRegionDistance(data.distances[IX_MIN_REGION_DISTANCE]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ಧ್ರುವ LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        ಧ್ರುವ LSR &desired,
        ಧ್ರುವ LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) ಧ್ರುವ {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, ಅಸತ್ಯ);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    ಪ್ರತಿ (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        ಧ್ರುವ LSR &supported = *supportedLSRs[slIndex];
        ತಾರ್ಕಿಕ star = ಅಸತ್ಯ;
        int32_t distance = desLangDistance;
        ಯದಿ (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            ಯದಿ (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, ಸತ್ಯ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        ಯದಿ (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } ಅನ್ಯಥಾ {  // <*, *>
            ಯದಿ (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } ಅನ್ಯಥಾ {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = ಸತ್ಯ;
        }
        U_ASSERT(0 <= distance && distance <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t roundedThreshold = (shiftedThreshold + DISTANCE_FRACTION_MASK) >> DISTANCE_SHIFT;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        ಯದಿ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        ಯದಿ (distance > roundedThreshold) {
            ಅನುವರ್ತ;
        }

        int32_t scriptDistance;
        ಯದಿ (star || flags != 0) {
            ಯದಿ (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } ಅನ್ಯಥಾ {
                scriptDistance = defaultScriptDistance;
            }
        } ಅನ್ಯಥಾ {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        ಯದಿ (distance > roundedThreshold) {
            ಅನುವರ್ತ;
        }

        ಯದಿ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } ಅನ್ಯಥಾ ಯದಿ (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } ಅನ್ಯಥಾ {
            int32_t remainingThreshold = roundedThreshold - distance;
            ಯದಿ (minRegionDistance > remainingThreshold) {
                ಅನುವರ್ತ;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            distance += getRegionPartitionsDistance(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    remainingThreshold);
        }
        int32_t shiftedDistance = shiftDistance(distance);
        ಯದಿ (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            ಯದಿ (shiftedDistance < shiftedThreshold) {
                ಯದಿ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    ಯದಿ (shiftedDistance == 0) {
                        ಪ್ರತ್ಯಾಗಮ slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ (shiftedDistance < shiftedThreshold) {
                ಯದಿ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } ಅನ್ಯಥಾ ಯದಿ (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                ಯದಿ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    ಯದಿ ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, ಧ್ರುವ ವರ್ಣ *desired, ಧ್ರುವ ವರ್ಣ *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, ಅಸತ್ಯ);
    ಯದಿ (distance >= 0) {
        distance = trieNext(iter, supported, ಸತ್ಯ);
    }
    ಯದಿ (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(result));
        ಯದಿ (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } ಅನ್ಯಥಾ {
            distance = iter.get_ಮೌಲ್ಯ();
            U_ASSERT(distance >= 0);
        }
        ಯದಿ (result == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    ಪ್ರತ್ಯಾಗಮ distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ಧ್ರುವ ವರ್ಣ *desiredPartitions, ಧ್ರುವ ವರ್ಣ *supportedPartitions, int32_t threshold) {
    ವರ್ಣ desired = *desiredPartitions++;
    ವರ್ಣ supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    ತಾರ್ಕಿಕ suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    ಯದಿ (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        ಯದಿ (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
                ಪ್ರತ್ಯಾಗಮ iter.get_ಮೌಲ್ಯ();
            }
        }
        ಪ್ರತ್ಯಾಗಮ getFallbackRegionDistance(iter, startState);
    }

    ಧ್ರುವ ವರ್ಣ *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    ತಾರ್ಕಿಕ star = ಅಸತ್ಯ;
    ಪ್ರತಿ (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        ಯದಿ (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            ಪ್ರತಿ (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
                    d = iter.get_ಮೌಲ್ಯ();
                } ಅನ್ಯಥಾ ಯದಿ (star) {
                    d = 0;
                } ಅನ್ಯಥಾ {
                    d = getFallbackRegionDistance(iter, startState);
                    star = ಸತ್ಯ;
                }
                ಯದಿ (d > threshold) {
                    ಪ್ರತ್ಯಾಗಮ d;
                } ಅನ್ಯಥಾ ಯದಿ (regionDistance < d) {
                    regionDistance = d;
                }
                ಯದಿ ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            ಯದಿ (d > threshold) {
                ಪ್ರತ್ಯಾಗಮ d;
            } ಅನ್ಯಥಾ ಯದಿ (regionDistance < d) {
                regionDistance = d;
            }
            star = ಸತ್ಯ;
        }
        ಯದಿ ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } ಅನ್ಯಥಾ {
            ಭಙ್ಗ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#ಯದಿ U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(result));
    int32_t distance = iter.get_ಮೌಲ್ಯ();
    U_ASSERT(distance >= 0);
    ಪ್ರತ್ಯಾಗಮ distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, ಧ್ರುವ ವರ್ಣ *s, ತಾರ್ಕಿಕ want_ಮೌಲ್ಯ) {
    uint8_t c;
    ಯದಿ ((c = *s) == 0) {
        ಪ್ರತ್ಯಾಗಮ -1;  // no empty subtags in the distance data
    }
    ಪ್ರತಿ (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        ಯದಿ (next != 0) {
            ಯದಿ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                ಪ್ರತ್ಯಾಗಮ -1;
            }
        } ಅನ್ಯಥಾ {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            ಯದಿ (want_ಮೌಲ್ಯ) {
                ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
                    int32_t value = iter.get_ಮೌಲ್ಯ();
                    ಯದಿ (result == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    ಪ್ರತ್ಯಾಗಮ value;
                }
            } ಅನ್ಯಥಾ {
                ಯದಿ (USTRINGTRIE_HAS_NEXT(result)) {
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
            }
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        c = next;
    }
}

ತಾರ್ಕಿಕ LocaleDistance::isParadigmLSR(ಧ್ರುವ LSR &lsr) ಧ್ರುವ {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    ಪ್ರತಿ (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        ಯದಿ (lsr.isEquivalentTo(paradigmLSRs[i])) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_NAMESPACE_END
