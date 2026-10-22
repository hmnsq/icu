// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/localematcher.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "locdistance.h"
#అన్తర్భావ "loclikelysubtags.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "umutex.h"

U_NAMESPACE_BEGIN

నామాకాశ {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t చివర_యొక్క_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t దూరం_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t దూరం_IS_FINAL = 0x100;
constexpr int32_t దూరం_IS_FINAL_OR_SKIP_SCRIPT = దూరం_IS_FINAL | దూరం_SKIP_SCRIPT;

constexpr int32_t ABOVE_పరిమితి = 100;

// Indexes into array of distances.
గణన {
    IX_DEF_LANG_దూరం,
    IX_DEF_SCRIPT_దూరం,
    IX_DEF_REGION_దూరం,
    IX_MIN_REGION_దూరం,
    IX_హద్దు
};

Locale_దూరం *g_Locale_దూరం = శూన్యసూచిక;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    తొలగించు g_Locale_దూరం;
    g_Locale_దూరం = శూన్యసూచిక;
    gInitOnce.reset();
    తిరిగి నిజం;
}

}  // namespace

శూన్య U_CALLCONV Locale_దూరం::init_Locale_దూరం(UError_సంకేతం &error_సంకేతం) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_దూరం == శూన్యసూచిక);
    స్థిరరాశి LikelySubtags &likely = *LikelySubtags::getSingleton(error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    స్థిరరాశి Locale_దూరం_దత్తాంశం &దత్తాంశం = likely.get_దూరం_దత్తాంశం();
    ఒకవేళ (దత్తాంశం.దూరం_Trie_బైట్లు == శూన్యసూచిక ||
            దత్తాంశం.region_గమ్యం_Partitions == శూన్యసూచిక || దత్తాంశం.partitions == శూన్యసూచిక ||
            // ok if no paradigms
            దత్తాంశం.distances == శూన్యసూచిక) {
        error_సంకేతం = U_MISSING_RESOURCE_ERROR;
        తిరిగి;
    }
    g_Locale_దూరం = కొత్త Locale_దూరం(దత్తాంశం, likely);
    ఒకవేళ (g_Locale_దూరం == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE_దూరం, cleanup);
}

స్థిరరాశి Locale_దూరం *Locale_దూరం::getSingleton(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(gInitOnce, &Locale_దూరం::init_Locale_దూరం, error_సంకేతం);
    తిరిగి g_Locale_దూరం;
}

