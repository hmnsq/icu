// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localematcher.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "locdistance.h"
#ଅନ୍ତର୍ଭାଵ "loclikelysubtags.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

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
ଗଣନା {
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
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

}  // namespace

ଶୂନ୍ୟ U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    ଅଚଳ LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଅଚଳ LocaleDistanceData &data = likely.getDistanceData();
    ଯଦି (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        ଫେରନ୍ତୁ;
    }
    gLocaleDistance = ନୂତନ LocaleDistance(data, likely);
    ଯଦି (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ଅଚଳ LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    ଫେରନ୍ତୁ gLocaleDistance;
}

LocaleDistance::LocaleDistance(ଅଚଳ LocaleDistanceData &data, ଅଚଳ LikelySubtags &likely) :
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
    ଅଚଳ LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        ଅଚଳ LSR &desired,
        ଅଚଳ LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) ଅଚଳ {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, ମିଥ୍ୟା);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    ପାଇଁ (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        ଅଚଳ LSR &supported = *supportedLSRs[slIndex];
        ତାର୍କିକ star = ମିଥ୍ୟା;
        int32_t distance = desLangDistance;
        ଯଦି (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            ଯଦି (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, ସତ୍ୟ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        ଯଦି (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } ଅନ୍ୟଥା {  // <*, *>
            ଯଦି (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } ଅନ୍ୟଥା {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = ସତ୍ୟ;
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
        ଯଦି (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        ଯଦି (distance > roundedThreshold) {
            ଜାରି;
        }

        int32_t scriptDistance;
        ଯଦି (star || flags != 0) {
            ଯଦି (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } ଅନ୍ୟଥା {
                scriptDistance = defaultScriptDistance;
            }
        } ଅନ୍ୟଥା {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        ଯଦି (distance > roundedThreshold) {
            ଜାରି;
        }

        ଯଦି (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } ଅନ୍ୟଥା ଯଦି (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } ଅନ୍ୟଥା {
            int32_t remainingThreshold = roundedThreshold - distance;
            ଯଦି (minRegionDistance > remainingThreshold) {
                ଜାରି;
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
        ଯଦି (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            ଯଦି (shiftedDistance < shiftedThreshold) {
                ଯଦି (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    ଯଦି (shiftedDistance == 0) {
                        ଫେରନ୍ତୁ slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } ଅନ୍ୟଥା {
            ଯଦି (shiftedDistance < shiftedThreshold) {
                ଯଦି (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } ଅନ୍ୟଥା ଯଦି (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                ଯଦି (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    ଯଦି ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    ଫେରନ୍ତୁ bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, ଅଚଳ ଅକ୍ଷର *desired, ଅଚଳ ଅକ୍ଷର *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, ମିଥ୍ୟା);
    ଯଦି (distance >= 0) {
        distance = trieNext(iter, supported, ସତ୍ୟ);
    }
    ଯଦି (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_ମୂଲ୍ୟ(result));
        ଯଦି (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } ଅନ୍ୟଥା {
            distance = iter.get_ମୂଲ୍ୟ();
            U_ASSERT(distance >= 0);
        }
        ଯଦି (result == USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    ଫେରନ୍ତୁ distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ଅଚଳ ଅକ୍ଷର *desiredPartitions, ଅଚଳ ଅକ୍ଷର *supportedPartitions, int32_t threshold) {
    ଅକ୍ଷର desired = *desiredPartitions++;
    ଅକ୍ଷର supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    ତାର୍କିକ suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    ଯଦି (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        ଯଦି (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
                ଫେରନ୍ତୁ iter.get_ମୂଲ୍ୟ();
            }
        }
        ଫେରନ୍ତୁ getFallbackRegionDistance(iter, startState);
    }

    ଅଚଳ ଅକ୍ଷର *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    ତାର୍କିକ star = ମିଥ୍ୟା;
    ପାଇଁ (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        ଯଦି (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            ପାଇଁ (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
                    d = iter.get_ମୂଲ୍ୟ();
                } ଅନ୍ୟଥା ଯଦି (star) {
                    d = 0;
                } ଅନ୍ୟଥା {
                    d = getFallbackRegionDistance(iter, startState);
                    star = ସତ୍ୟ;
                }
                ଯଦି (d > threshold) {
                    ଫେରନ୍ତୁ d;
                } ଅନ୍ୟଥା ଯଦି (regionDistance < d) {
                    regionDistance = d;
                }
                ଯଦି ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } ଅନ୍ୟଥା {
                    ଭାଙ୍ଗ;
                }
            }
        } ଅନ୍ୟଥା ଯଦି (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            ଯଦି (d > threshold) {
                ଫେରନ୍ତୁ d;
            } ଅନ୍ୟଥା ଯଦି (regionDistance < d) {
                regionDistance = d;
            }
            star = ସତ୍ୟ;
        }
        ଯଦି ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } ଅନ୍ୟଥା {
            ଭାଙ୍ଗ;
        }
    }
    ଫେରନ୍ତୁ regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#ଯଦି U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_ମୂଲ୍ୟ(result));
    int32_t distance = iter.get_ମୂଲ୍ୟ();
    U_ASSERT(distance >= 0);
    ଫେରନ୍ତୁ distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, ଅଚଳ ଅକ୍ଷର *s, ତାର୍କିକ want_ମୂଲ୍ୟ) {
    uint8_t c;
    ଯଦି ((c = *s) == 0) {
        ଫେରନ୍ତୁ -1;  // no empty subtags in the distance data
    }
    ପାଇଁ (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        ଯଦି (next != 0) {
            ଯଦି (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                ଫେରନ୍ତୁ -1;
            }
        } ଅନ୍ୟଥା {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            ଯଦି (want_ମୂଲ୍ୟ) {
                ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
                    int32_t value = iter.get_ମୂଲ୍ୟ();
                    ଯଦି (result == USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    ଫେରନ୍ତୁ value;
                }
            } ଅନ୍ୟଥା {
                ଯଦି (USTRINGTRIE_HAS_NEXT(result)) {
                    ଫେରନ୍ତୁ 0;
                }
            }
            ଫେରନ୍ତୁ -1;
        }
        c = next;
    }
}

ତାର୍କିକ LocaleDistance::isParadigmLSR(ଅଚଳ LSR &lsr) ଅଚଳ {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    ପାଇଁ (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        ଯଦି (lsr.isEquivalentTo(paradigmLSRs[i])) { ଫେରନ୍ତୁ ସତ୍ୟ; }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_NAMESPACE_END
