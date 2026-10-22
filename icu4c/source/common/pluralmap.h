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

#جيڪڏهن_وصف_نه_ٿيل __PLURAL_MAP_H__
#وصف_ڪريو __PLURAL_MAP_H__

#ضمن "unicode/uobject.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API Plural_Map_بنياد : عام UMemory {
عام:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    تعداد Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_ڳڻپ
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ساكن Category منزل_Category(ثابت حرف *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ساكن Category منزل_Category(ثابت UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ساكن ثابت حرف *getCategoryName(Category category);
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
سانچو<typename T>
class PluralMap : عام Plural_Map_بنياد {
عام:
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
        لكل (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &عامل=(ثابت PluralMap<T> &other) {
        إذا (this == &other) {
            أرجع *this;
        }
        لكل (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            إذا (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } وإلا إذا (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } وإلا إذا (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } وإلا {
                // do nothing
            }
        }
        أرجع *this;
    }

    ~PluralMap() {
        لكل (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    فراغ clear() {
        *fVariants[0] = T();
        لكل (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ثابت T *ايندڙ(Category &اشاريو) ثابت {
        int32_t idx = اشاريو;
        ++idx;
        لكل (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            إذا (fVariants[idx] != nullptr) {
                اشاريو = static_cast<Category>(idx);
                أرجع fVariants[idx];
            }
        }
        اشاريو = static_cast<Category>(idx);
        أرجع nullptr;
    }

    /**
     * non const version of next.
     */
    T *ايندڙ_Mutable(Category &اشاريو) {
        ثابت T *نتيجو = ايندڙ(اشاريو);
        أرجع const_cast<T *>(نتيجو);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ثابت T &getOther() ثابت {
        أرجع get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ثابت T &get(Category v) ثابت {
        int32_t اشاريو = v;
        إذا (اشاريو < 0 || اشاريو >= UPRV_LENGTHOF(fVariants) || fVariants[اشاريو] == nullptr) {
            أرجع *fVariants[0];
        }
        أرجع *fVariants[اشاريو];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ثابت T &get(ثابت حرف *category) ثابت {
        أرجع get(منزل_Category(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ثابت T &get(ثابت UnicodeString &category) ثابت {
        أرجع get(منزل_Category(category));
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
            UError_ڪوڊ &حالت) {
        أرجع getMutable(category, nullptr, حالت);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ثابت حرف *category,
            UError_ڪوڊ &حالت) {
        أرجع getMutable(منزل_Category(category), nullptr, حالت);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ثابت T &default_قدر,
            UError_ڪوڊ &حالت) {
        أرجع getMutable(category, &default_قدر, حالت);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ثابت PluralMap<T> &rhs,
            UBool (*eqFunc)(ثابت T &, ثابت T &)) ثابت {
        لكل (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            إذا (fVariants[i] == rhs.fVariants[i]) {
                استمر;
            }
            إذا (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                أرجع false;
            }
            إذا (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                أرجع false;
            }
        }
        أرجع true;
    }

خاص:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ثابت T *default_قدر,
            UError_ڪوڊ &حالت) {
        إذا (U_FAILURE(حالت)) {
            أرجع nullptr;
        }
        int32_t اشاريو = category;
        إذا (اشاريو < 0 || اشاريو >= UPRV_LENGTHOF(fVariants)) {
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع nullptr;
        }
        إذا (fVariants[اشاريو] == nullptr) {
            fVariants[اشاريو] = default_قدر == nullptr ?
                    new T() : new T(*default_قدر);
        }
        إذا (!fVariants[اشاريو]) {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
        أرجع fVariants[اشاريو];
    }

    فراغ initializeNew() {
        fVariants[0] = &fOtherVariant;
        لكل (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
