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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __PLURAL_MAP_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __PLURAL_MAP_H__

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API Plural_Map_ತಳ : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    ಎಣಿಕೆ Category {
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
    ಸ್ಥಿರ Category ಗಮ್ಯ_Category(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ಸ್ಥಿರ Category ಗಮ್ಯ_Category(ಸ್ಥಿರಾಂಕ UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getCategoryName(Category category);
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
ಮಾದರಿ<typename T>
class PluralMap : ಸಾರ್ವಜನಿಕ Plural_Map_ತಳ {
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
    PluralMap(ಸ್ಥಿರಾಂಕ T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ಸ್ಥಿರಾಂಕ PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        ಗಾಗಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ PluralMap<T> &other) {
        ಆದರೆ (this == &other) {
            ಹಿಂತಿರುಗಿ *this;
        }
        ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ಆದರೆ (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } ಇಲ್ಲದಿದ್ದರೆ {
                // do nothing
            }
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    ~PluralMap() {
        ಗಾಗಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    ಶೂನ್ಯ clear() {
        *fVariants[0] = T();
        ಗಾಗಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ಸ್ಥಿರಾಂಕ T *ಮುಂದಿನ(Category &ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
        int32_t idx = ಸೂಚ್ಯಂಕ;
        ++idx;
        ಗಾಗಿ (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            ಆದರೆ (fVariants[idx] != nullptr) {
                ಸೂಚ್ಯಂಕ = static_cast<Category>(idx);
                ಹಿಂತಿರುಗಿ fVariants[idx];
            }
        }
        ಸೂಚ್ಯಂಕ = static_cast<Category>(idx);
        ಹಿಂತಿರುಗಿ nullptr;
    }

    /**
     * non const version of next.
     */
    T *ಮುಂದಿನ_Mutable(Category &ಸೂಚ್ಯಂಕ) {
        ಸ್ಥಿರಾಂಕ T *ಫಲಿತಾಂಶ = ಮುಂದಿನ(ಸೂಚ್ಯಂಕ);
        ಹಿಂತಿರುಗಿ const_cast<T *>(ಫಲಿತಾಂಶ);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ಸ್ಥಿರಾಂಕ T &getOther() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ಸ್ಥಿರಾಂಕ T &get(Category v) ಸ್ಥಿರಾಂಕ {
        int32_t ಸೂಚ್ಯಂಕ = v;
        ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(fVariants) || fVariants[ಸೂಚ್ಯಂಕ] == nullptr) {
            ಹಿಂತಿರುಗಿ *fVariants[0];
        }
        ಹಿಂತಿರುಗಿ *fVariants[ಸೂಚ್ಯಂಕ];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ಸ್ಥಿರಾಂಕ T &get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *category) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ get(ಗಮ್ಯ_Category(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ಸ್ಥಿರಾಂಕ T &get(ಸ್ಥಿರಾಂಕ UnicodeString &category) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ get(ಗಮ್ಯ_Category(category));
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
            UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಹಿಂತಿರುಗಿ getMutable(category, nullptr, ಸ್ಥಿತಿ_2);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *category,
            UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಹಿಂತಿರುಗಿ getMutable(ಗಮ್ಯ_Category(category), nullptr, ಸ್ಥಿತಿ_2);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ಸ್ಥಿರಾಂಕ T &default_ಮೌಲ್ಯ,
            UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಹಿಂತಿರುಗಿ getMutable(category, &default_ಮೌಲ್ಯ, ಸ್ಥಿತಿ_2);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ಸ್ಥಿರಾಂಕ PluralMap<T> &rhs,
            UBool (*eqFunc)(ಸ್ಥಿರಾಂಕ T &, ಸ್ಥಿರಾಂಕ T &)) ಸ್ಥಿರಾಂಕ {
        ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ಆದರೆ (fVariants[i] == rhs.fVariants[i]) {
                ಮುಂದುವರಿ;
            }
            ಆದರೆ (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                ಹಿಂತಿರುಗಿ false;
            }
            ಆದರೆ (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                ಹಿಂತಿರುಗಿ false;
            }
        }
        ಹಿಂತಿರುಗಿ true;
    }

ಖಾಸಗಿ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ಸ್ಥಿರಾಂಕ T *default_ಮೌಲ್ಯ,
            UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ nullptr;
        }
        int32_t ಸೂಚ್ಯಂಕ = category;
        ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(fVariants)) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ nullptr;
        }
        ಆದರೆ (fVariants[ಸೂಚ್ಯಂಕ] == nullptr) {
            fVariants[ಸೂಚ್ಯಂಕ] = default_ಮೌಲ್ಯ == nullptr ?
                    new T() : new T(*default_ಮೌಲ್ಯ);
        }
        ಆದರೆ (!fVariants[ಸೂಚ್ಯಂಕ]) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಹಿಂತಿರುಗಿ fVariants[ಸೂಚ್ಯಂಕ];
    }

    ಶೂನ್ಯ initializeNew() {
        fVariants[0] = &fOtherVariant;
        ಗಾಗಿ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
