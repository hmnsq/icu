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

#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : பொது UMemory {
பொது:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    எண்ணிக்கை Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_மொத்தம்
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    நிலையான Category toCategory(மாறிலி எழுத்து *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    நிலையான Category toCategory(மாறிலி UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    நிலையான மாறிலி எழுத்து *getCategoryName(Category category);
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
class PluralMap : பொது PluralMapBase {
பொது:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(மாறிலி T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(மாறிலி PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        க்கு (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(மாறிலி PluralMap<T> &other) {
        எனில் (this == &other) {
            திரும்பு *this;
        }
        க்கு (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            எனில் (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } இல்லையெனில் எனில் (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } இல்லையெனில் எனில் (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } இல்லையெனில் {
                // do nothing
            }
        }
        திரும்பு *this;
    }

    ~PluralMap() {
        க்கு (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    வெறுமை clear() {
        *fVariants[0] = T();
        க்கு (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    மாறிலி T *next(Category &index) மாறிலி {
        int32_t idx = index;
        ++idx;
        க்கு (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            எனில் (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                திரும்பு fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        திரும்பு nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        மாறிலி T *result = next(index);
        திரும்பு const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    மாறிலி T &getOther() மாறிலி {
        திரும்பு get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    மாறிலி T &get(Category v) மாறிலி {
        int32_t index = v;
        எனில் (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            திரும்பு *fVariants[0];
        }
        திரும்பு *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    மாறிலி T &get(மாறிலி எழுத்து *category) மாறிலி {
        திரும்பு get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    மாறிலி T &get(மாறிலி UnicodeString &category) மாறிலி {
        திரும்பு get(toCategory(category));
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
        திரும்பு getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            மாறிலி எழுத்து *category,
            UErrorCode &status) {
        திரும்பு getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            மாறிலி T &default_மதிப்பு,
            UErrorCode &status) {
        திரும்பு getMutable(category, &default_மதிப்பு, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            மாறிலி PluralMap<T> &rhs,
            UBool (*eqFunc)(மாறிலி T &, மாறிலி T &)) மாறிலி {
        க்கு (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            எனில் (fVariants[i] == rhs.fVariants[i]) {
                தொடர்;
            }
            எனில் (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                திரும்பு false;
            }
            எனில் (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                திரும்பு false;
            }
        }
        திரும்பு true;
    }

தனிப்பட்ட:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            மாறிலி T *default_மதிப்பு,
            UErrorCode &status) {
        எனில் (U_FAILURE(status)) {
            திரும்பு nullptr;
        }
        int32_t index = category;
        எனில் (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு nullptr;
        }
        எனில் (fVariants[index] == nullptr) {
            fVariants[index] = default_மதிப்பு == nullptr ?
                    new T() : new T(*default_மதிப்பு);
        }
        எனில் (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        திரும்பு fVariants[index];
    }

    வெறுமை initializeNew() {
        fVariants[0] = &fOtherVariant;
        க்கு (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
