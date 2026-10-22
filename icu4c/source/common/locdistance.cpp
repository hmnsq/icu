// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/localematcher.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "locdistance.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "umutex.h"

U_NAMESPACE_BEGIN

नामाकाश {

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
गणना {
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
    प्रत्यागम सच्च;
}

}  // namespace

शून्य U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    ध्रुव LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    ध्रुव LocaleDistanceData &data = likely.getDistanceData();
    जइ (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम;
    }
    gLocaleDistance = नूतन LocaleDistance(data, likely);
    जइ (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ध्रुव LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    प्रत्यागम gLocaleDistance;
}

LocaleDistance::LocaleDistance(ध्रुव LocaleDistanceData &data, ध्रुव LikelySubtags &likely) :
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
    ध्रुव LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        ध्रुव LSR &desired,
        ध्रुव LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) ध्रुव {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, मिच्छ);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    प्रति (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        ध्रुव LSR &supported = *supportedLSRs[slIndex];
        तार्किक star = मिच्छ;
        int32_t distance = desLangDistance;
        जइ (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            जइ (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, सच्च);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        जइ (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } अण्णहा {  // <*, *>
            जइ (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } अण्णहा {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = सच्च;
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
        जइ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        जइ (distance > roundedThreshold) {
            अनुवर्त;
        }

        int32_t scriptDistance;
        जइ (star || flags != 0) {
            जइ (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } अण्णहा {
                scriptDistance = defaultScriptDistance;
            }
        } अण्णहा {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        जइ (distance > roundedThreshold) {
            अनुवर्त;
        }

        जइ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } अण्णहा जइ (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } अण्णहा {
            int32_t remainingThreshold = roundedThreshold - distance;
            जइ (minRegionDistance > remainingThreshold) {
                अनुवर्त;
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
        जइ (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            जइ (shiftedDistance < shiftedThreshold) {
                जइ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    जइ (shiftedDistance == 0) {
                        प्रत्यागम slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } अण्णहा {
            जइ (shiftedDistance < shiftedThreshold) {
                जइ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } अण्णहा जइ (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                जइ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    जइ ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    प्रत्यागम bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, ध्रुव वर्ण *desired, ध्रुव वर्ण *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, मिच्छ);
    जइ (distance >= 0) {
        distance = trieNext(iter, supported, सच्च);
    }
    जइ (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_मानम्(result));
        जइ (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } अण्णहा {
            distance = iter.get_मानम्();
            U_ASSERT(distance >= 0);
        }
        जइ (result == USTRINGTRIE_FINAL_मानम्) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    प्रत्यागम distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ध्रुव वर्ण *desiredPartitions, ध्रुव वर्ण *supportedPartitions, int32_t threshold) {
    वर्ण desired = *desiredPartitions++;
    वर्ण supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    तार्किक suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    जइ (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        जइ (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            जइ (USTRINGTRIE_HAS_मानम्(result)) {
                प्रत्यागम iter.get_मानम्();
            }
        }
        प्रत्यागम getFallbackRegionDistance(iter, startState);
    }

    ध्रुव वर्ण *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    तार्किक star = मिच्छ;
    प्रति (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        जइ (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            प्रति (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                जइ (USTRINGTRIE_HAS_मानम्(result)) {
                    d = iter.get_मानम्();
                } अण्णहा जइ (star) {
                    d = 0;
                } अण्णहा {
                    d = getFallbackRegionDistance(iter, startState);
                    star = सच्च;
                }
                जइ (d > threshold) {
                    प्रत्यागम d;
                } अण्णहा जइ (regionDistance < d) {
                    regionDistance = d;
                }
                जइ ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } अण्णहा {
                    भङ्ग;
                }
            }
        } अण्णहा जइ (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            जइ (d > threshold) {
                प्रत्यागम d;
            } अण्णहा जइ (regionDistance < d) {
                regionDistance = d;
            }
            star = सच्च;
        }
        जइ ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } अण्णहा {
            भङ्ग;
        }
    }
    प्रत्यागम regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#जइ U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_मानम्(result));
    int32_t distance = iter.get_मानम्();
    U_ASSERT(distance >= 0);
    प्रत्यागम distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, ध्रुव वर्ण *s, तार्किक want_मानम्) {
    uint8_t c;
    जइ ((c = *s) == 0) {
        प्रत्यागम -1;  // no empty subtags in the distance data
    }
    प्रति (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        जइ (next != 0) {
            जइ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                प्रत्यागम -1;
            }
        } अण्णहा {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            जइ (want_मानम्) {
                जइ (USTRINGTRIE_HAS_मानम्(result)) {
                    int32_t value = iter.get_मानम्();
                    जइ (result == USTRINGTRIE_FINAL_मानम्) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    प्रत्यागम value;
                }
            } अण्णहा {
                जइ (USTRINGTRIE_HAS_NEXT(result)) {
                    प्रत्यागम 0;
                }
            }
            प्रत्यागम -1;
        }
        c = next;
    }
}

तार्किक LocaleDistance::isParadigmLSR(ध्रुव LSR &lsr) ध्रुव {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    प्रति (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        जइ (lsr.isEquivalentTo(paradigmLSRs[i])) { प्रत्यागम सच्च; }
    }
    प्रत्यागम मिच्छ;
}

U_NAMESPACE_END
