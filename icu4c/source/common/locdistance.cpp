// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/localematcher.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "locdistance.h"
#অন্তর্ভাব "loclikelysubtags.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "umutex.h"

U_NAMESPACE_BEGIN

নামাকাশ {

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
গণনা {
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
    প্রত্যাগম সত্য;
}

}  // namespace

শূন্য U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    ধ্রুব LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    ধ্রুব LocaleDistanceData &data = likely.getDistanceData();
    যদি (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        প্রত্যাগম;
    }
    gLocaleDistance = নূতন LocaleDistance(data, likely);
    যদি (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ধ্রুব LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    প্রত্যাগম gLocaleDistance;
}

LocaleDistance::LocaleDistance(ধ্রুব LocaleDistanceData &data, ধ্রুব LikelySubtags &likely) :
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
    ধ্রুব LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        ধ্রুব LSR &desired,
        ধ্রুব LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) ধ্রুব {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, অসত্য);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    প্রতি (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        ধ্রুব LSR &supported = *supportedLSRs[slIndex];
        তার্কিক star = অসত্য;
        int32_t distance = desLangDistance;
        যদি (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            যদি (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, সত্য);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        যদি (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } অন্যথা {  // <*, *>
            যদি (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } অন্যথা {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = সত্য;
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
        যদি (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        যদি (distance > roundedThreshold) {
            অনুবর্ত;
        }

        int32_t scriptDistance;
        যদি (star || flags != 0) {
            যদি (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } অন্যথা {
                scriptDistance = defaultScriptDistance;
            }
        } অন্যথা {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        যদি (distance > roundedThreshold) {
            অনুবর্ত;
        }

        যদি (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } অন্যথা যদি (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } অন্যথা {
            int32_t remainingThreshold = roundedThreshold - distance;
            যদি (minRegionDistance > remainingThreshold) {
                অনুবর্ত;
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
        যদি (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            যদি (shiftedDistance < shiftedThreshold) {
                যদি (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    যদি (shiftedDistance == 0) {
                        প্রত্যাগম slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } অন্যথা {
            যদি (shiftedDistance < shiftedThreshold) {
                যদি (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } অন্যথা যদি (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                যদি (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    যদি ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    প্রত্যাগম bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, ধ্রুব বর্ণ *desired, ধ্রুব বর্ণ *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, অসত্য);
    যদি (distance >= 0) {
        distance = trieNext(iter, supported, সত্য);
    }
    যদি (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_মান(result));
        যদি (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } অন্যথা {
            distance = iter.get_মান();
            U_ASSERT(distance >= 0);
        }
        যদি (result == USTRINGTRIE_FINAL_মান) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    প্রত্যাগম distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        ধ্রুব বর্ণ *desiredPartitions, ধ্রুব বর্ণ *supportedPartitions, int32_t threshold) {
    বর্ণ desired = *desiredPartitions++;
    বর্ণ supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    তার্কিক suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    যদি (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        যদি (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            যদি (USTRINGTRIE_HAS_মান(result)) {
                প্রত্যাগম iter.get_মান();
            }
        }
        প্রত্যাগম getFallbackRegionDistance(iter, startState);
    }

    ধ্রুব বর্ণ *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    তার্কিক star = অসত্য;
    প্রতি (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        যদি (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            প্রতি (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                যদি (USTRINGTRIE_HAS_মান(result)) {
                    d = iter.get_মান();
                } অন্যথা যদি (star) {
                    d = 0;
                } অন্যথা {
                    d = getFallbackRegionDistance(iter, startState);
                    star = সত্য;
                }
                যদি (d > threshold) {
                    প্রত্যাগম d;
                } অন্যথা যদি (regionDistance < d) {
                    regionDistance = d;
                }
                যদি ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } অন্যথা {
                    ভঙ্গ;
                }
            }
        } অন্যথা যদি (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            যদি (d > threshold) {
                প্রত্যাগম d;
            } অন্যথা যদি (regionDistance < d) {
                regionDistance = d;
            }
            star = সত্য;
        }
        যদি ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } অন্যথা {
            ভঙ্গ;
        }
    }
    প্রত্যাগম regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#যদি U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_মান(result));
    int32_t distance = iter.get_মান();
    U_ASSERT(distance >= 0);
    প্রত্যাগম distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, ধ্রুব বর্ণ *s, তার্কিক want_মান) {
    uint8_t c;
    যদি ((c = *s) == 0) {
        প্রত্যাগম -1;  // no empty subtags in the distance data
    }
    প্রতি (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        যদি (next != 0) {
            যদি (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                প্রত্যাগম -1;
            }
        } অন্যথা {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            যদি (want_মান) {
                যদি (USTRINGTRIE_HAS_মান(result)) {
                    int32_t value = iter.get_মান();
                    যদি (result == USTRINGTRIE_FINAL_মান) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    প্রত্যাগম value;
                }
            } অন্যথা {
                যদি (USTRINGTRIE_HAS_NEXT(result)) {
                    প্রত্যাগম 0;
                }
            }
            প্রত্যাগম -1;
        }
        c = next;
    }
}

তার্কিক LocaleDistance::isParadigmLSR(ধ্রুব LSR &lsr) ধ্রুব {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    প্রতি (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        যদি (lsr.isEquivalentTo(paradigmLSRs[i])) { প্রত্যাগম সত্য; }
    }
    প্রত্যাগম অসত্য;
}

U_NAMESPACE_END
