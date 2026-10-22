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
        CATEGORY_गन्ती
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    स्थिर Category toCategory(अचर वर्ण *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    स्थिर Category toCategory(अचर UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    स्थिर अचर वर्ण *getCategoryName(Category category);
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
ढाँचा<typename T>
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
    PluralMap(अचर T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(अचर PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        लागि (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &चालक=(अचर PluralMap<T> &other) {
        यदि (this == &other) {
            फर्काऊ *this;
        }
        लागि (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
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
        फर्काऊ *this;
    }

    ~PluralMap() {
        लागि (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    शून्य clear() {
        *fVariants[0] = T();
        लागि (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    अचर T *next(Category &index) अचर {
        int32_t idx = index;
        ++idx;
        लागि (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            यदि (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                फर्काऊ fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        फर्काऊ nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        अचर T *result = next(index);
        फर्काऊ const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    अचर T &getOther() अचर {
        फर्काऊ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    अचर T &get(Category v) अचर {
        int32_t index = v;
        यदि (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            फर्काऊ *fVariants[0];
        }
        फर्काऊ *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    अचर T &get(अचर वर्ण *category) अचर {
        फर्काऊ get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    अचर T &get(अचर UnicodeString &category) अचर {
        फर्काऊ get(toCategory(category));
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
        फर्काऊ getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            अचर वर्ण *category,
            UErrorCode &status) {
        फर्काऊ getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            अचर T &default_मान,
            UErrorCode &status) {
        फर्काऊ getMutable(category, &default_मान, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            अचर PluralMap<T> &rhs,
            UBool (*eqFunc)(अचर T &, अचर T &)) अचर {
        लागि (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            यदि (fVariants[i] == rhs.fVariants[i]) {
                जारी;
            }
            यदि (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                फर्काऊ false;
            }
            यदि (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                फर्काऊ false;
            }
        }
        फर्काऊ true;
    }

गुप्त:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            अचर T *default_मान,
            UErrorCode &status) {
        यदि (U_FAILURE(status)) {
            फर्काऊ nullptr;
        }
        int32_t index = category;
        यदि (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ nullptr;
        }
        यदि (fVariants[index] == nullptr) {
            fVariants[index] = default_मान == nullptr ?
                    new T() : new T(*default_मान);
        }
        यदि (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        फर्काऊ fVariants[index];
    }

    शून्य initializeNew() {
        fVariants[0] = &fOtherVariant;
        लागि (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
