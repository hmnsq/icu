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

#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : જાહેર UMemory {
જાહેર:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    ગણના Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_ગણતરી
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    સ્થિર Category toCategory(અચલ અક્ષર *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    સ્થિર Category toCategory(અચલ UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    સ્થિર અચલ અક્ષર *getCategoryName(Category category);
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
class PluralMap : જાહેર PluralMapBase {
જાહેર:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(અચલ T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(અચલ PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        માટે (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(અચલ PluralMap<T> &other) {
        જો (this == &other) {
            પાછા *this;
        }
        માટે (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            જો (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } નહીંતર જો (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } નહીંતર જો (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } નહીંતર {
                // do nothing
            }
        }
        પાછા *this;
    }

    ~PluralMap() {
        માટે (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    શૂન્ય clear() {
        *fVariants[0] = T();
        માટે (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    અચલ T *next(Category &index) અચલ {
        int32_t idx = index;
        ++idx;
        માટે (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            જો (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                પાછા fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        પાછા nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        અચલ T *result = next(index);
        પાછા const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    અચલ T &getOther() અચલ {
        પાછા get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    અચલ T &get(Category v) અચલ {
        int32_t index = v;
        જો (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            પાછા *fVariants[0];
        }
        પાછા *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    અચલ T &get(અચલ અક્ષર *category) અચલ {
        પાછા get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    અચલ T &get(અચલ UnicodeString &category) અચલ {
        પાછા get(toCategory(category));
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
        પાછા getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            અચલ અક્ષર *category,
            UErrorCode &status) {
        પાછા getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            અચલ T &default_કિંમત,
            UErrorCode &status) {
        પાછા getMutable(category, &default_કિંમત, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            અચલ PluralMap<T> &rhs,
            UBool (*eqFunc)(અચલ T &, અચલ T &)) અચલ {
        માટે (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            જો (fVariants[i] == rhs.fVariants[i]) {
                ચાલુ;
            }
            જો (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                પાછા false;
            }
            જો (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                પાછા false;
            }
        }
        પાછા true;
    }

ખાનગી:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            અચલ T *default_કિંમત,
            UErrorCode &status) {
        જો (U_FAILURE(status)) {
            પાછા nullptr;
        }
        int32_t index = category;
        જો (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા nullptr;
        }
        જો (fVariants[index] == nullptr) {
            fVariants[index] = default_કિંમત == nullptr ?
                    new T() : new T(*default_કિંમત);
        }
        જો (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        પાછા fVariants[index];
    }

    શૂન્ય initializeNew() {
        fVariants[0] = &fOtherVariant;
        માટે (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
