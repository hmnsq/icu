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
constexpr int32_t अन्तः_सम्बन्धि_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t DISTANCE_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t DISTANCE_IS_FINAL = 0x100;
constexpr int32_t DISTANCE_IS_FINAL_OR_SKIP_SCRIPT = DISTANCE_IS_FINAL | DISTANCE_SKIP_SCRIPT;

constexpr int32_t ABOVE_देहली = 100;

// Indexes into array of distances.
गणना {
    IX_DEF_LANG_DISTANCE,
    IX_DEF_SCRIPT_DISTANCE,
    IX_DEF_REGION_DISTANCE,
    IX_MIN_REGION_DISTANCE,
    IX_सीमा
};

LocaleDistance *gLocaleDistance = शून्यसूचकः;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    विलोपय gLocaleDistance;
    gLocaleDistance = शून्यसूचकः;
    gInitOnce.reset();
    प्रत्यागम सत्य;
}

}  // namespace

शून्य U_CALLCONV LocaleDistance::initLocaleDistance(UError_संहिता &error_संहिता) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLocaleDistance == शून्यसूचकः);
    ध्रुव LikelySubtags &likely = *LikelySubtags::getSingleton(error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    ध्रुव Locale_Distance_दत्त &दत्त = likely.get_Distance_दत्त();
    यदि (दत्त.distance_Trie_अष्टकानि == शून्यसूचकः ||
            दत्त.region_गन्तव्यम्_Partitions == शून्यसूचकः || दत्त.partitions == शून्यसूचकः ||
            // ok if no paradigms
            दत्त.distances == शून्यसूचकः) {
        error_संहिता = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम;
    }
    gLocaleDistance = नूतन LocaleDistance(दत्त, likely);
    यदि (gLocaleDistance == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE_DISTANCE, cleanup);
}

ध्रुव LocaleDistance *LocaleDistance::getSingleton(UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(gInitOnce, &LocaleDistance::initLocaleDistance, error_संहिता);
    प्रत्यागम gLocaleDistance;
}

