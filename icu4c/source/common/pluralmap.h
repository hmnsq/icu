// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File pluralmap.h - PluralMap class that maps plural categories to values.
******************************************************************************
*/

#ifndef __PLURAL_MAP_H__
#define __PLURAL_MAP_H__

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    ଗଣନା Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_ଗଣନା
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ସ୍ଥିର Category toCategory(ଅଚଳ ଅକ୍ଷର *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ସ୍ଥିର Category toCategory(ଅଚଳ UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *getCategoryName(Category category);
};

/**
 * A Map of plural categories to values. It maintains ownership of the
 * values.
 *
 * Type T is the value type. T must provide the following:
 * 1) Default constructor
 * 2) Copy constructor
 * 3) Assignment operator
 * 4) Must extend UMemory
 */
template<typename T>
class PluralMap : ସର୍ବସାଧାରଣ PluralMapBase {
ସର୍ବସାଧାରଣ:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ଅଚଳ T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ଅଚଳ PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        ପାଇଁ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(ଅଚଳ PluralMap<T> &other) {
        ଯଦି (this == &other) {
            ଫେରନ୍ତୁ *this;
        }
        ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ଯଦି (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } ଅନ୍ୟଥା ଯଦି (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } ଅନ୍ୟଥା ଯଦି (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } ଅନ୍ୟଥା {
                // do nothing
            }
        }
        ଫେରନ୍ତୁ *this;
    }

    ~PluralMap() {
        ପାଇଁ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    ଶୂନ୍ୟ clear() {
        *fVariants[0] = T();
        ପାଇଁ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
            fVariants[i] = nullptr;
        }
    }

    /**
     * Iterates through the mappings in this instance, set index to NONE
     * prior to using. Call next repeatedly to get the values until it
     * returns nullptr. Each time next returns, caller may pass index
     * to getCategoryName() to get the name of the plural category.
     * When this function returns nullptr, index is CATEGORY_COUNT
     */
    ଅଚଳ T *next(Category &index) ଅଚଳ {
        int32_t idx = index;
        ++idx;
        ପାଇଁ (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            ଯଦି (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                ଫେରନ୍ତୁ fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        ଫେରନ୍ତୁ nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        ଅଚଳ T *result = next(index);
        ଫେରନ୍ତୁ const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ଅଚଳ T &getOther() ଅଚଳ {
        ଫେରନ୍ତୁ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ଅଚଳ T &get(Category v) ଅଚଳ {
        int32_t index = v;
        ଯଦି (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            ଫେରନ୍ତୁ *fVariants[0];
        }
        ଫେରନ୍ତୁ *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ଅଚଳ T &get(ଅଚଳ ଅକ୍ଷର *category) ଅଚଳ {
        ଫେରନ୍ତୁ get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ଅଚଳ T &get(ଅଚଳ UnicodeString &category) ଅଚଳ {
        ଫେରନ୍ତୁ get(toCategory(category));
    }

    /**
     * Returns a pointer to the value associated with a category
     * that caller can safely modify. If the value was defaulting to the 'other'
     * variant because no explicit value was stored, this method creates a
     * new value using the default constructor at the returned pointer.
     *
     * @param category the category with the value to change.
     * @param status error returned here if index is NONE or CATEGORY_COUNT
     *  or memory could not be allocated, or any other error happens.
     */
    T *getMutable(
            Category category,
            UErrorCode &status) {
        ଫେରନ୍ତୁ getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ଅଚଳ ଅକ୍ଷର *category,
            UErrorCode &status) {
        ଫେରନ୍ତୁ getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ଅଚଳ T &default_ମୂଲ୍ୟ,
            UErrorCode &status) {
        ଫେରନ୍ତୁ getMutable(category, &default_ମୂଲ୍ୟ, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ଅଚଳ PluralMap<T> &rhs,
            UBool (*eqFunc)(ଅଚଳ T &, ଅଚଳ T &)) ଅଚଳ {
        ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ଯଦି (fVariants[i] == rhs.fVariants[i]) {
                ଜାରି;
            }
            ଯଦି (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                ଫେରନ୍ତୁ false;
            }
            ଯଦି (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                ଫେରନ୍ତୁ false;
            }
        }
        ଫେରନ୍ତୁ true;
    }

ବ୍ୟକ୍ତିଗତ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ଅଚଳ T *default_ମୂଲ୍ୟ,
            UErrorCode &status) {
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ nullptr;
        }
        int32_t index = category;
        ଯଦି (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        ଯଦି (fVariants[index] == nullptr) {
            fVariants[index] = default_ମୂଲ୍ୟ == nullptr ?
                    new T() : new T(*default_ମୂଲ୍ୟ);
        }
        ଯଦି (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ଫେରନ୍ତୁ fVariants[index];
    }

    ଶୂନ୍ୟ initializeNew() {
        fVariants[0] = &fOtherVariant;
        ପାଇଁ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
