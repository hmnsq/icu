// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ifndef FIXEDSTRING_H
#define FIXEDSTRING_H

#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

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
class FixedString : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    FixedString() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
    ~FixedString() { operator delete[](ptr); }

    FixedString(ଅଚଳ FixedString& other) : FixedString(other.data()) {}

    FixedString(std::string_view init) {
        size_t size = init.size();
        ଯଦି (size > 0 && reserve(size + 1)) {
            uprv_memcpy(ptr, init.data(), size);
            ptr[size] = '\0';
        }
    }

    FixedString& operator=(ଅଚଳ FixedString& other) {
        *this = other.data();
        ଫେରନ୍ତୁ *this;
    }

    FixedString& operator=(std::string_view init) {
        ଯଦି (init.empty()) {
            operator delete[](ptr);
            ptr = nullptr;
        } ଅନ୍ୟଥା {
            size_t size = init.size();
            ଯଦି (reserve(size + 1)) {
                uprv_memcpy(ptr, init.data(), size);
                ptr[size] = '\0';
            }
        }
        ଫେରନ୍ତୁ *this;
    }

    FixedString(FixedString&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}

    FixedString& operator=(FixedString&& other) noexcept {
        operator delete[](ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        ଫେରନ୍ତୁ *this;
    }

    ଶୂନ୍ୟ clear() {
        operator delete[](ptr);
        ptr = nullptr;
    }

    ଅଚଳ ଅକ୍ଷର* data() ଅଚଳ {
        ଫେରନ୍ତୁ isEmpty() ? "" : ptr;
    }

    ଅକ୍ଷର* getAlias() {
        ଫେରନ୍ତୁ ptr;
    }

    bool isEmpty() ଅଚଳ {
        ଫେରନ୍ତୁ ptr == nullptr;
    }

    /** Allocate storage for a new string, without initializing it. */
    bool reserve(size_t size) {
        operator delete[](ptr);
        ptr = static_cast<ଅକ୍ଷର*>(operator new[](size));
        ଫେରନ୍ତୁ ptr != nullptr;
    }

ବ୍ୟକ୍ତିଗତ:
    ଅକ୍ଷର* ptr = nullptr;
};

U_COMMON_API ଶୂନ୍ୟ copyInvariantChars(ଅଚଳ UnicodeString& src, FixedString& dst, UErrorCode& status);

U_NAMESPACE_END

#endif
