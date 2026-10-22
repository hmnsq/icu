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

#include "unicode/uobject.h"
#include "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : public UMemory {
public:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    enum Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_COUNT
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    static Category toCategory(const char *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    static Category toCategory(const UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    static const char *getCategoryName(Category category);
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
class PluralMap : public PluralMapBase {
public:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(const T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(const PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        ために (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(const PluralMap<T> &other) {
        もし (this == &other) {
            戻る *this;
        }
        ために (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            もし (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } 否则 もし (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } 否则 もし (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } 否则 {
                // do nothing
            }
        }
        戻る *this;
    }

    ~PluralMap() {
        ために (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    void clear() {
        *fVariants[0] = T();
        ために (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    const T *next(Category &index) const {
        int32_t idx = index;
        ++idx;
        ために (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            もし (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                戻る fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        戻る nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        const T *result = next(index);
        戻る const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    const T &getOther() const {
        戻る get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    const T &get(Category v) const {
        int32_t index = v;
        もし (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            戻る *fVariants[0];
        }
        戻る *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    const T &get(const char *category) const {
        戻る get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    const T &get(const UnicodeString &category) const {
        戻る get(toCategory(category));
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
        戻る getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            const char *category,
            UErrorCode &status) {
        戻る getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            const T &defaultValue,
            UErrorCode &status) {
        戻る getMutable(category, &defaultValue, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            const PluralMap<T> &rhs,
            UBool (*eqFunc)(const T &, const T &)) const {
        ために (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            もし (fVariants[i] == rhs.fVariants[i]) {
                continue;
            }
            もし (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                戻る false;
            }
            もし (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                戻る false;
            }
        }
        戻る true;
    }

private:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            const T *defaultValue,
            UErrorCode &status) {
        もし (U_FAILURE(status)) {
            戻る nullptr;
        }
        int32_t index = category;
        もし (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            戻る nullptr;
        }
        もし (fVariants[index] == nullptr) {
            fVariants[index] = defaultValue == nullptr ?
                    new T() : new T(*defaultValue);
        }
        もし (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        戻る fVariants[index];
    }

    void initializeNew() {
        fVariants[0] = &fOtherVariant;
        ために (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
