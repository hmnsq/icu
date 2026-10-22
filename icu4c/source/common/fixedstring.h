// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#నిర్వచితం_కాకపోతే FIXEDSTRING_H
#నిర్వచించు FIXEDSTRING_H

#అన్తర్భావ <string_view>
#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

/**
 * ICU-internal fixed-length char* string class.
 * This is a complement to CharString to store fixed-length strings efficiently
 * (not allocating any unnecessary storage for future additions to the string).
 *
 * A terminating NUL is always stored, but the length of the string isn't.
 * An empty string is stored as nullptr, allocating no storage at all.
 *
 * This class wants to be convenient but is also deliberately minimalist.
 * Please do not add methods if they only add minor convenience.
 */
class FixedString : బహిరంగ UMemory {
బహిరంగ:
    FixedString() = అప్రమేయ;
    ~FixedString() { క్రియాకారకం delete[](ptr); }

    FixedString(స్థిరరాశి FixedString& other) : FixedString(other.దత్తాంశం()) {}

    FixedString(std::string_view init) {
        size_t కొలత_2 = init.కొలత_2();
        ఒకవేళ (కొలత_2 > 0 && reserve(కొలత_2 + 1)) {
            uprv_memcpy(ptr, init.దత్తాంశం(), కొలత_2);
            ptr[కొలత_2] = '\0';
        }
    }

    FixedString& క్రియాకారకం=(స్థిరరాశి FixedString& other) {
        *this = other.దత్తాంశం();
        తిరిగి *this;
    }

    FixedString& క్రియాకారకం=(std::string_view init) {
        ఒకవేళ (init.ఖాళీలు()) {
            క్రియాకారకం delete[](ptr);
            ptr = nullptr;
        } లేకపోతే {
            size_t కొలత_2 = init.కొలత_2();
            ఒకవేళ (reserve(కొలత_2 + 1)) {
                uprv_memcpy(ptr, init.దత్తాంశం(), కొలత_2);
                ptr[కొలత_2] = '\0';
            }
        }
        తిరిగి *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& క్రియాకారకం=(FixedString&& other) noexcept {
        క్రియాకారకం delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        తిరిగి *this;
    }

    శూన్య clear() {
        క్రియాకారకం delete[](ptr);
        ptr = nullptr;
    }

    స్థిరరాశి అక్షరం* దత్తాంశం() స్థిరరాశి {
        తిరిగి is_ఖాళీలు() ? "" : ptr;
    }

    అక్షరం* getAlias() {
        తిరిగి ptr;
    }

    bool is_ఖాళీలు() స్థిరరాశి {
        తిరిగి ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t కొలత_2) {
        క్రియాకారకం delete[](ptr);
        ptr = static_cast<అక్షరం*>(క్రియాకారకం new[](కొలత_2));
        తిరిగి ptr != nullptr;
    }

అంతరంగ:
    అక్షరం* ptr = nullptr;
};

U_COMMON_API శూన్య copyInvariantChars(స్థిరరాశి UnicodeString& src, FixedString& dst, UError_సంకేతం& స్థితి_2);

U_NAMESPACE_చివర

#అయితే_ముగింపు
