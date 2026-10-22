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

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : सार्वजनिक UMemory {
सार्वजनिक:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    गणना Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_गिनती
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    स्थिर Category toCategory(ध्रुव वर्ण *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    स्थिर Category toCategory(ध्रुव UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    स्थिर ध्रुव वर्ण *getCategoryName(Category category);
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
class PluralMap : सार्वजनिक PluralMapBase {
सार्वजनिक:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(ध्रुव T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(ध्रुव PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        प्रति (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(ध्रुव PluralMap<T> &other) {
        यदि (this == &other) {
            प्रत्यागम *this;
        }
        प्रति (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            यदि (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } अन्यथा यदि (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } अन्यथा यदि (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } अन्यथा {
                // do nothing
            }
        }
        प्रत्यागम *this;
    }

    ~PluralMap() {
        प्रति (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    शून्य clear() {
        *fVariants[0] = T();
        प्रति (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ध्रुव T *next(Category &index) ध्रुव {
        int32_t idx = index;
        ++idx;
        प्रति (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            यदि (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                प्रत्यागम fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        प्रत्यागम nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        ध्रुव T *result = next(index);
        प्रत्यागम const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ध्रुव T &getOther() ध्रुव {
        प्रत्यागम get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ध्रुव T &get(Category v) ध्रुव {
        int32_t index = v;
        यदि (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            प्रत्यागम *fVariants[0];
        }
        प्रत्यागम *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ध्रुव T &get(ध्रुव वर्ण *category) ध्रुव {
        प्रत्यागम get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ध्रुव T &get(ध्रुव UnicodeString &category) ध्रुव {
        प्रत्यागम get(toCategory(category));
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
        प्रत्यागम getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ध्रुव वर्ण *category,
            UErrorCode &status) {
        प्रत्यागम getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ध्रुव T &default_मूल्य,
            UErrorCode &status) {
        प्रत्यागम getMutable(category, &default_मूल्य, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ध्रुव PluralMap<T> &rhs,
            UBool (*eqFunc)(ध्रुव T &, ध्रुव T &)) ध्रुव {
        प्रति (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            यदि (fVariants[i] == rhs.fVariants[i]) {
                अनुवर्त;
            }
            यदि (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                प्रत्यागम false;
            }
            यदि (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                प्रत्यागम false;
            }
        }
        प्रत्यागम true;
    }

गुप्त:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ध्रुव T *default_मूल्य,
            UErrorCode &status) {
        यदि (U_FAILURE(status)) {
            प्रत्यागम nullptr;
        }
        int32_t index = category;
        यदि (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम nullptr;
        }
        यदि (fVariants[index] == nullptr) {
            fVariants[index] = default_मूल्य == nullptr ?
                    new T() : new T(*default_मूल्य);
        }
        यदि (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        प्रत्यागम fVariants[index];
    }

    शून्य initializeNew() {
        fVariants[0] = &fOtherVariant;
        प्रति (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
