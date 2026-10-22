// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.h
// created: 2019may08 Markus W. Scherer

#నిర్వచితం_కాకపోతే __LOCLIKELYSUBTAGS_H__
#నిర్వచించు __LOCLIKELYSUBTAGS_H__

#అన్తర్భావ <utility>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "charstrmap.h"
#అన్తర్భావ "lsr.h"

U_NAMESPACE_BEGIN

నిర్మాణం Likely_Subtags_దత్తాంశం;

నిర్మాణం Locale_దూరం_దత్తాంశం {
    Locale_దూరం_దత్తాంశం() = అప్రమేయ;
    Locale_దూరం_దత్తాంశం(Locale_దూరం_దత్తాంశం &&దత్తాంశం);
    ~Locale_దూరం_దత్తాంశం();

    స్థిరరాశి uint8_t *దూరం_Trie_బైట్లు = nullptr;
    స్థిరరాశి uint8_t *region_గమ్యం_Partitions = nullptr;
    స్థిరరాశి అక్షరం **partitions = nullptr;
    స్థిరరాశి LSR *paradigms = nullptr;
    int32_t paradigms_నిడివి = 0;
    స్థిరరాశి int32_t *distances = nullptr;

అంతరంగ:
    Locale_దూరం_దత్తాంశం &క్రియాకారకం=(స్థిరరాశి Locale_దూరం_దత్తాంశం &) = delete;
};

class LikelySubtags final : బహిరంగ UMemory {
బహిరంగ:
    ~LikelySubtags();

    స్థిర constexpr int32_t SKIP_SCRIPT = 1;

    // VisibleForTesting
    స్థిర స్థిరరాశి LikelySubtags *getSingleton(UError_సంకేతం &error_సంకేతం);

    // VisibleForTesting
    LSR makeMaximizedLsrFrom(స్థిరరాశి Locale &locale,
                             bool return_ఆదానం_If_Unmatch,
                             UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    /**
     * Tests whether lsr is "more likely" than other.
     * For example, fr-Latn-FR is more likely than fr-Latn-CH because
     * FR is the default region for fr-Latn.
     *
     * The likelyInfo caches lookup information between calls.
     * The return value is an updated likelyInfo value,
     * with bit 0 set if lsr is "more likely".
     * The initial value of likelyInfo must be negative.
     */
    int32_t compareLikely(స్థిరరాశి LSR &lsr, స్థిరరాశి LSR &other, int32_t likelyInfo) స్థిరరాశి;

    LSR minimizeSubtags(StringPiece language, StringPiece script, StringPiece region,
                        bool favorScript,
                        UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    // visible for LocaleDistance
    స్థిరరాశి Locale_దూరం_దత్తాంశం& get_దూరం_దత్తాంశం() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి దూరం_దత్తాంశం; }

అంతరంగ:
    LikelySubtags(Likely_Subtags_దత్తాంశం &దత్తాంశం);
    LikelySubtags(స్థిరరాశి LikelySubtags &other) = delete;
    LikelySubtags &క్రియాకారకం=(స్థిరరాశి LikelySubtags &other) = delete;

    స్థిర శూన్య initLikelySubtags(UError_సంకేతం &error_సంకేతం);

    LSR makeMaximizedLsr(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script, స్థిరరాశి అక్షరం *region,
                         స్థిరరాశి అక్షరం *variant,
                         bool return_ఆదానం_If_Unmatch,
                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    /**
     * Raw access to addLikelySubtags. Input must be in canonical format, eg "en", not "eng" or "EN".
     */
    LSR maximize(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script, స్థిరరాశి అక్షరం *region,
                 bool return_ఆదానం_If_Unmatch,
                 UError_సంకేతం &error_సంకేతం) స్థిరరాశి;
    LSR maximize(StringPiece language, StringPiece script, StringPiece region,
                 bool return_ఆదానం_If_Unmatch,
                 UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    int32_t get_Likely_సూచీ(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script) స్థిరరాశి;
    bool isMacroregion(StringPiece& region, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    స్థిర int32_t trie_తదుపరి(బైట్లు_Trie &iter, స్థిరరాశి అక్షరం *s, int32_t i);
    స్థిర int32_t trie_తదుపరి(బైట్లు_Trie &iter, StringPiece s, int32_t i);

    UResourceBundle *langInfoBundle;
    // We could store the strings by value, except that if there were few enough strings,
    // moving the contents could copy it to a different array,
    // invalidating the pointers stored in the maps.
    CharString *అక్షరశ్రేణులు;
    CharStringMap languageAliases;
    CharStringMap regionAliases;

    // The trie maps each lang+script+region (encoded in ASCII) to an index into lsrs.
    // There is also a trie value for each intermediate lang and lang+script.
    // '*' is used instead of "und", "Zzzz"/"" and "ZZ"/"".
    బైట్లు_Trie trie;
    uint64_t trieUndState;
    uint64_t trieUndZzzzState;
    int32_t default_Lsr_సూచీ;
    uint64_t trie_మొదటి_Letter_స్థితులు[26];
    స్థిరరాశి LSR *lsrs;
#ఒకవేళ U_DEBUG
    int32_t lsrs_నిడివి;
#అయితే_ముగింపు

    // distance/matcher data: see comment in LikelySubtagsData::load()
    Locale_దూరం_దత్తాంశం దూరం_దత్తాంశం;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __LOCLIKELYSUBTAGS_H__
