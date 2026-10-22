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

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    ಗಣನಾ Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_ಎಣಿಕೆ
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ಸ್ಥಿರ Category toCategory(ಧ್ರುವ ವರ್ಣ *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ಸ್ಥಿರ Category toCategory(ಧ್ರುವ UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *getCategoryName(Category category);
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
class PluralMap : ಸಾರ್ವಜನಿಕ PluralMapBase {
ಸಾರ್ವಜನಿಕ:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ಧ್ರುವ T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ಧ್ರುವ PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        ಪ್ರತಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(ಧ್ರುವ PluralMap<T> &other) {
        ಯದಿ (this == &other) {
            ಪ್ರತ್ಯಾಗಮ *this;
        }
        ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ಯದಿ (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } ಅನ್ಯಥಾ ಯದಿ (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } ಅನ್ಯಥಾ ಯದಿ (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } ಅನ್ಯಥಾ {
                // do nothing
            }
        }
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    ~PluralMap() {
        ಪ್ರತಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    ಶೂನ್ಯ clear() {
        *fVariants[0] = T();
        ಪ್ರತಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ಧ್ರುವ T *next(Category &index) ಧ್ರುವ {
        int32_t idx = index;
        ++idx;
        ಪ್ರತಿ (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            ಯದಿ (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                ಪ್ರತ್ಯಾಗಮ fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        ಧ್ರುವ T *result = next(index);
        ಪ್ರತ್ಯಾಗಮ const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ಧ್ರುವ T &getOther() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ಧ್ರುವ T &get(Category v) ಧ್ರುವ {
        int32_t index = v;
        ಯದಿ (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            ಪ್ರತ್ಯಾಗಮ *fVariants[0];
        }
        ಪ್ರತ್ಯಾಗಮ *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ಧ್ರುವ T &get(ಧ್ರುವ ವರ್ಣ *category) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ಧ್ರುವ T &get(ಧ್ರುವ UnicodeString &category) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ get(toCategory(category));
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
        ಪ್ರತ್ಯಾಗಮ getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ಧ್ರುವ ವರ್ಣ *category,
            UErrorCode &status) {
        ಪ್ರತ್ಯಾಗಮ getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ಧ್ರುವ T &default_ಮೌಲ್ಯ,
            UErrorCode &status) {
        ಪ್ರತ್ಯಾಗಮ getMutable(category, &default_ಮೌಲ್ಯ, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ಧ್ರುವ PluralMap<T> &rhs,
            UBool (*eqFunc)(ಧ್ರುವ T &, ಧ್ರುವ T &)) ಧ್ರುವ {
        ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ಯದಿ (fVariants[i] == rhs.fVariants[i]) {
                ಅನುವರ್ತ;
            }
            ಯದಿ (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                ಪ್ರತ್ಯಾಗಮ false;
            }
            ಯದಿ (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                ಪ್ರತ್ಯಾಗಮ false;
            }
        }
        ಪ್ರತ್ಯಾಗಮ true;
    }

ಗುಪ್ತ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ಧ್ರುವ T *default_ಮೌಲ್ಯ,
            UErrorCode &status) {
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        int32_t index = category;
        ಯದಿ (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಯದಿ (fVariants[index] == nullptr) {
            fVariants[index] = default_ಮೌಲ್ಯ == nullptr ?
                    new T() : new T(*default_ಮೌಲ್ಯ);
        }
        ಯದಿ (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ fVariants[index];
    }

    ಶೂನ್ಯ initializeNew() {
        fVariants[0] = &fOtherVariant;
        ಪ್ರತಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
