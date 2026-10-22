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

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : সর্বজনীন UMemory {
সর্বজনীন:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    গণনা Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_গুনতি
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    স্থির Category toCategory(ধ্রুবক অক্ষর *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    স্থির Category toCategory(ধ্রুবক UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    স্থির ধ্রুবক অক্ষর *getCategoryName(Category category);
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
class PluralMap : সর্বজনীন PluralMapBase {
সর্বজনীন:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ধ্রুবক T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ধ্রুবক PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        জন্য (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(ধ্রুবক PluralMap<T> &other) {
        যদি (this == &other) {
            ফেরত *this;
        }
        জন্য (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            যদি (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } অন্যথা যদি (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } অন্যথা যদি (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } অন্যথা {
                // do nothing
            }
        }
        ফেরত *this;
    }

    ~PluralMap() {
        জন্য (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    শূন্য clear() {
        *fVariants[0] = T();
        জন্য (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ধ্রুবক T *পরবর্তী(Category &সূচক) ধ্রুবক {
        int32_t idx = সূচক;
        ++idx;
        জন্য (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            যদি (fVariants[idx] != nullptr) {
                সূচক = static_cast<Category>(idx);
                ফেরত fVariants[idx];
            }
        }
        সূচক = static_cast<Category>(idx);
        ফেরত nullptr;
    }

    /**
     * non const version of next.
     */
    T *পরবর্তী_Mutable(Category &সূচক) {
        ধ্রুবক T *ফলাফল = পরবর্তী(সূচক);
        ফেরত const_cast<T *>(ফলাফল);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ধ্রুবক T &getOther() ধ্রুবক {
        ফেরত get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ধ্রুবক T &get(Category v) ধ্রুবক {
        int32_t সূচক = v;
        যদি (সূচক < 0 || সূচক >= UPRV_LENGTHOF(fVariants) || fVariants[সূচক] == nullptr) {
            ফেরত *fVariants[0];
        }
        ফেরত *fVariants[সূচক];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ধ্রুবক T &get(ধ্রুবক অক্ষর *category) ধ্রুবক {
        ফেরত get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ধ্রুবক T &get(ধ্রুবক UnicodeString &category) ধ্রুবক {
        ফেরত get(toCategory(category));
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
        ফেরত getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ধ্রুবক অক্ষর *category,
            UErrorCode &status) {
        ফেরত getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ধ্রুবক T &default_মান,
            UErrorCode &status) {
        ফেরত getMutable(category, &default_মান, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ধ্রুবক PluralMap<T> &rhs,
            UBool (*eqFunc)(ধ্রুবক T &, ধ্রুবক T &)) ধ্রুবক {
        জন্য (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            যদি (fVariants[i] == rhs.fVariants[i]) {
                চালু;
            }
            যদি (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                ফেরত false;
            }
            যদি (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                ফেরত false;
            }
        }
        ফেরত true;
    }

ব্যক্তিগত:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ধ্রুবক T *default_মান,
            UErrorCode &status) {
        যদি (U_FAILURE(status)) {
            ফেরত nullptr;
        }
        int32_t সূচক = category;
        যদি (সূচক < 0 || সূচক >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত nullptr;
        }
        যদি (fVariants[সূচক] == nullptr) {
            fVariants[সূচক] = default_মান == nullptr ?
                    new T() : new T(*default_মান);
        }
        যদি (!fVariants[সূচক]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ফেরত fVariants[সূচক];
    }

    শূন্য initializeNew() {
        fVariants[0] = &fOtherVariant;
        জন্য (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
