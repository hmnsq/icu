// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.h
// created: 2019may08 Markus W. Scherer

#నిర్వచితం_కాకపోతే __LOCDISTANCE_H__
#నిర్వచించు __LOCDISTANCE_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/localematcher.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "lsr.h"

U_NAMESPACE_BEGIN

నిర్మాణం Locale_దూరం_దత్తాంశం;

/**
 * Offline-built data for LocaleMatcher.
 * Mostly but not only the data for mapping locales to their maximized forms.
 */
class Locale_దూరం final : బహిరంగ UMemory {
బహిరంగ:
    స్థిర స్థిరరాశి Locale_దూరం *getSingleton(UError_సంకేతం &error_సంకేతం);

    స్థిర int32_t జరుపు_దూరం(int32_t దూరం) {
        తిరిగి దూరం << దూరం_జరుపు;
    }

    స్థిర int32_t get_Shifted_దూరం(int32_t సూచీ_And_దూరం) {
        తిరిగి సూచీ_And_దూరం & దూరం_ముసుగు;
    }

    స్థిర ద్విగుణ get_దూరం_Double(int32_t సూచీ_And_దూరం) {
        ద్విగుణ shifted_దూరం = get_Shifted_దూరం(సూచీ_And_దూరం);
        తిరిగి shifted_దూరం / (1 << దూరం_జరుపు);
    }

    స్థిర int32_t get_దూరం_Floor(int32_t సూచీ_And_దూరం) {
        తిరిగి (సూచీ_And_దూరం & దూరం_ముసుగు) >> దూరం_జరుపు;
    }

    స్థిర int32_t get_సూచీ(int32_t సూచీ_And_దూరం) {
        // assert indexAndDistance >= 0;
        తిరిగి సూచీ_And_దూరం >> సూచీ_జరుపు;
    }

    /**
     * Finds the supported LSR with the smallest distance from the desired one.
     * Equivalent LSR subtags must be normalized into a canonical form.
     *
     * <p>Returns the index of the lowest-distance supported LSR in the high bits
     * (negative if none has a distance below the threshold),
     * and its distance (0..ABOVE_THRESHOLD) in the low bits.
     */
    int32_t get_ఉత్తమ_సూచీ_And_దూరం(స్థిరరాశి LSR &desired,
                                    స్థిరరాశి LSR **supportedLSRs, int32_t supported_LSRs_నిడివి,
                                    int32_t shifted_పరిమితి,
                                    ULoc_సరిపోలిక_Favor_Subtag favorSubtag,
                                    ULoc_సరిపోలిక_Direction direction) స్థిరరాశి;

    bool isParadigmLSR(స్థిరరాశి LSR &lsr) స్థిరరాశి;

    int32_t get_Default_Script_దూరం() స్థిరరాశి {
        తిరిగి default_Script_దూరం;
    }

    int32_t get_Default_Demotion_చొప్పున_Desired_Locale() స్థిరరాశి {
        తిరిగి default_Demotion_చొప్పున_Desired_Locale;
    }

అంతరంగ:
    // The distance is shifted left to gain some fraction bits.
    స్థిర constexpr int32_t దూరం_జరుపు = 3;
    స్థిర constexpr int32_t దూరం_FRACTION_ముసుగు = 7;
    // 7 bits for 0..100
    స్థిర constexpr int32_t దూరం_INT_జరుపు = 7;
    స్థిర constexpr int32_t సూచీ_జరుపు = దూరం_INT_జరుపు + దూరం_జరుపు;
    స్థిర constexpr int32_t దూరం_ముసుగు = 0x3ff;
    // tic constexpr int32_t MAX_INDEX = 0x1fffff;  // avoids sign bit
    స్థిర constexpr int32_t సూచీ_NEG_1 = 0xfffffc00;

    Locale_దూరం(స్థిరరాశి Locale_దూరం_దత్తాంశం &దత్తాంశం, స్థిరరాశి LikelySubtags &likely);
    Locale_దూరం(స్థిరరాశి Locale_దూరం &other) = delete;
    Locale_దూరం &క్రియాకారకం=(స్థిరరాశి Locale_దూరం &other) = delete;

    స్థిర శూన్య init_Locale_దూరం(UError_సంకేతం &error_సంకేతం);

    bool is_సరిపోలిక(స్థిరరాశి LSR &desired, స్థిరరాశి LSR &supported,
                 int32_t shifted_పరిమితి, ULoc_సరిపోలిక_Favor_Subtag favorSubtag) స్థిరరాశి {
        స్థిరరాశి LSR *pSupp = &supported;
        తిరిగి get_ఉత్తమ_సూచీ_And_దూరం(
            desired, &pSupp, 1,
            shifted_పరిమితి, favorSubtag, ULOCMATCH_DIRECTION_WITH_ONE_WAY) >= 0;
    }

    స్థిర int32_t get_Des_Supp_Script_దూరం(బైట్లు_Trie &iter, uint64_t ప్రారంభం_State,
                                            స్థిరరాశి అక్షరం *desired, స్థిరరాశి అక్షరం *supported);

    స్థిర int32_t get_Region_Partitions_దూరం(
        బైట్లు_Trie &iter, uint64_t ప్రారంభం_State,
        స్థిరరాశి అక్షరం *desiredPartitions, స్థిరరాశి అక్షరం *supportedPartitions,
        int32_t పరిమితి);

    స్థిర int32_t get_Fallback_Region_దూరం(బైట్లు_Trie &iter, uint64_t ప్రారంభం_State);

    స్థిర int32_t trie_తదుపరి(బైట్లు_Trie &iter, స్థిరరాశి అక్షరం *s, bool want_విలువ);

    స్థిరరాశి అక్షరం *partitionsForRegion(స్థిరరాశి LSR &lsr) స్థిరరాశి {
        // ill-formed region -> one non-matching string
        int32_t p_సూచీ = region_గమ్యం_Partitions_సూచీ[lsr.region_సూచీ];
        తిరిగి విభజన_Arrays[p_సూచీ];
    }

    int32_t get_Default_Region_దూరం() స్థిరరాశి {
        తిరిగి default_Region_దూరం;
    }

    స్థిరరాశి LikelySubtags &likelySubtags;

    // The trie maps each dlang+slang+dscript+sscript+dregion+sregion
    // (encoded in ASCII with bit 7 set on the last character of each subtag) to a distance.
    // There is also a trie value for each subsequence of whole subtags.
    // One '*' is used for a (desired, supported) pair of "und", "Zzzz"/"", or "ZZ"/"".
    బైట్లు_Trie trie;

    /**
     * Maps each region to zero or more single-character partitions.
     */
    స్థిరరాశి uint8_t *region_గమ్యం_Partitions_సూచీ;
    స్థిరరాశి అక్షరం **విభజన_Arrays;

    /**
     * Used to get the paradigm region for a cluster, if there is one.
     */
    స్థిరరాశి LSR *paradigmLSRs;
    int32_t paradigm_LSRs_నిడివి;

    int32_t default_Language_దూరం;
    int32_t default_Script_దూరం;
    int32_t default_Region_దూరం;
    int32_t min_Region_దూరం;
    int32_t default_Demotion_చొప్పున_Desired_Locale;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __LOCDISTANCE_H__
