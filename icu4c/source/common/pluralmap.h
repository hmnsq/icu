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

#شامل "unicode/uobject.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : عمومی UMemory {
عمومی:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    شمارش Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_شمار
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ایستا Category toCategory(ثابت نویسه *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ایستا Category toCategory(ثابت UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ایستا ثابت نویسه *getCategoryName(Category category);
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
class PluralMap : عمومی PluralMapBase {
عمومی:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ثابت T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ثابت PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        برای (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(ثابت PluralMap<T> &other) {
        اگر (this == &other) {
            بازگشت *this;
        }
        برای (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            اگر (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } وگرنه اگر (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } وگرنه اگر (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } وگرنه {
                // do nothing
            }
        }
        بازگشت *this;
    }

    ~PluralMap() {
        برای (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    تهی clear() {
        *fVariants[0] = T();
        برای (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ثابت T *next(Category &index) ثابت {
        int32_t idx = index;
        ++idx;
        برای (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            اگر (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                بازگشت fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        بازگشت nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        ثابت T *result = next(index);
        بازگشت const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ثابت T &getOther() ثابت {
        بازگشت get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ثابت T &get(Category v) ثابت {
        int32_t index = v;
        اگر (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            بازگشت *fVariants[0];
        }
        بازگشت *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ثابت T &get(ثابت نویسه *category) ثابت {
        بازگشت get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ثابت T &get(ثابت UnicodeString &category) ثابت {
        بازگشت get(toCategory(category));
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
        بازگشت getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ثابت نویسه *category,
            UErrorCode &status) {
        بازگشت getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ثابت T &default_مقدار,
            UErrorCode &status) {
        بازگشت getMutable(category, &default_مقدار, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ثابت PluralMap<T> &rhs,
            UBool (*eqFunc)(ثابت T &, ثابت T &)) ثابت {
        برای (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            اگر (fVariants[i] == rhs.fVariants[i]) {
                ادامه;
            }
            اگر (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                بازگشت false;
            }
            اگر (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                بازگشت false;
            }
        }
        بازگشت true;
    }

خصوصی:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ثابت T *default_مقدار,
            UErrorCode &status) {
        اگر (U_FAILURE(status)) {
            بازگشت nullptr;
        }
        int32_t index = category;
        اگر (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت nullptr;
        }
        اگر (fVariants[index] == nullptr) {
            fVariants[index] = default_مقدار == nullptr ?
                    new T() : new T(*default_مقدار);
        }
        اگر (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        بازگشت fVariants[index];
    }

    تهی initializeNew() {
        fVariants[0] = &fOtherVariant;
        برای (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
