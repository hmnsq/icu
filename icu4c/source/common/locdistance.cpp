// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/localematcher.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "locdistance.h"
#அந்தர்பாவ "loclikelysubtags.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "umutex.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

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
எண்ணிக்கை {
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
    திரும்பு உண்மை;
}

}  // namespace

வெறுமை U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    மாறிலி LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    மாறிலி LocaleDistanceData &data = likely.getDistanceData();
    எனில் (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        திரும்பு;
    }
    gLocaleDistance = புதிய LocaleDistance(data, likely);
    எனில் (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

மாறிலி LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    திரும்பு gLocaleDistance;
}

LocaleDistance::LocaleDistance(மாறிலி LocaleDistanceData &data, மாறிலி LikelySubtags &likely) :
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
    மாறிலி LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        மாறிலி LSR &desired,
        மாறிலி LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) மாறிலி {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, பொய்);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    க்கு (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        மாறிலி LSR &supported = *supportedLSRs[slIndex];
        தருக்க star = பொய்;
        int32_t distance = desLangDistance;
        எனில் (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            எனில் (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, உண்மை);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        எனில் (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } இல்லையெனில் {  // <*, *>
            எனில் (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } இல்லையெனில் {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = உண்மை;
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
        எனில் (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        எனில் (distance > roundedThreshold) {
            தொடர்;
        }

        int32_t scriptDistance;
        எனில் (star || flags != 0) {
            எனில் (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } இல்லையெனில் {
                scriptDistance = defaultScriptDistance;
            }
        } இல்லையெனில் {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        எனில் (distance > roundedThreshold) {
            தொடர்;
        }

        எனில் (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } இல்லையெனில் எனில் (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } இல்லையெனில் {
            int32_t remainingThreshold = roundedThreshold - distance;
            எனில் (minRegionDistance > remainingThreshold) {
                தொடர்;
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
        எனில் (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            எனில் (shiftedDistance < shiftedThreshold) {
                எனில் (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    எனில் (shiftedDistance == 0) {
                        திரும்பு slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } இல்லையெனில் {
            எனில் (shiftedDistance < shiftedThreshold) {
                எனில் (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } இல்லையெனில் எனில் (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                எனில் (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    எனில் ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    திரும்பு bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, மாறிலி எழுத்து *desired, மாறிலி எழுத்து *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, பொய்);
    எனில் (distance >= 0) {
        distance = trieNext(iter, supported, உண்மை);
    }
    எனில் (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_மதிப்பு(result));
        எனில் (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } இல்லையெனில் {
            distance = iter.get_மதிப்பு();
            U_ASSERT(distance >= 0);
        }
        எனில் (result == USTRINGTRIE_FINAL_மதிப்பு) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    திரும்பு distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        மாறிலி எழுத்து *desiredPartitions, மாறிலி எழுத்து *supportedPartitions, int32_t threshold) {
    எழுத்து desired = *desiredPartitions++;
    எழுத்து supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    தருக்க suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    எனில் (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        எனில் (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
                திரும்பு iter.get_மதிப்பு();
            }
        }
        திரும்பு getFallbackRegionDistance(iter, startState);
    }

    மாறிலி எழுத்து *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    தருக்க star = பொய்;
    க்கு (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        எனில் (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            க்கு (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
                    d = iter.get_மதிப்பு();
                } இல்லையெனில் எனில் (star) {
                    d = 0;
                } இல்லையெனில் {
                    d = getFallbackRegionDistance(iter, startState);
                    star = உண்மை;
                }
                எனில் (d > threshold) {
                    திரும்பு d;
                } இல்லையெனில் எனில் (regionDistance < d) {
                    regionDistance = d;
                }
                எனில் ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } இல்லையெனில் {
                    நிறுத்து;
                }
            }
        } இல்லையெனில் எனில் (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            எனில் (d > threshold) {
                திரும்பு d;
            } இல்லையெனில் எனில் (regionDistance < d) {
                regionDistance = d;
            }
            star = உண்மை;
        }
        எனில் ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } இல்லையெனில் {
            நிறுத்து;
        }
    }
    திரும்பு regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#எனில் U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_மதிப்பு(result));
    int32_t distance = iter.get_மதிப்பு();
    U_ASSERT(distance >= 0);
    திரும்பு distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, மாறிலி எழுத்து *s, தருக்க want_மதிப்பு) {
    uint8_t c;
    எனில் ((c = *s) == 0) {
        திரும்பு -1;  // no empty subtags in the distance data
    }
    க்கு (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        எனில் (next != 0) {
            எனில் (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                திரும்பு -1;
            }
        } இல்லையெனில் {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            எனில் (want_மதிப்பு) {
                எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
                    int32_t value = iter.get_மதிப்பு();
                    எனில் (result == USTRINGTRIE_FINAL_மதிப்பு) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    திரும்பு value;
                }
            } இல்லையெனில் {
                எனில் (USTRINGTRIE_HAS_NEXT(result)) {
                    திரும்பு 0;
                }
            }
            திரும்பு -1;
        }
        c = next;
    }
}

தருக்க LocaleDistance::isParadigmLSR(மாறிலி LSR &lsr) மாறிலி {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    க்கு (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        எனில் (lsr.isEquivalentTo(paradigmLSRs[i])) { திரும்பு உண்மை; }
    }
    திரும்பு பொய்;
}

U_NAMESPACE_END
