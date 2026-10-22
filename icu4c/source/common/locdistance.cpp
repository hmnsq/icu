// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/localematcher.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "locdistance.h"
#അന്തര്ഭാവ "loclikelysubtags.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "umutex.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

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
എണ്ണൽ {
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
    തിരികെ സത്യം;
}

}  // namespace

ശൂന്യം U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    സ്ഥിരം LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    സ്ഥിരം LocaleDistanceData &data = likely.getDistanceData();
    എങ്കിൽ (data.distanceTrieBytes == nullptr ||
            data.regionToPartitions == nullptr || data.partitions == nullptr ||
            // ok if no paradigms
            data.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        തിരികെ;
    }
    gLocaleDistance = പുതിയ LocaleDistance(data, likely);
    എങ്കിൽ (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

സ്ഥിരം LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    തിരികെ gLocaleDistance;
}

LocaleDistance::LocaleDistance(സ്ഥിരം LocaleDistanceData &data, സ്ഥിരം LikelySubtags &likely) :
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
    സ്ഥിരം LSR *p_enGB = &enGB;
    int32_t indexAndDistance = getBestIndexAndDistance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(indexAndDistance);
}

int32_t LocaleDistance::getBestIndexAndDistance(
        സ്ഥിരം LSR &desired,
        സ്ഥിരം LSR **supportedLSRs, int32_t supportedLSRsLength,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) സ്ഥിരം {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trieNext(iter, desired.language, അസത്യം);
    uint64_t desLangState = desLangDistance >= 0 && supportedLSRsLength > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t bestIndex = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    വേണ്ടി (int32_t slIndex = 0; slIndex < supportedLSRsLength; ++slIndex) {
        സ്ഥിരം LSR &supported = *supportedLSRs[slIndex];
        തർക്കം star = അസത്യം;
        int32_t distance = desLangDistance;
        എങ്കിൽ (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            എങ്കിൽ (slIndex != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trieNext(iter, supported.language, സത്യം);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        എങ്കിൽ (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } അല്ലെങ്കിൽ {  // <*, *>
            എങ്കിൽ (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } അല്ലെങ്കിൽ {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = സത്യം;
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
        എങ്കിൽ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        എങ്കിൽ (distance > roundedThreshold) {
            തുടരുക;
        }

        int32_t scriptDistance;
        എങ്കിൽ (star || flags != 0) {
            എങ്കിൽ (uprv_strcmp(desired.script, supported.script) == 0) {
                scriptDistance = 0;
            } അല്ലെങ്കിൽ {
                scriptDistance = defaultScriptDistance;
            }
        } അല്ലെങ്കിൽ {
            scriptDistance = getDesSuppScriptDistance(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = scriptDistance & DISTANCE_IS_FINAL;
            scriptDistance &= ~DISTANCE_IS_FINAL;
        }
        distance += scriptDistance;
        എങ്കിൽ (distance > roundedThreshold) {
            തുടരുക;
        }

        എങ്കിൽ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } അല്ലെങ്കിൽ എങ്കിൽ (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } അല്ലെങ്കിൽ {
            int32_t remainingThreshold = roundedThreshold - distance;
            എങ്കിൽ (minRegionDistance > remainingThreshold) {
                തുടരുക;
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
        എങ്കിൽ (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            എങ്കിൽ (shiftedDistance < shiftedThreshold) {
                എങ്കിൽ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    എങ്കിൽ (shiftedDistance == 0) {
                        തിരികെ slIndex << INDEX_SHIFT;
                    }
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (shiftedDistance < shiftedThreshold) {
                എങ്കിൽ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestIndex = slIndex;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (shiftedDistance == shiftedThreshold && bestIndex >= 0) {
                എങ്കിൽ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[bestIndex], bestLikelyInfo);
                    എങ്കിൽ ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        bestIndex = slIndex;
                    }
                }
            }
        }
    }
    തിരികെ bestIndex >= 0 ?
            (bestIndex << INDEX_SHIFT) | shiftedThreshold :
            INDEX_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, സ്ഥിരം അക്ഷരം *desired, സ്ഥിരം അക്ഷരം *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trieNext(iter, desired, അസത്യം);
    എങ്കിൽ (distance >= 0) {
        distance = trieNext(iter, supported, സത്യം);
    }
    എങ്കിൽ (distance < 0) {
        UStringTrieResult result = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_മൂല്യം(result));
        എങ്കിൽ (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } അല്ലെങ്കിൽ {
            distance = iter.get_മൂല്യം();
            U_ASSERT(distance >= 0);
        }
        എങ്കിൽ (result == USTRINGTRIE_FINAL_മൂല്യം) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    തിരികെ distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        സ്ഥിരം അക്ഷരം *desiredPartitions, സ്ഥിരം അക്ഷരം *supportedPartitions, int32_t threshold) {
    അക്ഷരം desired = *desiredPartitions++;
    അക്ഷരം supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    തർക്കം suppLengthGt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    എങ്കിൽ (*desiredPartitions == 0 && !suppLengthGt1) {
        // Fastpath for single desired/supported partitions.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        എങ്കിൽ (USTRINGTRIE_HAS_NEXT(result)) {
            result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
                തിരികെ iter.get_മൂല്യം();
            }
        }
        തിരികെ getFallbackRegionDistance(iter, startState);
    }

    സ്ഥിരം അക്ഷരം *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    തർക്കം star = അസത്യം;
    വേണ്ടി (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UStringTrieResult result = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        എങ്കിൽ (USTRINGTRIE_HAS_NEXT(result)) {
            uint64_t desState = suppLengthGt1 ? iter.getState64() : 0;
            വേണ്ടി (;;) {
                result = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
                    d = iter.get_മൂല്യം();
                } അല്ലെങ്കിൽ എങ്കിൽ (star) {
                    d = 0;
                } അല്ലെങ്കിൽ {
                    d = getFallbackRegionDistance(iter, startState);
                    star = സത്യം;
                }
                എങ്കിൽ (d > threshold) {
                    തിരികെ d;
                } അല്ലെങ്കിൽ എങ്കിൽ (regionDistance < d) {
                    regionDistance = d;
                }
                എങ്കിൽ ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } അല്ലെങ്കിൽ {
                    നിർത്തുക;
                }
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            എങ്കിൽ (d > threshold) {
                തിരികെ d;
            } അല്ലെങ്കിൽ എങ്കിൽ (regionDistance < d) {
                regionDistance = d;
            }
            star = സത്യം;
        }
        എങ്കിൽ ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } അല്ലെങ്കിൽ {
            നിർത്തുക;
        }
    }
    തിരികെ regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#എങ്കിൽ U_DEBUG
    UStringTrieResult result =
#endif
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_മൂല്യം(result));
    int32_t distance = iter.get_മൂല്യം();
    U_ASSERT(distance >= 0);
    തിരികെ distance;
}

