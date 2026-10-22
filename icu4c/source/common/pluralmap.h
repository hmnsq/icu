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

#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : പൊതു UMemory {
പൊതു:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    എണ്ണൽ Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_എണ്ണം
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    സ്ഥിതം Category toCategory(സ്ഥിരം അക്ഷരം *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    സ്ഥിതം Category toCategory(സ്ഥിരം UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    സ്ഥിതം സ്ഥിരം അക്ഷരം *getCategoryName(Category category);
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
class PluralMap : പൊതു PluralMapBase {
പൊതു:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(സ്ഥിരം T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(സ്ഥിരം PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        വേണ്ടി (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &operator=(സ്ഥിരം PluralMap<T> &other) {
        എങ്കിൽ (this == &other) {
            തിരികെ *this;
        }
        വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            എങ്കിൽ (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } അല്ലെങ്കിൽ എങ്കിൽ (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } അല്ലെങ്കിൽ എങ്കിൽ (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } അല്ലെങ്കിൽ {
                // do nothing
            }
        }
        തിരികെ *this;
    }

    ~PluralMap() {
        വേണ്ടി (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    ശൂന്യം clear() {
        *fVariants[0] = T();
        വേണ്ടി (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    സ്ഥിരം T *next(Category &index) സ്ഥിരം {
        int32_t idx = index;
        ++idx;
        വേണ്ടി (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            എങ്കിൽ (fVariants[idx] != nullptr) {
                index = static_cast<Category>(idx);
                തിരികെ fVariants[idx];
            }
        }
        index = static_cast<Category>(idx);
        തിരികെ nullptr;
    }

    /**
     * non const version of next.
     */
    T *nextMutable(Category &index) {
        സ്ഥിരം T *result = next(index);
        തിരികെ const_cast<T *>(result);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    സ്ഥിരം T &getOther() സ്ഥിരം {
        തിരികെ get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    സ്ഥിരം T &get(Category v) സ്ഥിരം {
        int32_t index = v;
        എങ്കിൽ (index < 0 || index >= UPRV_LENGTHOF(fVariants) || fVariants[index] == nullptr) {
            തിരികെ *fVariants[0];
        }
        തിരികെ *fVariants[index];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    സ്ഥിരം T &get(സ്ഥിരം അക്ഷരം *category) സ്ഥിരം {
        തിരികെ get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    സ്ഥിരം T &get(സ്ഥിരം UnicodeString &category) സ്ഥിരം {
        തിരികെ get(toCategory(category));
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
        തിരികെ getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            സ്ഥിരം അക്ഷരം *category,
            UErrorCode &status) {
        തിരികെ getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            സ്ഥിരം T &default_മൂല്യം,
            UErrorCode &status) {
        തിരികെ getMutable(category, &default_മൂല്യം, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            സ്ഥിരം PluralMap<T> &rhs,
            UBool (*eqFunc)(സ്ഥിരം T &, സ്ഥിരം T &)) സ്ഥിരം {
        വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            എങ്കിൽ (fVariants[i] == rhs.fVariants[i]) {
                തുടരുക;
            }
            എങ്കിൽ (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                തിരികെ false;
            }
            എങ്കിൽ (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                തിരികെ false;
            }
        }
        തിരികെ true;
    }

സ്വകാര്യ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            സ്ഥിരം T *default_മൂല്യം,
            UErrorCode &status) {
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ nullptr;
        }
        int32_t index = category;
        എങ്കിൽ (index < 0 || index >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ nullptr;
        }
        എങ്കിൽ (fVariants[index] == nullptr) {
            fVariants[index] = default_മൂല്യം == nullptr ?
                    new T() : new T(*default_മൂല്യം);
        }
        എങ്കിൽ (!fVariants[index]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        തിരികെ fVariants[index];
    }

    ശൂന്യം initializeNew() {
        fVariants[0] = &fOtherVariant;
        വേണ്ടി (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#endif
