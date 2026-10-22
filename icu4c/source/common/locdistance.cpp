// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/localematcher.h"
#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "cstring.h"
#شامل "locdistance.h"
#شامل "loclikelysubtags.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uinvchar.h"
#شامل "umutex.h"

U_NAMESPACE_BEGIN

فضاینام {

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
شمارش {
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
    بازگشت درست;
}

}  // namespace

تهی U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    ثابت LocaleDistanceData &data = likely.getDistanceData();
    اگر (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        بازگشت;
    }
    gLocaleDistance = جدید LocaleDistance(data, likely);
    اگر (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ثابت LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    بازگشت gLocaleDistance;
}

LocaleDistance::LocaleDistance(ثابت LocaleDistanceData &data, ثابت LikelySubtags &likely) :
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
    ثابت LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) ثابت {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, نادرست);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    برای (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        ثابت LSR &supported = *supportedLSRs[slIndex];
        منطقی star = نادرست;
        int32_t distance = desLangDistance;
        اگر (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            اگر (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, درست);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        اگر (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } وگرنه {  // <*, *>
            اگر (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } وگرنه {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = درست;
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
        اگر (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        اگر (distance > roundedThreshold) {
            ادامه;
        }

        int32_t scriptDistance;
        اگر (star || flags != 0) {
            اگر (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } وگرنه {
                scriptDistance = defaultScriptDistance;
            }
        } وگرنه {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        اگر (distance > roundedThreshold) {
            ادامه;
        }

        اگر (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } وگرنه اگر (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } وگرنه {
            int32_t remainingThreshold = roundedThreshold - distance;
            اگر (minRegionDistance > remainingThreshold) {
                ادامه;
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
        اگر (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            اگر (shiftedDistance < shiftedThreshold) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    اگر (shiftedDistance == 0) {
                        بازگشت slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } وگرنه {
            اگر (shiftedDistance < shiftedThreshold) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } وگرنه اگر (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    اگر ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    بازگشت bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, ثابت نویسه *desired, ثابت نویسه *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, نادرست);
    اگر (distance >= 0) {
        distance = trieNext(iter, supported, درست);
    }
    اگر (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_مقدار(result));
        اگر (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } وگرنه {
            distance = iter.get_مقدار();
            U_ASSERT(distance >= 0);
        }
        اگر (result == USTRINGTRIE_FINAL_مقدار) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    بازگشت distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ثابت نویسه *desiredPartitions, ثابت نویسه *supportedPartitions, int32_t threshold) {
    نویسه desired = *desiredPartitions++;
    نویسه supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقی suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    اگر (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        اگر (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            اگر (USTRINGTRIE_HAS_مقدار(result)) {
                بازگشت iter.get_مقدار();
            }
        }
        بازگشت getFallbackRegionDistance(iter, startState);
    }

    ثابت نویسه *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقی star = نادرست;
    برای (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        اگر (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            برای (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                اگر (USTRINGTRIE_HAS_مقدار(result)) {
                    d = iter.get_مقدار();
                } وگرنه اگر (star) {
                    d = 0;
                } وگرنه {
                    d = getFallbackRegionDistance(iter, startState);
                    star = درست;
                }
                اگر (d > threshold) {
                    بازگشت d;
                } وگرنه اگر (regionDistance < d) {
                    regionDistance = d;
                }
                اگر ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } وگرنه {
                    بشکن;
                }
            }
        } وگرنه اگر (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            اگر (d > threshold) {
                بازگشت d;
            } وگرنه اگر (regionDistance < d) {
                regionDistance = d;
            }
            star = درست;
        }
        اگر ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } وگرنه {
            بشکن;
        }
    }
    بازگشت regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#اگر U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_مقدار(result));
    int32_t distance = iter.get_مقدار();
    U_ASSERT(distance >= 0);
    بازگشت distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, ثابت نویسه *s, منطقی want_مقدار) {
    uint8_t c;
    اگر ((c = *s) == 0) {
        بازگشت -1;  // no empty subtags in the distance data
    }
    برای (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        اگر (next != 0) {
            اگر (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                بازگشت -1;
            }
        } وگرنه {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            اگر (want_مقدار) {
                اگر (USTRINGTRIE_HAS_مقدار(result)) {
                    int32_t value = iter.get_مقدار();
                    اگر (result == USTRINGTRIE_FINAL_مقدار) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    بازگشت value;
                }
            } وگرنه {
                اگر (USTRINGTRIE_HAS_NEXT(result)) {
                    بازگشت 0;
                }
            }
            بازگشت -1;
        }
        c = next;
    }
}

منطقی LocaleDistance::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    برای (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        اگر (lsr.isEquivalentTo(paradigmLSRs[i])) { بازگشت درست; }
    }
    بازگشت نادرست;
}

U_NAMESPACE_END
