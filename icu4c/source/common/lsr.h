// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#నిర్వచితం_కాకపోతే __LSR_H__
#నిర్వచించు __LSR_H__

#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cstring.h"

U_NAMESPACE_BEGIN

నిర్మాణం LSR final : బహిరంగ UMemory {
    స్థిర constexpr int32_t REGION_సూచీ_హద్దు = 1001 + 26 * 26;

    స్థిర constexpr int32_t EXPLICIT_LSR = 7;
    స్థిర constexpr int32_t EXPLICIT_LANGUAGE = 4;
    స్థిర constexpr int32_t EXPLICIT_SCRIPT = 2;
    స్థిర constexpr int32_t EXPLICIT_REGION = 1;
    స్థిర constexpr int32_t IMPLICIT_LSR = 0;
    స్థిర constexpr int32_t DONT_CARE_FLAGS = 0;

    స్థిరరాశి అక్షరం *language;
    స్థిరరాశి అక్షరం *script;
    స్థిరరాశి అక్షరం *region;
    అక్షరం *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_సూచీ = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hash_సంకేతం = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(స్థిరరాశి అక్షరం *lang, స్థిరరాశి అక్షరం *scr, స్థిరరాశి అక్షరం *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_సూచీ(సూచీ_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(అక్షరం ఉపసర్గ, స్థిరరాశి అక్షరం *lang, స్థిరరాశి అక్షరం *scr, స్థిరరాశి అక్షరం *r, int32_t f,
        UError_సంకేతం &error_సంకేతం);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UError_సంకేతం &error_సంకేతం);
    LSR(LSR &&other) noexcept;
    LSR(స్థిరరాశి LSR &other) = delete;
    అంతఃవరుస ~LSR() {
        // Pure inline code for almost all instances.
        ఒకవేళ (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &క్రియాకారకం=(LSR &&other) noexcept;
    LSR &క్రియాకారకం=(స్థిరరాశి LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    స్థిర int32_t సూచీ_For_Region(స్థిరరాశి అక్షరం *region);

    UBool is_Equivalent_గమ్యం(స్థిరరాశి LSR &other) స్థిరరాశి;
    bool క్రియాకారకం==(స్థిరరాశి LSR &other) స్థిరరాశి;

    అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి LSR &other) స్థిరరాశి {
        తిరిగి !క్రియాకారకం==(other);
    }

    LSR &సమితి_Hash_సంకేతం();

అంతరంగ:
    శూన్య deleteOwned();
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __LSR_H__
