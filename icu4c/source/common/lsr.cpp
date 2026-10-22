// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(वर्ण उपसर्गः, ध्रुव वर्ण *lang, ध्रुव वर्ण *scr, ध्रुव वर्ण *r, int32_t f,
         UError_संहिता &error_संहिता) :
        language(शून्यसूचकः), script(शून्यसूचकः), region(r),
        region_सूचकांक(सूचकांक_For_Region(region)), flags(f) {
    यदि (U_SUCCESS(error_संहिता)) {
        CharString langScript;
        langScript.append(उपसर्गः, error_संहिता).append(lang, error_संहिता).append('\0', error_संहिता);
        int32_t script_विस्थापनम् = langScript.length();
        langScript.append(उपसर्गः, error_संहिता).append(scr, error_संहिता);
        owned = langScript.clone_दत्त(error_संहिता);
        यदि (U_SUCCESS(error_संहिता)) {
            language = owned;
            script = owned + script_विस्थापनम्;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_संहिता &error_संहिता) :
        language(शून्यसूचकः), script(शून्यसूचकः), region(शून्यसूचकः),
        region_सूचकांक(सूचकांक_For_Region(r.दत्त())), flags(f) {
    यदि (U_SUCCESS(error_संहिता)) {
        CharString दत्त;
        दत्त.append(lang, error_संहिता).append('\0', error_संहिता);
        int32_t script_विस्थापनम् = दत्त.length();
        दत्त.append(scr, error_संहिता).append('\0', error_संहिता);
        int32_t region_विस्थापनम् = दत्त.length();
        दत्त.append(r, error_संहिता);
        owned = दत्त.clone_दत्त(error_संहिता);
        यदि (U_SUCCESS(error_संहिता)) {
            language = owned;
            script = owned + script_विस्थापनम्;
            region = owned + region_विस्थापनम्;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_सूचकांक(other.region_सूचकांक), flags(other.flags),
        hash_संहिता(other.hash_संहिता) {
    यदि (owned != शून्यसूचकः) {
        other.language = other.script = "";
        other.owned = शून्यसूचकः;
        other.hash_संहिता = 0;
    }
}

शून्य LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::संक्रियकः=(LSR &&other) noexcept {
    एतत्->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_सूचकांक = other.region_सूचकांक;
    flags = other.flags;
    owned = other.owned;
    hash_संहिता = other.hash_संहिता;
    यदि (owned != शून्यसूचकः) {
        other.language = other.script = "";
        other.owned = शून्यसूचकः;
        other.hash_संहिता = 0;
    }
    प्रत्यागम *एतत्;
}

UBool LSR::is_Equivalent_गन्तव्यम्(ध्रुव LSR &other) ध्रुव {
    प्रत्यागम
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_सूचकांक == other.region_सूचकांक &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_सूचकांक > 0 || uprv_strcmp(region, other.region) == 0);
}

तार्किक LSR::संक्रियकः==(ध्रुव LSR &other) ध्रुव {
    प्रत्यागम
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_सूचकांक == other.region_सूचकांक &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_सूचकांक > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::सूचकांक_For_Region(ध्रुव वर्ण *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    यदि (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        यदि (b < 0 || 9 < b) { प्रत्यागम 0; }
        c = region[2] - '0';
        यदि (c < 0 || 9 < c || region[3] != 0) { प्रत्यागम 0; }
        प्रत्यागम (10 * a + b) * 10 + c + 1;
    } अन्यथा {  // letters: "DE"
        a = uprv_ऊर्ध्वस्थम्_Ordinal(c);
        यदि (a < 0 || 25 < a) { प्रत्यागम 0; }
        int32_t b = uprv_ऊर्ध्वस्थम्_Ordinal(region[1]);
        यदि (b < 0 || 25 < b || region[2] != 0) { प्रत्यागम 0; }
        प्रत्यागम 26 * a + b + 1001;
    }
    प्रत्यागम 0;
}

LSR &LSR::समुच्चयः_Hash_संहिता() {
    यदि (hash_संहिता == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_सूचकांक;
        hash_संहिता = h * 37 + flags;
    }
    प्रत्यागम *एतत्;
}

U_NAMESPACE_अन्तः