Locale_దూరం::Locale_దూరం(స్థిరరాశి Locale_దూరం_దత్తాంశం &దత్తాంశం, స్థిరరాశి LikelySubtags &likely) :
        likelySubtags(likely),
        trie(దత్తాంశం.దూరం_Trie_బైట్లు),
        region_గమ్యం_Partitions_సూచీ(దత్తాంశం.region_గమ్యం_Partitions), విభజన_Arrays(దత్తాంశం.partitions),
        paradigmLSRs(దత్తాంశం.paradigms), paradigm_LSRs_నిడివి(దత్తాంశం.paradigms_నిడివి),
        default_Language_దూరం(దత్తాంశం.distances[IX_DEF_LANG_దూరం]),
        default_Script_దూరం(దత్తాంశం.distances[IX_DEF_SCRIPT_దూరం]),
        default_Region_దూరం(దత్తాంశం.distances[IX_DEF_REGION_దూరం]),
        min_Region_దూరం(దత్తాంశం.distances[IX_MIN_REGION_దూరం]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    స్థిరరాశి LSR *p_enGB = &enGB;
    int32_t సూచీ_And_దూరం = get_ఉత్తమ_సూచీ_And_దూరం(en, &p_enGB, 1,
            జరుపు_దూరం(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_చొప్పున_Desired_Locale  = get_దూరం_Floor(సూచీ_And_దూరం);
}

int32_t Locale_దూరం::get_ఉత్తమ_సూచీ_And_దూరం(
        స్థిరరాశి LSR &desired,
        స్థిరరాశి LSR **supportedLSRs, int32_t supported_LSRs_నిడివి,
        int32_t shifted_పరిమితి,
        ULoc_సరిపోలిక_Favor_Subtag favorSubtag, ULoc_సరిపోలిక_Direction direction) స్థిరరాశి {
    బైట్లు_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_దూరం = trie_తదుపరి(iter, desired.language, అబద్ధం);
    uint64_t desLangState = des_Lang_దూరం >= 0 && supported_LSRs_నిడివి > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t ఉత్తమ_సూచీ = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t ఉత్తమ_Likely_Info = -1;
    కొరకు (int32_t sl_సూచీ = 0; sl_సూచీ < supported_LSRs_నిడివి; ++sl_సూచీ) {
        స్థిరరాశి LSR &supported = *supportedLSRs[sl_సూచీ];
        తార్కిక నక్షత్రం = అబద్ధం;
        int32_t దూరం = des_Lang_దూరం;
        ఒకవేళ (దూరం >= 0) {
            U_ASSERT((దూరం & దూరం_IS_FINAL) == 0);
            ఒకవేళ (sl_సూచీ != 0) {
                iter.reset_గమ్యం_State64(desLangState);
            }
            దూరం = trie_తదుపరి(iter, supported.language, నిజం);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        ఒకవేళ (దూరం >= 0) {
            flags = దూరం & దూరం_IS_FINAL_OR_SKIP_SCRIPT;
            దూరం &= ~దూరం_IS_FINAL_OR_SKIP_SCRIPT;
        } లేకపోతే {  // <*, *>
            ఒకవేళ (uprv_strcmp(desired.language, supported.language) == 0) {
                దూరం = 0;
            } లేకపోతే {
                దూరం = default_Language_దూరం;
            }
            flags = 0;
            నక్షత్రం = నిజం;
        }
        U_ASSERT(0 <= దూరం && దూరం <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_పరిమితి = (shifted_పరిమితి + దూరం_FRACTION_ముసుగు) >> దూరం_జరుపు;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        ఒకవేళ (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            దూరం >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        ఒకవేళ (దూరం > rounded_పరిమితి) {
            కొనసాగు;
        }

        int32_t script_దూరం;
        ఒకవేళ (నక్షత్రం || flags != 0) {
            ఒకవేళ (uprv_strcmp(desired.script, supported.script) == 0) {
                script_దూరం = 0;
            } లేకపోతే {
                script_దూరం = default_Script_దూరం;
            }
        } లేకపోతే {
            script_దూరం = get_Des_Supp_Script_దూరం(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_దూరం & దూరం_IS_FINAL;
            script_దూరం &= ~దూరం_IS_FINAL;
        }
        దూరం += script_దూరం;
        ఒకవేళ (దూరం > rounded_పరిమితి) {
            కొనసాగు;
        }

        ఒకవేళ (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } లేకపోతే ఒకవేళ (నక్షత్రం || (flags & దూరం_IS_FINAL) != 0) {
            దూరం += default_Region_దూరం;
        } లేకపోతే {
            int32_t మిగిలిన_పరిమితి = rounded_పరిమితి - దూరం;
            ఒకవేళ (min_Region_దూరం > మిగిలిన_పరిమితి) {
                కొనసాగు;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            దూరం += get_Region_Partitions_దూరం(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    మిగిలిన_పరిమితి);
        }
        int32_t shifted_దూరం = జరుపు_దూరం(దూరం);
        ఒకవేళ (shifted_దూరం == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_దూరం |= (desired.flags ^ supported.flags);
            ఒకవేళ (shifted_దూరం < shifted_పరిమితి) {
                ఒకవేళ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_సరిపోలిక(supported, desired, shifted_పరిమితి, favorSubtag)) {
                    ఒకవేళ (shifted_దూరం == 0) {
                        తిరిగి sl_సూచీ << సూచీ_జరుపు;
                    }
                    ఉత్తమ_సూచీ = sl_సూచీ;
                    shifted_పరిమితి = shifted_దూరం;
                    ఉత్తమ_Likely_Info = -1;
                }
            }
        } లేకపోతే {
            ఒకవేళ (shifted_దూరం < shifted_పరిమితి) {
                ఒకవేళ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_సరిపోలిక(supported, desired, shifted_పరిమితి, favorSubtag)) {
                    ఉత్తమ_సూచీ = sl_సూచీ;
                    shifted_పరిమితి = shifted_దూరం;
                    ఉత్తమ_Likely_Info = -1;
                }
            } లేకపోతే ఒకవేళ (shifted_దూరం == shifted_పరిమితి && ఉత్తమ_సూచీ >= 0) {
                ఒకవేళ (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_సరిపోలిక(supported, desired, shifted_పరిమితి, favorSubtag)) {
                    ఉత్తమ_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[ఉత్తమ_సూచీ], ఉత్తమ_Likely_Info);
                    ఒకవేళ ((ఉత్తమ_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        ఉత్తమ_సూచీ = sl_సూచీ;
                    }
                }
            }
        }
    }
    తిరిగి ఉత్తమ_సూచీ >= 0 ?
            (ఉత్తమ_సూచీ << సూచీ_జరుపు) | shifted_పరిమితి :
            సూచీ_NEG_1 | జరుపు_దూరం(ABOVE_పరిమితి);
}

int32_t Locale_దూరం::get_Des_Supp_Script_దూరం(
        బైట్లు_Trie &iter, uint64_t ప్రారంభం_State, స్థిరరాశి అక్షరం *desired, స్థిరరాశి అక్షరం *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t దూరం = trie_తదుపరి(iter, desired, అబద్ధం);
    ఒకవేళ (దూరం >= 0) {
        దూరం = trie_తదుపరి(iter, supported, నిజం);
    }
    ఒకవేళ (దూరం < 0) {
        UString_Trie_ఫలితం ఫలితం = iter.reset_గమ్యం_State64(ప్రారంభం_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_విలువ(ఫలితం));
        ఒకవేళ (uprv_strcmp(desired, supported) == 0) {
            దూరం = 0;  // same script
        } లేకపోతే {
            దూరం = iter.get_విలువ();
            U_ASSERT(దూరం >= 0);
        }
        ఒకవేళ (ఫలితం == USTRINGTRIE_FINAL_విలువ) {
            దూరం |= దూరం_IS_FINAL;
        }
    }
    తిరిగి దూరం;
}

int32_t Locale_దూరం::get_Region_Partitions_దూరం(
        బైట్లు_Trie &iter, uint64_t ప్రారంభం_State,
        స్థిరరాశి అక్షరం *desiredPartitions, స్థిరరాశి అక్షరం *supportedPartitions, int32_t పరిమితి) {
    అక్షరం desired = *desiredPartitions++;
    అక్షరం supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    తార్కిక supp_నిడివి_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    ఒకవేళ (*desiredPartitions == 0 && !supp_నిడివి_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_ఫలితం ఫలితం = iter.next(uprv_inv_Char_గమ్యం_Ascii(desired) | చివర_యొక్క_SUBTAG);
        ఒకవేళ (USTRINGTRIE_HAS_తదుపరి(ఫలితం)) {
            ఫలితం = iter.next(uprv_inv_Char_గమ్యం_Ascii(supported) | చివర_యొక్క_SUBTAG);
            ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
                తిరిగి iter.get_విలువ();
            }
        }
        తిరిగి get_Fallback_Region_దూరం(iter, ప్రారంభం_State);
    }

    స్థిరరాశి అక్షరం *supported_ప్రారంభం = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_దూరం = 0;
    // Fall back to * only once, not for each pair of partition strings.
    తార్కిక నక్షత్రం = అబద్ధం;
    కొరకు (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_ఫలితం ఫలితం = iter.next(uprv_inv_Char_గమ్యం_Ascii(desired) | చివర_యొక్క_SUBTAG);
        ఒకవేళ (USTRINGTRIE_HAS_తదుపరి(ఫలితం)) {
            uint64_t desState = supp_నిడివి_Gt1 ? iter.getState64() : 0;
            కొరకు (;;) {
                ఫలితం = iter.next(uprv_inv_Char_గమ్యం_Ascii(supported) | చివర_యొక్క_SUBTAG);
                int32_t d;
                ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
                    d = iter.get_విలువ();
                } లేకపోతే ఒకవేళ (నక్షత్రం) {
                    d = 0;
                } లేకపోతే {
                    d = get_Fallback_Region_దూరం(iter, ప్రారంభం_State);
                    నక్షత్రం = నిజం;
                }
                ఒకవేళ (d > పరిమితి) {
                    తిరిగి d;
                } లేకపోతే ఒకవేళ (region_దూరం < d) {
                    region_దూరం = d;
                }
                ఒకవేళ ((supported = *supportedPartitions++) != 0) {
                    iter.reset_గమ్యం_State64(desState);
                } లేకపోతే {
                    ఆపు;
                }
            }
        } లేకపోతే ఒకవేళ (!నక్షత్రం) {
            int32_t d = get_Fallback_Region_దూరం(iter, ప్రారంభం_State);
            ఒకవేళ (d > పరిమితి) {
                తిరిగి d;
            } లేకపోతే ఒకవేళ (region_దూరం < d) {
                region_దూరం = d;
            }
            నక్షత్రం = నిజం;
        }
        ఒకవేళ ((desired = *desiredPartitions++) != 0) {
            iter.reset_గమ్యం_State64(ప్రారంభం_State);
            supportedPartitions = supported_ప్రారంభం;
            supported = *supportedPartitions++;
        } లేకపోతే {
            ఆపు;
        }
    }
    తిరిగి region_దూరం;
}

int32_t Locale_దూరం::get_Fallback_Region_దూరం(బైట్లు_Trie &iter, uint64_t ప్రారంభం_State) {
#ఒకవేళ U_DEBUG
    UString_Trie_ఫలితం ఫలితం =
#అయితే_ముగింపు
    iter.reset_గమ్యం_State64(ప్రారంభం_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_విలువ(ఫలితం));
    int32_t దూరం = iter.get_విలువ();
    U_ASSERT(దూరం >= 0);
    తిరిగి దూరం;
}

int32_t Locale_దూరం::trie_తదుపరి(బైట్లు_Trie &iter, స్థిరరాశి అక్షరం *s, తార్కిక want_విలువ) {
    uint8_t c;
    ఒకవేళ ((c = *s) == 0) {
        తిరిగి -1;  // no empty subtags in the distance data
    }
    కొరకు (;;) {
        c = uprv_inv_Char_గమ్యం_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        ఒకవేళ (next != 0) {
            ఒకవేళ (!USTRINGTRIE_HAS_తదుపరి(iter.next(c))) {
                తిరిగి -1;
            }
        } లేకపోతే {
            // last character of this subtag
            UString_Trie_ఫలితం ఫలితం = iter.next(c | చివర_యొక్క_SUBTAG);
            ఒకవేళ (want_విలువ) {
                ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
                    int32_t value = iter.get_విలువ();
                    ఒకవేళ (ఫలితం == USTRINGTRIE_FINAL_విలువ) {
                        value |= దూరం_IS_FINAL;
                    }
                    తిరిగి value;
                }
            } లేకపోతే {
                ఒకవేళ (USTRINGTRIE_HAS_తదుపరి(ఫలితం)) {
                    తిరిగి 0;
                }
            }
            తిరిగి -1;
        }
        c = next;
    }
}

తార్కిక Locale_దూరం::isParadigmLSR(స్థిరరాశి LSR &lsr) స్థిరరాశి {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_నిడివి <= 15);
    కొరకు (int32_t i = 0; i < paradigm_LSRs_నిడివి; ++i) {
        ఒకవేళ (lsr.is_Equivalent_గమ్యం(paradigmLSRs[i])) { తిరిగి నిజం; }
    }
    తిరిగి అబద్ధం;
}

U_NAMESPACE_చివర
