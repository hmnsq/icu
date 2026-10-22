// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FIXEDSTRING_H
#ವ್ಯಾಖ್ಯಾನಿಸು FIXEDSTRING_H

#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

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
class FixedString : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    FixedString() = ಪೂರ್ವನಿಯೋಜಿತ;
    ~FixedString() { ಕ್ರಿಯಾಕಾರಕ delete[](ptr); }

    FixedString(ಸ್ಥಿರಾಂಕ FixedString& other) : FixedString(other.ದತ್ತಾಂಶ()) {}

    FixedString(std::string_view init) {
        size_t ಗಾತ್ರ_3 = init.ಗಾತ್ರ_3();
        ಆದರೆ (ಗಾತ್ರ_3 > 0 && reserve(ಗಾತ್ರ_3 + 1)) {
            uprv_memcpy(ptr, init.ದತ್ತಾಂಶ(), ಗಾತ್ರ_3);
            ptr[ಗಾತ್ರ_3] = '\0';
        }
    }

    FixedString& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ FixedString& other) {
        *this = other.ದತ್ತಾಂಶ();
        ಹಿಂತಿರುಗಿ *this;
    }

    FixedString& ಕ್ರಿಯಾಕಾರಕ=(std::string_view init) {
        ಆದರೆ (init.ಖಾಲಿಗಳು()) {
            ಕ್ರಿಯಾಕಾರಕ delete[](ptr);
            ptr = nullptr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            size_t ಗಾತ್ರ_3 = init.ಗಾತ್ರ_3();
            ಆದರೆ (reserve(ಗಾತ್ರ_3 + 1)) {
                uprv_memcpy(ptr, init.ದತ್ತಾಂಶ(), ಗಾತ್ರ_3);
                ptr[ಗಾತ್ರ_3] = '\0';
            }
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& ಕ್ರಿಯಾಕಾರಕ=(FixedString&& other) noexcept {
        ಕ್ರಿಯಾಕಾರಕ delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }

    ಶೂನ್ಯ clear() {
        ಕ್ರಿಯಾಕಾರಕ delete[](ptr);
        ptr = nullptr;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ is_ಖಾಲಿಗಳು() ? "" : ptr;
    }

    ಅಕ್ಷರ* getAlias() {
        ಹಿಂತಿರುಗಿ ptr;
    }

    bool is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t ಗಾತ್ರ_3) {
        ಕ್ರಿಯಾಕಾರಕ delete[](ptr);
        ptr = static_cast<ಅಕ್ಷರ*>(ಕ್ರಿಯಾಕಾರಕ new[](ಗಾತ್ರ_3));
        ಹಿಂತಿರುಗಿ ptr != nullptr;
    }

ಖಾಸಗಿ:
    ಅಕ್ಷರ* ptr = nullptr;
};

U_COMMON_API ಶೂನ್ಯ copyInvariantChars(ಸ್ಥಿರಾಂಕ UnicodeString& src, FixedString& dst, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
