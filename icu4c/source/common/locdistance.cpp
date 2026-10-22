// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/localematcher.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/ures.h"
#समावेश "cstring.h"
#समावेश "locdistance.h"
#समावेश "loclikelysubtags.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "uinvchar.h"
#समावेश "umutex.h"

U_NAMESPACE_BEGIN

namespace {

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
क्रमागत {
    IX_DEF_LANG_DISTANCE,
    IX_DEF_SCRIPT_DISTANCE,
    IX_DEF_REGION_DISTANCE,
    IX_MIN_REGION_DISTANCE,
    IX_LIMIT
};

LocaleDistance *gLocaleDistance = nullptr;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    मिटाओ gLocaleDistance;
    gLocaleDistance = nullptr;
    gInitOnce.reset();
    वापस true;
}

}  // namespace

व्योम U_CALLCONV LocaleDistance::initLocaleDistance(UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == nullptr);
    स्थिर LikelySubtags &likely = *LikelySubtags::getSingleton(errorCode);
    अगर (U_FAILURE(errorCode)) { वापस; }
    स्थिर Locale_Distance_तथ्य &तथ्य = likely.get_Distance_तथ्य();
    अगर (तथ्य.distanceTrieBytes == nullptr ||
            तथ्य.regionToPartitions == nullptr || तथ्य.partitions == nullptr ||
            // ok if no paradigms
            तथ्य.distances == nullptr) {
        errorCode = U_MISSING_RESOURCE_ERROR;
        वापस;
    }
    gLocaleDistance = नया LocaleDistance(तथ्य, likely);
    अगर (gLocaleDistance == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

स्थिर LocaleDistance *LocaleDistance::getSingleton(UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, errorCode);
    वापस gLocaleDistance;
}

