// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// charstrmap.h
// created: 2020sep01 Frank Yung-Fong Tang

#నిర్వచితం_కాకపోతే __CHARSTRMAP_H__
#నిర్వచించు __CHARSTRMAP_H__

#అన్తర్భావ <utility>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Map of const char * keys & values.
 * Stores pointers as is: Does not own/copy/adopt/release strings.
 */
class CharStringMap final : బహిరంగ UMemory {
బహిరంగ:
    /** Constructs an unusable non-map. */
    CharStringMap() : map(nullptr) {}
    CharStringMap(int32_t కొలత_2, UError_సంకేతం &error_సంకేతం) {
        map = uhash_open_కొలత(uhash_hashChars, uhash_compareChars, uhash_compareChars,
                             కొలత_2, &error_సంకేతం);
    }
    CharStringMap(CharStringMap &&other) noexcept : map(other.map) {
        other.map = nullptr;
    }
    CharStringMap(స్థిరరాశి CharStringMap &other) = delete;
    ~CharStringMap() {
        uhash_close(map);
    }

    CharStringMap &క్రియాకారకం=(CharStringMap &&other) noexcept {
        map = other.map;
        other.map = nullptr;
        తిరిగి *this;
    }
    CharStringMap &క్రియాకారకం=(స్థిరరాశి CharStringMap &other) = delete;

    స్థిరరాశి అక్షరం *get(స్థిరరాశి అక్షరం *కీలకం) స్థిరరాశి { తిరిగి static_cast<స్థిరరాశి అక్షరం *>(uhash_get(map, కీలకం)); }
    శూన్య put(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ_2, UError_సంకేతం &error_సంకేతం) {
        uhash_put(map, const_cast<అక్షరం *>(కీలకం), const_cast<అక్షరం *>(విలువ_2), &error_సంకేతం);
    }

అంతరంగ:
    UHashtable *map;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  //  __CHARSTRMAP_H__