LocaleDistance::LocaleDistance(ध्रुव Locale_Distance_दत्त &दत्त, ध्रुव LikelySubtags &likely) :
        likelySubtags(likely),
        trie(दत्त.distance_Trie_अष्टकानि),
        region_गन्तव्यम्_Partitions_सूचकांक(दत्त.region_गन्तव्यम्_Partitions), partitionArrays(दत्त.partitions),
        paradigmLSRs(दत्त.paradigms), paradigm_LSRs_दीर्घता(दत्त.paradigms_दीर्घता),
        defaultLanguageDistance(दत्त.distances[IX_DEF_LANG_DISTANCE]),
        defaultScriptDistance(दत्त.distances[IX_DEF_SCRIPT_DISTANCE]),
        defaultRegionDistance(दत्त.distances[IX_DEF_REGION_DISTANCE]),
        minRegionDistance(दत्त.distances[IX_MIN_REGION_DISTANCE]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ध्रुव LSR *p_enGB = &enGB;
    int32_t सूचकांक_And_Distance = get_Best_सूचकांक_And_Distance(en, &p_enGB, 1,
            सरणम्_Distance(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_प्रत्येकम्_Desired_Locale  = getDistanceFloor(सूचकांक_And_Distance);
}

int32_t LocaleDistance::get_Best_सूचकांक_And_Distance(
        ध्रुव LSR &desired,
        ध्रुव LSR **supportedLSRs, int32_t supported_LSRs_दीर्घता,
        int32_t shifted_देहली,
        ULoc_मेलनम्_Favor_Subtag favorSubtag, ULoc_मेलनम्_Direction direction) ध्रुव {
    अष्टकानि_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t desLangDistance = trie_अग्र(iter, desired.language, असत्य);
    uint64_t desLangState = desLangDistance >= 0 && supported_LSRs_दीर्घता > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t best_सूचकांक = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t bestLikelyInfo = -1;
    प्रति (int32_t sl_सूचकांक = 0; sl_सूचकांक < supported_LSRs_दीर्घता; ++sl_सूचकांक) {
        ध्रुव LSR &supported = *supportedLSRs[sl_सूचकांक];
        तार्किक तारकम् = असत्य;
        int32_t distance = desLangDistance;
        यदि (distance >= 0) {
            U_ASSERT((distance & DISTANCE_IS_FINAL) == 0);
            यदि (sl_सूचकांक != 0) {
                iter.reset_गन्तव्यम्_State64(desLangState);
            }
            distance = trie_अग्र(iter, supported.language, सत्य);
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
            तारकम् = सत्य;
        }
        U_ASSERT(0 <= distance && distance <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_देहली = (shifted_देहली + DISTANCE_FRACTION_आवरणम्) >> DISTANCE_सरणम्;
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
        यदि (distance > rounded_देहली) {
            अनुवर्त;
        }

        int32_t scriptDistance;
        यदि (तारकम् || flags != 0) {
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
        यदि (distance > rounded_देहली) {
            अनुवर्त;
        }

        यदि (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } अन्यथा यदि (तारकम् || (flags & DISTANCE_IS_FINAL) != 0) {
            distance += defaultRegionDistance;
        } अन्यथा {
            int32_t remaining_देहली = rounded_देहली - distance;
            यदि (minRegionDistance > remaining_देहली) {
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
                    remaining_देहली);
        }
        int32_t shiftedDistance = सरणम्_Distance(distance);
        यदि (shiftedDistance == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shiftedDistance |= (desired.flags ^ supported.flags);
            यदि (shiftedDistance < shifted_देहली) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_मेलनम्(supported, desired, shifted_देहली, favorSubtag)) {
                    यदि (shiftedDistance == 0) {
                        प्रत्यागम sl_सूचकांक << सूचकांक_सरणम्;
                    }
                    best_सूचकांक = sl_सूचकांक;
                    shifted_देहली = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            }
        } अन्यथा {
            यदि (shiftedDistance < shifted_देहली) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_मेलनम्(supported, desired, shifted_देहली, favorSubtag)) {
                    best_सूचकांक = sl_सूचकांक;
                    shifted_देहली = shiftedDistance;
                    bestLikelyInfo = -1;
                }
            } अन्यथा यदि (shiftedDistance == shifted_देहली && best_सूचकांक >= 0) {
                यदि (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_मेलनम्(supported, desired, shifted_देहली, favorSubtag)) {
                    bestLikelyInfo = likelySubtags.compareLikely(
                            supported, *supportedLSRs[best_सूचकांक], bestLikelyInfo);
                    यदि ((bestLikelyInfo & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        best_सूचकांक = sl_सूचकांक;
                    }
                }
            }
        }
    }
    प्रत्यागम best_सूचकांक >= 0 ?
            (best_सूचकांक << सूचकांक_सरणम्) | shifted_देहली :
            सूचकांक_NEG_1 | सरणम्_Distance(ABOVE_देहली);
}

int32_t LocaleDistance::getDesSuppScriptDistance(
        अष्टकानि_Trie &iter, uint64_t startState, ध्रुव वर्ण *desired, ध्रुव वर्ण *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t distance = trie_अग्र(iter, desired, असत्य);
    यदि (distance >= 0) {
        distance = trie_अग्र(iter, supported, सत्य);
    }
    यदि (distance < 0) {
        UString_Trie_परिणाम परिणाम = iter.reset_गन्तव्यम्_State64(startState).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_मानम्(परिणाम));
        यदि (uprv_strcmp(desired, supported) == 0) {
            distance = 0;  // same script
        } अन्यथा {
            distance = iter.get_मानम्();
            U_ASSERT(distance >= 0);
        }
        यदि (परिणाम == USTRINGTRIE_FINAL_मानम्) {
            distance |= DISTANCE_IS_FINAL;
        }
    }
    प्रत्यागम distance;
}

int32_t LocaleDistance::getRegionPartitionsDistance(
        अष्टकानि_Trie &iter, uint64_t startState,
        ध्रुव वर्ण *desiredPartitions, ध्रुव वर्ण *supportedPartitions, int32_t देहली) {
    वर्ण desired = *desiredPartitions++;
    वर्ण supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    तार्किक supp_दीर्घता_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    यदि (*desiredPartitions == 0 && !supp_दीर्घता_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_परिणाम परिणाम = iter.next(uprv_inv_Char_गन्तव्यम्_Ascii(desired) | अन्तः_सम्बन्धि_SUBTAG);
        यदि (USTRINGTRIE_HAS_अग्र(परिणाम)) {
            परिणाम = iter.next(uprv_inv_Char_गन्तव्यम्_Ascii(supported) | अन्तः_सम्बन्धि_SUBTAG);
            यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
                प्रत्यागम iter.get_मानम्();
            }
        }
        प्रत्यागम getFallbackRegionDistance(iter, startState);
    }

    ध्रुव वर्ण *supportedStart = supportedPartitions - 1;  // for restart of inner loop
    int32_t regionDistance = 0;
    // Fall back to * only once, not for each pair of partition strings.
    तार्किक तारकम् = असत्य;
    प्रति (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_परिणाम परिणाम = iter.next(uprv_inv_Char_गन्तव्यम्_Ascii(desired) | अन्तः_सम्बन्धि_SUBTAG);
        यदि (USTRINGTRIE_HAS_अग्र(परिणाम)) {
            uint64_t desState = supp_दीर्घता_Gt1 ? iter.getState64() : 0;
            प्रति (;;) {
                परिणाम = iter.next(uprv_inv_Char_गन्तव्यम्_Ascii(supported) | अन्तः_सम्बन्धि_SUBTAG);
                int32_t d;
                यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
                    d = iter.get_मानम्();
                } अन्यथा यदि (तारकम्) {
                    d = 0;
                } अन्यथा {
                    d = getFallbackRegionDistance(iter, startState);
                    तारकम् = सत्य;
                }
                यदि (d > देहली) {
                    प्रत्यागम d;
                } अन्यथा यदि (regionDistance < d) {
                    regionDistance = d;
                }
                यदि ((supported = *supportedPartitions++) != 0) {
                    iter.reset_गन्तव्यम्_State64(desState);
                } अन्यथा {
                    भङ्ग;
                }
            }
        } अन्यथा यदि (!तारकम्) {
            int32_t d = getFallbackRegionDistance(iter, startState);
            यदि (d > देहली) {
                प्रत्यागम d;
            } अन्यथा यदि (regionDistance < d) {
                regionDistance = d;
            }
            तारकम् = सत्य;
        }
        यदि ((desired = *desiredPartitions++) != 0) {
            iter.reset_गन्तव्यम्_State64(startState);
            supportedPartitions = supportedStart;
            supported = *supportedPartitions++;
        } अन्यथा {
            भङ्ग;
        }
    }
    प्रत्यागम regionDistance;
}