LocaleDistance::LocaleDistance(स्थिर Locale_Distance_तथ्य &तथ्य, स्थिर LikelySubtags &likely) :
        likelySubtags(likely),
        trie(तथ्य.distanceTrieBytes),
        region_To_Partitions_सूचकांक(तथ्य.regionToPartitions), partitionArrays(तथ्य.partitions),
        paradigmLSRs(तथ्य.paradigms), paradigm_LSRs_लंबाई(तथ्य.paradigms_लंबाई),
        defaultLanguageDistance(तथ्य.distances[IX_DEF_LANG_DISTANCE]),
        defaultScriptDistance(तथ्य.distances[IX_DEF_SCRIPT_DISTANCE]),
        defaultRegionDistance(तथ्य.distances[IX_DEF_REGION_DISTANCE]),
        minRegionDistance(तथ्य.distances[IX_MIN_REGION_DISTANCE]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    स्थिर LSR *p_enGB = &enGB;
    int32_t सूचकांक_And_Distance = get_Best_सूचकांक_And_Distance(en, &p_enGB, 1,
            shiftDistance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    defaultDemotionPerDesiredLocale  = getDistanceFloor(सूचकांक_And_Distance);
}

int32_t LocaleDistance::get_Best_सूचकांक_And_Distance(
        स्थिर LSR &desired,
        स्थिर LSR **supportedLSRs, int32_t supported_LSRs_लंबाई,
        int32_t shiftedThreshold,
        ULocMatchFavorSubtag favorSubtag, ULocMatchDirection direction) स्थिर {
    BytesTrie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trie_अग्र(iter, desired.language, false);
    uint64_t desLangState = desLangDistance >= 0 && supported_LSRs_लंबाई > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t best_सूचकांक = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    क्रम (int32_t sl_सूचकांक = 0; sl_सूचकांक < supported_LSRs_लंबाई; ++sl_सूचकांक) {
        स्थिर LSR &supported = *supportedLSRs[sl_सूचकांक];
        bool star = false;
        int32_t distance = desLangDistance;
        अगर (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            अगर (sl_सूचकांक != 0) {
                iter.resetToState64(desLangState);
            }
            distance = trie_अग्र(iter, supported.language, true);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        अगर (distance >= 0) {
            flags = distance & DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
            distance &= ~DISTANCE_IS_FINAL_OR_SKIP_SCRIPT;
        } अन्यथा {  // <*, *>
            अगर (uprv_strcmp(desired.language, supported.language) == 0) {
                distance = 0;
            } अन्यथा {
                distance = defaultLanguageDistance;
            }
            flags = 0;
            star = true;
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
        अगर (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            distance >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        अगर (distance > roundedThreshold) {
            जारी;
        }

        int32_t scriptDistance;
        अगर (star || flags != 0) {
            अगर (uprv_strcmp(desired.script, supported.script) == 0) {
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
        अगर (distance > roundedThreshold) {
            जारी;
        }

        अगर (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } अन्यथा अगर (star || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } अन्यथा {
            int32_t remainingThreshold = roundedThreshold - distance;
            अगर (minRegionDistance > remainingThreshold) {
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
        अगर (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            अगर (shiftedDistance < shiftedThreshold) {
                अगर (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    अगर (shiftedDistance == 0) {
                        वापस sl_सूचकांक << सूचकांक_SHIFT;
                    }
                    best_सूचकांक = sl_सूचकांक;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } अन्यथा {
            अगर (shiftedDistance < shiftedThreshold) {
                अगर (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    best_सूचकांक = sl_सूचकांक;
                    shiftedThreshold = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } अन्यथा अगर (shiftedDistance == shiftedThreshold && best_सूचकांक >= 0) {
                अगर (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        isMatch(supported, desired, shiftedThreshold, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[best_सूचकांक], bestLikelyInfo);
                    अगर ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        best_सूचकांक = sl_सूचकांक;
                    }
                }
            }
        }
    }
    वापस best_सूचकांक >= 0 ?
            (best_सूचकांक << सूचकांक_SHIFT) | shiftedThreshold :
            सूचकांक_NEG_1 | shiftDistance(ABOVE_THRESHOLD);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        BytesTrie &iter, uint64_t startState, स्थिर अक्षर *desired, स्थिर अक्षर *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trie_अग्र(iter, desired, false);
    अगर (distance >= 0) {
        distance = trie_अग्र(iter, supported, true);
    }
    अगर (distance < 0) {
        UString_Trie_परिणाम परिणाम = iter.resetToState64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_मूल्य(परिणाम));
        अगर (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } अन्यथा {
            distance = iter.get_मूल्य();
            U_ASSERT(distance >= 0);
        }
        अगर (परिणाम == USTRINGTRIE_FINAL_मूल्य) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    वापस distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        BytesTrie &iter, uint64_t startState,
        स्थिर अक्षर *desiredPartitions, स्थिर अक्षर *supportedPartitions, int32_t threshold) {
    अक्षर desired = *desiredPartitions++;
    अक्षर supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    bool supp_लंबाई_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    अगर (*desiredPartitions == 0 && !supp_लंबाई_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_परिणाम परिणाम = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        अगर (USTRINGTRIE_HAS_अग्र(परिणाम)) {
            परिणाम = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
            अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
                वापस iter.get_मूल्य();
            }
        }
        वापस getFallbackRegionDistance(iter, startState);
    }

    स्थिर अक्षर *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    bool star = false;
    क्रम (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_परिणाम परिणाम = iter.next(uprv_invCharToAscii(desired) | END_OF_SUBTAG);
        अगर (USTRINGTRIE_HAS_अग्र(परिणाम)) {
            uint64_t desState = supp_लंबाई_Gt1 ? iter.getState64() : 0;
            क्रम (;;) {
                परिणाम = iter.next(uprv_invCharToAscii(supported) | END_OF_SUBTAG);
                int32_t d;
                अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
                    d = iter.get_मूल्य();
                } अन्यथा अगर (star) {
                    d = 0;
                } अन्यथा {
                    d = getFallbackRegionDistance(iter, startState);
                    star = true;
                }
                अगर (d > threshold) {
                    वापस d;
                } अन्यथा अगर (regionDistance < d) {
                    regionDistance = d;
                }
                अगर ((supported = *supportedPartitions++) != 0) {
                    iter.resetToState64(desState);
                } अन्यथा {
                    अवरोध;
                }
            }
        } अन्यथा अगर (!star) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            अगर (d > threshold) {
                वापस d;
            } अन्यथा अगर (regionDistance < d) {
                regionDistance = d;
            }
            star = true;
        }
        अगर ((desired = *desiredPartitions++) != 0) {
            iter.resetToState64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } अन्यथा {
            अवरोध;
        }
    }
    वापस regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(BytesTrie &iter, uint64_t startState) {
#अगर U_DEBUG
    UString_Trie_परिणाम परिणाम =
#पूर्ण_अगर
    iter.resetToState64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_मूल्य(परिणाम));
    int32_t distance = iter.get_मूल्य();
    U_ASSERT(distance >= 0);
    वापस distance;
}

int32_t LocaleDistance::trie_अग्र(BytesTrie &iter, स्थिर अक्षर *s, bool want_मूल्य) {
    uint8_t c;
    अगर ((c = *s) == 0) {
        वापस -1;  // no empty subtags in the distance data
    }
    क्रम (;;) {
        c = uprv_invCharToAscii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        अगर (next != 0) {
            अगर (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                वापस -1;
            }
        } अन्यथा {
            // last character of this subtag
            UString_Trie_परिणाम परिणाम = iter.next(c | END_OF_SUBTAG);
            अगर (want_मूल्य) {
                अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
                    int32_t value = iter.get_मूल्य();
                    अगर (परिणाम == USTRINGTRIE_FINAL_मूल्य) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    वापस value;
                }
            } अन्यथा {
                अगर (USTRINGTRIE_HAS_अग्र(परिणाम)) {
                    वापस 0;
                }
            }
            वापस -1;
        }
        c = next;
    }
}

bool LocaleDistance::isParadigmLSR(स्थिर LSR &lsr) स्थिर {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_लंबाई <= 15);
    क्रम (int32_t i = 0; i < paradigm_LSRs_लंबाई; ++i) {
        अगर (lsr.isEquivalentTo(paradigmLSRs[i])) { वापस true; }
    }
    वापस false;
}

U_NAMESPACE_END
