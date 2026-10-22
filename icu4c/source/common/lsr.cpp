// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "lsr.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(అక్షరం ఉపసర్గ, స్థిరరాశి అక్షరం *lang, స్థిరరాశి అక్షరం *scr, స్థిరరాశి అక్షరం *r, int32_t f,
         UError_సంకేతం &error_సంకేతం) :
        language(శూన్యసూచిక), script(శూన్యసూచిక), region(r),
        region_సూచీ(సూచీ_For_Region(region)), flags(f) {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        CharString langScript;
        langScript.append(ఉపసర్గ, error_సంకేతం).append(lang, error_సంకేతం).append('\0', error_సంకేతం);
        int32_t script_స్థానభ్రంశం = langScript.length();
        langScript.append(ఉపసర్గ, error_సంకేతం).append(scr, error_సంకేతం);
        owned = langScript.clone_దత్తాంశం(error_సంకేతం);
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            language = owned;
            script = owned + script_స్థానభ్రంశం;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_సంకేతం &error_సంకేతం) :
        language(శూన్యసూచిక), script(శూన్యసూచిక), region(శూన్యసూచిక),
        region_సూచీ(సూచీ_For_Region(r.దత్తాంశం())), flags(f) {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        CharString దత్తాంశం;
        దత్తాంశం.append(lang, error_సంకేతం).append('\0', error_సంకేతం);
        int32_t script_స్థానభ్రంశం = దత్తాంశం.length();
        దత్తాంశం.append(scr, error_సంకేతం).append('\0', error_సంకేతం);
        int32_t region_స్థానభ్రంశం = దత్తాంశం.length();
        దత్తాంశం.append(r, error_సంకేతం);
        owned = దత్తాంశం.clone_దత్తాంశం(error_సంకేతం);
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            language = owned;
            script = owned + script_స్థానభ్రంశం;
            region = owned + region_స్థానభ్రంశం;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_సూచీ(other.region_సూచీ), flags(other.flags),
        hash_సంకేతం(other.hash_సంకేతం) {
    ఒకవేళ (owned != శూన్యసూచిక) {
        other.language = other.script = "";
        other.owned = శూన్యసూచిక;
        other.hash_సంకేతం = 0;
    }
}

శూన్య LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::క్రియాకారకం=(LSR &&other) noexcept {
    ఇది->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_సూచీ = other.region_సూచీ;
    flags = other.flags;
    owned = other.owned;
    hash_సంకేతం = other.hash_సంకేతం;
    ఒకవేళ (owned != శూన్యసూచిక) {
        other.language = other.script = "";
        other.owned = శూన్యసూచిక;
        other.hash_సంకేతం = 0;
    }
    తిరిగి *ఇది;
}

UBool LSR::is_Equivalent_గమ్యం(స్థిరరాశి LSR &other) స్థిరరాశి {
    తిరిగి
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_సూచీ == other.region_సూచీ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_సూచీ > 0 || uprv_strcmp(region, other.region) == 0);
}

తార్కిక LSR::క్రియాకారకం==(స్థిరరాశి LSR &other) స్థిరరాశి {
    తిరిగి
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_సూచీ == other.region_సూచీ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_సూచీ > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::సూచీ_For_Region(స్థిరరాశి అక్షరం *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    ఒకవేళ (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        ఒకవేళ (b < 0 || 9 < b) { తిరిగి 0; }
        c = region[2] - '0';
        ఒకవేళ (c < 0 || 9 < c || region[3] != 0) { తిరిగి 0; }
        తిరిగి (10 * a + b) * 10 + c + 1;
    } లేకపోతే {  // letters: "DE"
        a = uprv_పై_Ordinal(c);
        ఒకవేళ (a < 0 || 25 < a) { తిరిగి 0; }
        int32_t b = uprv_పై_Ordinal(region[1]);
        ఒకవేళ (b < 0 || 25 < b || region[2] != 0) { తిరిగి 0; }
        తిరిగి 26 * a + b + 1001;
    }
    తిరిగి 0;
}

LSR &LSR::సమితి_Hash_సంకేతం() {
    ఒకవేళ (hash_సంకేతం == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_సూచీ;
        hash_సంకేతం = h * 37 + flags;
    }
    తిరిగి *ఇది;
}

U_NAMESPACE_చివర
