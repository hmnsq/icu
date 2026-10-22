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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __PLURAL_MAP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __PLURAL_MAP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API Plural_Map_ਅਧਾਰ : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    ਗਿਣਤੀ Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_ਗਿਣਤੀ
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ਸਥਿਰ Category ਮੰਜ਼ਿਲ_Category(ਅਚਲ ਅੱਖਰ *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ਸਥਿਰ Category ਮੰਜ਼ਿਲ_Category(ਅਚਲ UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *getCategoryName(Category category);
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
ਸਾਂਚਾ<typename T>
class PluralMap : ਜਨਤਕ Plural_Map_ਅਧਾਰ {
ਜਨਤਕ:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ਅਚਲ T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ਅਚਲ PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        ਲਈ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &ਸੰਚਾਲਕ=(ਅਚਲ PluralMap<T> &other) {
        ਜੇ (this == &other) {
            ਵਾਪਸ *this;
        }
        ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ਜੇ (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } ਨਹੀਂਤਾਂ ਜੇ (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } ਨਹੀਂਤਾਂ ਜੇ (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } ਨਹੀਂਤਾਂ {
                // do nothing
            }
        }
        ਵਾਪਸ *this;
    }

    ~PluralMap() {
        ਲਈ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    ਖਾਲੀ clear() {
        *fVariants[0] = T();
        ਲਈ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ਅਚਲ T *ਅਗਲਾ(Category &ਸੂਚਕਾਂਕ) ਅਚਲ {
        int32_t idx = ਸੂਚਕਾਂਕ;
        ++idx;
        ਲਈ (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            ਜੇ (fVariants[idx] != nullptr) {
                ਸੂਚਕਾਂਕ = static_cast<Category>(idx);
                ਵਾਪਸ fVariants[idx];
            }
        }
        ਸੂਚਕਾਂਕ = static_cast<Category>(idx);
        ਵਾਪਸ nullptr;
    }

    /**
     * non const version of next.
     */
    T *ਅਗਲਾ_Mutable(Category &ਸੂਚਕਾਂਕ) {
        ਅਚਲ T *ਨਤੀਜਾ = ਅਗਲਾ(ਸੂਚਕਾਂਕ);
        ਵਾਪਸ const_cast<T *>(ਨਤੀਜਾ);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ਅਚਲ T &getOther() ਅਚਲ {
        ਵਾਪਸ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ਅਚਲ T &get(Category v) ਅਚਲ {
        int32_t ਸੂਚਕਾਂਕ = v;
        ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(fVariants) || fVariants[ਸੂਚਕਾਂਕ] == nullptr) {
            ਵਾਪਸ *fVariants[0];
        }
        ਵਾਪਸ *fVariants[ਸੂਚਕਾਂਕ];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ਅਚਲ T &get(ਅਚਲ ਅੱਖਰ *category) ਅਚਲ {
        ਵਾਪਸ get(ਮੰਜ਼ਿਲ_Category(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ਅਚਲ T &get(ਅਚਲ UnicodeString &category) ਅਚਲ {
        ਵਾਪਸ get(ਮੰਜ਼ਿਲ_Category(category));
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
            UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਵਾਪਸ getMutable(category, nullptr, ਸਥਿਤੀ);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ਅਚਲ ਅੱਖਰ *category,
            UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਵਾਪਸ getMutable(ਮੰਜ਼ਿਲ_Category(category), nullptr, ਸਥਿਤੀ);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ਅਚਲ T &default_ਮੁੱਲ,
            UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਵਾਪਸ getMutable(category, &default_ਮੁੱਲ, ਸਥਿਤੀ);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ਅਚਲ PluralMap<T> &rhs,
            UBool (*eqFunc)(ਅਚਲ T &, ਅਚਲ T &)) ਅਚਲ {
        ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ਜੇ (fVariants[i] == rhs.fVariants[i]) {
                ਜਾਰੀ;
            }
            ਜੇ (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                ਵਾਪਸ false;
            }
            ਜੇ (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                ਵਾਪਸ false;
            }
        }
        ਵਾਪਸ true;
    }

ਨਿਜੀ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ਅਚਲ T *default_ਮੁੱਲ,
            UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ nullptr;
        }
        int32_t ਸੂਚਕਾਂਕ = category;
        ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(fVariants)) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ nullptr;
        }
        ਜੇ (fVariants[ਸੂਚਕਾਂਕ] == nullptr) {
            fVariants[ਸੂਚਕਾਂਕ] = default_ਮੁੱਲ == nullptr ?
                    new T() : new T(*default_ਮੁੱਲ);
        }
        ਜੇ (!fVariants[ਸੂਚਕਾਂਕ]) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਵਾਪਸ fVariants[ਸੂਚਕਾਂਕ];
    }

    ਖਾਲੀ initializeNew() {
        fVariants[0] = &fOtherVariant;
        ਲਈ (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
