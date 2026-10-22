// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "cmemory.h"

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
class FixedString : જાહેર UMemory {
જાહેર:
    FixedString() = મૂળભૂત;
    ~FixedString() { operator delete[](ptr); }

    FixedString(અચલ FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        જો (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(અચલ FixedString& other) {
        *this = other.data();
        પાછા *this;
    }

    FixedString& operator=(std::string_view init) {
        જો (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } નહીંતર {
            size_t size = init.size();
            જો (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        પાછા *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        પાછા *this;
    }

    શૂન્ય clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    અચલ અક્ષર* data() અચલ {
        પાછા isEmpty() ? "" : ptr;
    }

    અક્ષર* getAlias() {
        પાછા ptr;
    }

    bool isEmpty() અચલ {
        પાછા ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<અક્ષર*>(operator new[](size));
        પાછા ptr != nullptr;
    }

ખાનગી:
    અક્ષર* ptr = nullptr;
};

U_COMMON_API શૂન્ય copyInvariantChars(અચલ UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