int32_t LocaleDistance::getFallbackRegionDistance(अष्टकानि_Trie &iter, uint64_t startState) {
#यदि U_DEBUG
    UString_Trie_परिणाम परिणाम =
#यदि_अन्तः
    iter.reset_गन्तव्यम्_State64(startState).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_मानम्(परिणाम));
    int32_t distance = iter.get_मानम्();
    U_ASSERT(distance >= 0);
    प्रत्यागम distance;
}

int32_t LocaleDistance::trie_अग्र(अष्टकानि_Trie &iter, ध्रुव वर्ण *s, तार्किक want_मानम्) {
    uint8_t c;
    यदि ((c = *s) == 0) {
        प्रत्यागम -1;  // no empty subtags in the distance data
    }
    प्रति (;;) {
        c = uprv_inv_Char_गन्तव्यम्_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        यदि (next != 0) {
            यदि (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                प्रत्यागम -1;
            }
        } अन्यथा {
            // last character of this subtag
            UString_Trie_परिणाम परिणाम = iter.next(c | अन्तः_सम्बन्धि_SUBTAG);
            यदि (want_मानम्) {
                यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
                    int32_t value = iter.get_मानम्();
                    यदि (परिणाम == USTRINGTRIE_FINAL_मानम्) {
                        value |= DISTANCE_IS_FINAL;
                    }
                    प्रत्यागम value;
                }
            } अन्यथा {
                यदि (USTRINGTRIE_HAS_अग्र(परिणाम)) {
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
    U_ASSERT(paradigm_LSRs_दीर्घता <= 15);
    प्रति (int32_t i = 0; i < paradigm_LSRs_दीर्घता; ++i) {
        यदि (lsr.is_Equivalent_गन्तव्यम्(paradigmLSRs[i])) { प्रत्यागम सत्य; }
    }
    प्रत्यागम असत्य;
}

U_NAMESPACE_अन्तः
