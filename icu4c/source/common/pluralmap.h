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

#अगर_अघोषित __PLURAL_MAP_H__
#घोषणा __PLURAL_MAP_H__

#समावेश "unicode/uobject.h"
#समावेश "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API PluralMapBase : खुला UMemory {
खुला:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    क्रमागत Category {
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
    अटल Category toCategory(स्थिर अक्षर *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    अटल Category toCategory(स्थिर UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    अटल स्थिर अक्षर *getCategoryName(Category category);
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
class PluralMap : खुला PluralMapBase {
खुला:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(स्थिर T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(स्थिर PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        क्रम (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &चालक=(स्थिर PluralMap<T> &other) {
        अगर (this == &other) {
            वापस *this;
        }
        क्रम (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            अगर (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } अन्यथा अगर (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } अन्यथा अगर (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } अन्यथा {
                // do nothing
            }
        }
        वापस *this;
    }

    ~PluralMap() {
        क्रम (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    व्योम clear() {
        *fVariants[0] = T();
        क्रम (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    स्थिर T *अग्र(Category &सूचकांक) स्थिर {
        int32_t idx = सूचकांक;
        ++idx;
        क्रम (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            अगर (fVariants[idx] != nullptr) {
                सूचकांक = static_cast<Category>(idx);
                वापस fVariants[idx];
            }
        }
        सूचकांक = static_cast<Category>(idx);
        वापस nullptr;
    }

    /**
     * non const version of next.
     */
    T *अग्र_Mutable(Category &सूचकांक) {
        स्थिर T *परिणाम = अग्र(सूचकांक);
        वापस const_cast<T *>(परिणाम);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    स्थिर T &getOther() स्थिर {
        वापस get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    स्थिर T &get(Category v) स्थिर {
        int32_t सूचकांक = v;
        अगर (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(fVariants) || fVariants[सूचकांक] == nullptr) {
            वापस *fVariants[0];
        }
        वापस *fVariants[सूचकांक];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    स्थिर T &get(स्थिर अक्षर *category) स्थिर {
        वापस get(toCategory(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    स्थिर T &get(स्थिर UnicodeString &category) स्थिर {
        वापस get(toCategory(category));
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
        वापस getMutable(category, nullptr, status);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            स्थिर अक्षर *category,
            UErrorCode &status) {
        वापस getMutable(toCategory(category), nullptr, status);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            स्थिर T &default_मूल्य,
            UErrorCode &status) {
        वापस getMutable(category, &default_मूल्य, status);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            स्थिर PluralMap<T> &rhs,
            UBool (*eqFunc)(स्थिर T &, स्थिर T &)) स्थिर {
        क्रम (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            अगर (fVariants[i] == rhs.fVariants[i]) {
                जारी;
            }
            अगर (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                वापस false;
            }
            अगर (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                वापस false;
            }
        }
        वापस true;
    }

निजी:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            स्थिर T *default_मूल्य,
            UErrorCode &status) {
        अगर (U_FAILURE(status)) {
            वापस nullptr;
        }
        int32_t सूचकांक = category;
        अगर (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(fVariants)) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस nullptr;
        }
        अगर (fVariants[सूचकांक] == nullptr) {
            fVariants[सूचकांक] = default_मूल्य == nullptr ?
                    new T() : new T(*default_मूल्य);
        }
        अगर (!fVariants[सूचकांक]) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        वापस fVariants[सूचकांक];
    }

    व्योम initializeNew() {
        fVariants[0] = &fOtherVariant;
        क्रम (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_END

#पूर्ण_अगर
