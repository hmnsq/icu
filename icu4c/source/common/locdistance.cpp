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
    फर्काऊ सत्य;
}

}  // namespace

शून्य U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    अचर LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    अचर LocaleDistanceData &data = likely.getDistanceData();
    यदि (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        फर्काऊ;
    }
    gLocaleDistance = नूतन LocaleDistance(data, likely);
    यदि (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

अचर LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    फर्काऊ gLocaleDistance;
}

LocaleDistance::LocaleDistance(अचर LocaleDistanceData &data, अचर LikelySubtags &likely) :
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
    अचर LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        अचर LSR &desired,
        अचर LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) अचर {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, असत्य);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    लागि (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        अचर LSR &supported = *supportedLSRs[slIndex];
        तार्किक star = असत्य;
        int32_t distance = desLangDistance;
        यदि (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            यदि (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, सत्य);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        यदि (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } अन्यथा {  // <*, *>
            यदि (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } अन्यथा {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = सत्य;
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
        यदि (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        यदि (distance > roundedThreshold) {
            जारी;
        }

        int32_t scriptDistance;
        यदि (star || flags != 0) {
            यदि (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } अन्यथा {
                scriptDistance = defaultScriptDistance;
            }
        } अन्यथा {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        यदि (distance > roundedThreshold) {
            जारी;
        }

        यदि (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } अन्यथा यदि (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } अन्यथा {
            int32_t remainingThreshold = roundedThreshold - distance;
            यदि (minRegionDistance > remainingThreshold) {
                जारी;
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
        यदि (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            यदि (shiftedDistance < shiftedThreshold) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    यदि (shiftedDistance == 0) {
                        फर्काऊ slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } अन्यथा {
            यदि (shiftedDistance < shiftedThreshold) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } अन्यथा यदि (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    यदि ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    फर्काऊ bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, अचर वर्ण *desired, अचर वर्ण *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, असत्य);
    यदि (distance >= 0) {
        distance = trieNext(iter, supported, सत्य);
    }
    यदि (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_मान(result));
        यदि (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } अन्यथा {
            distance = iter.get_मान();
            U_ASSERT(distance >= 0);
        }
        यदि (result == USTRINGTRIE_FINAL_मान) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    फर्काऊ distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        अचर वर्ण *desiredPartitions, अचर वर्ण *supportedPartitions, int32_t threshold) {
    वर्ण desired = *desiredPartitions++;
    वर्ण supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    तार्किक suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    यदि (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        यदि (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            यदि (USTRINGTRIE_HAS_मान(result)) {
                फर्काऊ iter.get_मान();
            }
        }
        फर्काऊ getFallbackRegionDistance(iter, startState);
    }

    अचर वर्ण *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    तार्किक star = असत्य;
    लागि (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        यदि (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            लागि (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                यदि (USTRINGTRIE_HAS_मान(result)) {
                    d = iter.get_मान();
                } अन्यथा यदि (star) {
                    d = 0;
                } अन्यथा {
                    d = getFallbackRegionDistance(iter, startState);
                    star = सत्य;
                }
                यदि (d > threshold) {
                    फर्काऊ d;
                } अन्यथा यदि (regionDistance < d) {
                    regionDistance = d;
                }
                यदि ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } अन्यथा {
                    रोक;
                }
            }
        } अन्यथा यदि (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            यदि (d > threshold) {
                फर्काऊ d;
            } अन्यथा यदि (regionDistance < d) {
                regionDistance = d;
            }
            star = सत्य;
        }
        यदि ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } अन्यथा {
            रोक;
        }
    }
    फर्काऊ regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#यदि U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_मान(result));
    int32_t distance = iter.get_मान();
    U_ASSERT(distance >= 0);
    फर्काऊ distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, अचर वर्ण *s, तार्किक want_मान) {
    uint8_t c;
    यदि ((c = *s) == 0) {
        फर्काऊ -1;  // no empty subtags in the distance data
    }
    लागि (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        यदि (next != 0) {
            यदि (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                फर्काऊ -1;
            }
        } अन्यथा {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            यदि (want_मान) {
                यदि (USTRINGTRIE_HAS_मान(result)) {
                    int32_t value = iter.get_मान();
                    यदि (result == USTRINGTRIE_FINAL_मान) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    फर्काऊ value;
                }
            } अन्यथा {
                यदि (USTRINGTRIE_HAS_NEXT(result)) {
                    फर्काऊ 0;
                }
            }
            फर्काऊ -1;
        }
        c = next;
    }
}

तार्किक LocaleDistance::isParadigmLSR(अचर LSR &lsr) अचर {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    लागि (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        यदि (lsr.isEquivalentTo(paradigmLSRs[i])) { फर्काऊ सत्य; }
    }
    फर्काऊ असत्य;
}

U_NAMESPACE_END