int32_t LocaleDistance::trieNext(BytesTrie &iter, സ്ഥിരം അക്ഷരം *s, തർക്കം want_മൂല്യം) {
    uint8_t c;
    എങ്കിൽ ((c = *s) == 0) {
        തിരികെ -1;  // no empty subtags in the distance data
    }
    വേണ്ടി (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        എങ്കിൽ (next != 0) {
            എങ്കിൽ (!USTRINGTRIE_HAS_NEXT(iter.next(c))) {
                തിരികെ -1;
            }
        } അല്ലെങ്കിൽ {
            // last character of this subtag
            UStringTrieResult result = iter.next(c | END_OF_SUBTAG);
            എങ്കിൽ (want_മൂല്യം) {
                എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
                    int32_t value = iter.get_മൂല്യം();
                    എങ്കിൽ (result == USTRINGTRIE_FINAL_മൂല്യം) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    തിരികെ value;
                }
            } അല്ലെങ്കിൽ {
                എങ്കിൽ (USTRINGTRIE_HAS_NEXT(result)) {
                    തിരികെ 0;
                }
            }
            തിരികെ -1;
        }
        c = next;
    }
}

തർക്കം LocaleDistance::isParadigmLSR(സ്ഥിരം LSR &lsr) സ്ഥിരം {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigmLSRsLength <= 15);
    വേണ്ടി (int32_t i = 0; i < paradigmLSRsLength; ++i) {
        എങ്കിൽ (lsr.isEquivalentTo(paradigmLSRs[i])) { തിരികെ സത്യം; }
    }
    തിരികെ അസത്യം;
}

U_NAMESPACE_END
