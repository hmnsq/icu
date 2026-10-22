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

#اگر_غیر_تعریف_شدہ __PLURAL_MAP_H__
#تعریف_کرو __PLURAL_MAP_H__

#شامل "unicode/uobject.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API Plural_Map_بنیاد : عوامی UMemory {
عوامی:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    شمار Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_گنتی
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    ساکن Category مقصد_Category(ثابت حرف *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    ساکن Category مقصد_Category(ثابت UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    ساکن ثابت حرف *getCategoryName(Category category);
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
سانچہ<typename T>
class PluralMap : عوامی Plural_Map_بنیاد {
عوامی:
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
        برائے (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &عامل=(ثابت PluralMap<T> &other) {
        اگر (this == &other) {
            واپس *this;
        }
        برائے (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            اگر (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } ورنہ اگر (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } ورنہ اگر (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } ورنہ {
                // do nothing
            }
        }
        واپس *this;
    }

    ~PluralMap() {
        برائے (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    خالی clear() {
        *fVariants[0] = T();
        برائے (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    ثابت T *اگلا(Category &اشاریہ) ثابت {
        int32_t idx = اشاریہ;
        ++idx;
        برائے (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            اگر (fVariants[idx] != nullptr) {
                اشاریہ = static_cast<Category>(idx);
                واپس fVariants[idx];
            }
        }
        اشاریہ = static_cast<Category>(idx);
        واپس nullptr;
    }

    /**
     * non const version of next.
     */
    T *اگلا_Mutable(Category &اشاریہ) {
        ثابت T *نتیجہ = اگلا(اشاریہ);
        واپس const_cast<T *>(نتیجہ);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    ثابت T &getOther() ثابت {
        واپس get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    ثابت T &get(Category v) ثابت {
        int32_t اشاریہ = v;
        اگر (اشاریہ < 0 || اشاریہ >= UPRV_LENGTHOF(fVariants) || fVariants[اشاریہ] == nullptr) {
            واپس *fVariants[0];
        }
        واپس *fVariants[اشاریہ];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    ثابت T &get(ثابت حرف *category) ثابت {
        واپس get(مقصد_Category(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    ثابت T &get(ثابت UnicodeString &category) ثابت {
        واپس get(مقصد_Category(category));
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
            UError_رمز &حالت_2) {
        واپس getMutable(category, nullptr, حالت_2);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            ثابت حرف *category,
            UError_رمز &حالت_2) {
        واپس getMutable(مقصد_Category(category), nullptr, حالت_2);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            ثابت T &default_قدر,
            UError_رمز &حالت_2) {
        واپس getMutable(category, &default_قدر, حالت_2);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            ثابت PluralMap<T> &rhs,
            UBool (*eqFunc)(ثابت T &, ثابت T &)) ثابت {
        برائے (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            اگر (fVariants[i] == rhs.fVariants[i]) {
                جاری;
            }
            اگر (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                واپس false;
            }
            اگر (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                واپس false;
            }
        }
        واپس true;
    }

نجی:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            ثابت T *default_قدر,
            UError_رمز &حالت_2) {
        اگر (U_FAILURE(حالت_2)) {
            واپس nullptr;
        }
        int32_t اشاریہ = category;
        اگر (اشاریہ < 0 || اشاریہ >= UPRV_LENGTHOF(fVariants)) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس nullptr;
        }
        اگر (fVariants[اشاریہ] == nullptr) {
            fVariants[اشاریہ] = default_قدر == nullptr ?
                    new T() : new T(*default_قدر);
        }
        اگر (!fVariants[اشاریہ]) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        واپس fVariants[اشاریہ];
    }

    خالی initializeNew() {
        fVariants[0] = &fOtherVariant;
        برائے (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_انتہا

#اگر_ختم
