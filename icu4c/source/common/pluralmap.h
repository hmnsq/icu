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

#నిర్వచితం_కాకపోతే __PLURAL_MAP_H__
#నిర్వచించు __PLURAL_MAP_H__

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

class UnicodeString;

class U_COMMON_API Plural_Map_పునాది : బహిరంగ UMemory {
బహిరంగ:
    /**
     * The names of all the plural categories. NONE is not an actual plural
     * category, but rather represents the absence of a plural category.
     */
    గణన Category {
        NONE = -1,
        OTHER,
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        CATEGORY_లెక్క
    };

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum. Returns NONE for an unrecognized
     * category name.
     */
    స్థిర Category గమ్యం_Category(స్థిరరాశి అక్షరం *categoryName);

    /**
     * Converts a category name such as "zero", "one", "two", "few", "many"
     * or "other" to a category enum.  Returns NONE for unrecognized
     * category name.
     */
    స్థిర Category గమ్యం_Category(స్థిరరాశి UnicodeString &categoryName);

    /**
     * Converts a category to a name.
     * Passing NONE or CATEGORY_COUNT for category returns nullptr.
     */
    స్థిర స్థిరరాశి అక్షరం *getCategoryName(Category category);
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
మూస<typename T>
class PluralMap : బహిరంగ Plural_Map_పునాది {
బహిరంగ:
    /**
     * Other category is maps to a copy of the default value.
     */
    PluralMap() : fOtherVariant() {
        initializeNew();
    }

    /**
     * Other category is mapped to otherVariant.
     */
    PluralMap(స్థిరరాశి T &otherVariant) : fOtherVariant(otherVariant) {
        initializeNew();
    }

    PluralMap(స్థిరరాశి PluralMap<T> &other) : fOtherVariant(other.fOtherVariant) {
        fVariants[0] = &fOtherVariant;
        కొరకు (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = other.fVariants[i] ?
                    new T(*other.fVariants[i]) : nullptr;
        }
    }

    PluralMap<T> &క్రియాకారకం=(స్థిరరాశి PluralMap<T> &other) {
        ఒకవేళ (this == &other) {
            తిరిగి *this;
        }
        కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ఒకవేళ (fVariants[i] != nullptr && other.fVariants[i] != nullptr) {
                *fVariants[i] = *other.fVariants[i];
            } లేకపోతే ఒకవేళ (fVariants[i] != nullptr) {
                delete fVariants[i];
                fVariants[i] = nullptr;
            } లేకపోతే ఒకవేళ (other.fVariants[i] != nullptr) {
                fVariants[i] = new T(*other.fVariants[i]);
            } లేకపోతే {
                // do nothing
            }
        }
        తిరిగి *this;
    }

    ~PluralMap() {
        కొరకు (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            delete fVariants[i];
        }
    }

    /**
     * Removes all mappings and makes 'other' point to the default value.
     */
    శూన్య clear() {
        *fVariants[0] = T();
        కొరకు (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
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
    స్థిరరాశి T *తదుపరి(Category &సూచీ) స్థిరరాశి {
        int32_t idx = సూచీ;
        ++idx;
        కొరకు (; idx < UPRV_LENGTHOF(fVariants); ++idx) {
            ఒకవేళ (fVariants[idx] != nullptr) {
                సూచీ = static_cast<Category>(idx);
                తిరిగి fVariants[idx];
            }
        }
        సూచీ = static_cast<Category>(idx);
        తిరిగి nullptr;
    }

    /**
     * non const version of next.
     */
    T *తదుపరి_Mutable(Category &సూచీ) {
        స్థిరరాశి T *ఫలితం = తదుపరి(సూచీ);
        తిరిగి const_cast<T *>(ఫలితం);
    }

    /**
     * Returns the 'other' variant.
     * Same as calling get(OTHER).
     */
    స్థిరరాశి T &getOther() స్థిరరాశి {
        తిరిగి get(OTHER);
    }

    /**
     * Returns the value associated with a category.
     * If no value found, or v is NONE or CATEGORY_COUNT, falls
     * back to returning the value for the 'other' category.
     */
    స్థిరరాశి T &get(Category v) స్థిరరాశి {
        int32_t సూచీ = v;
        ఒకవేళ (సూచీ < 0 || సూచీ >= UPRV_LENGTHOF(fVariants) || fVariants[సూచీ] == nullptr) {
            తిరిగి *fVariants[0];
        }
        తిరిగి *fVariants[సూచీ];
    }

    /**
     * Convenience routine to get the value by category name. Otherwise
     * works just like get(Category).
     */
    స్థిరరాశి T &get(స్థిరరాశి అక్షరం *category) స్థిరరాశి {
        తిరిగి get(గమ్యం_Category(category));
    }

    /**
     * Convenience routine to get the value by category name as a
     * UnicodeString. Otherwise works just like get(category).
     */
    స్థిరరాశి T &get(స్థిరరాశి UnicodeString &category) స్థిరరాశి {
        తిరిగి get(గమ్యం_Category(category));
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
            UError_సంకేతం &స్థితి_2) {
        తిరిగి getMutable(category, nullptr, స్థితి_2);
    }

    /**
     * Convenience routine to get a mutable pointer to a value by category name.
     * Otherwise works just like getMutable(Category, UErrorCode &).
     * reports an error if the category name is invalid.
     */
    T *getMutable(
            స్థిరరాశి అక్షరం *category,
            UError_సంకేతం &స్థితి_2) {
        తిరిగి getMutable(గమ్యం_Category(category), nullptr, స్థితి_2);
    }

    /**
     * Just like getMutable(Category, UErrorCode &) but copies defaultValue to
     * returned pointer if it was defaulting to the 'other' variant
     * because no explicit value was stored.
     */
    T *getMutableWithDefault(
            Category category,
            స్థిరరాశి T &default_విలువ,
            UError_సంకేతం &స్థితి_2) {
        తిరిగి getMutable(category, &default_విలువ, స్థితి_2);
    }

    /**
     * Returns true if this object equals rhs.
     */
    UBool equals(
            స్థిరరాశి PluralMap<T> &rhs,
            UBool (*eqFunc)(స్థిరరాశి T &, స్థిరరాశి T &)) స్థిరరాశి {
        కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(fVariants); ++i) {
            ఒకవేళ (fVariants[i] == rhs.fVariants[i]) {
                కొనసాగు;
            }
            ఒకవేళ (fVariants[i] == nullptr || rhs.fVariants[i] == nullptr) {
                తిరిగి false;
            }
            ఒకవేళ (!eqFunc(*fVariants[i], *rhs.fVariants[i])) {
                తిరిగి false;
            }
        }
        తిరిగి true;
    }

అంతరంగ:
    T fOtherVariant;
    T* fVariants[6];

    T *getMutable(
            Category category,
            స్థిరరాశి T *default_విలువ,
            UError_సంకేతం &స్థితి_2) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి nullptr;
        }
        int32_t సూచీ = category;
        ఒకవేళ (సూచీ < 0 || సూచీ >= UPRV_LENGTHOF(fVariants)) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి nullptr;
        }
        ఒకవేళ (fVariants[సూచీ] == nullptr) {
            fVariants[సూచీ] = default_విలువ == nullptr ?
                    new T() : new T(*default_విలువ);
        }
        ఒకవేళ (!fVariants[సూచీ]) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        తిరిగి fVariants[సూచీ];
    }

    శూన్య initializeNew() {
        fVariants[0] = &fOtherVariant;
        కొరకు (int32_t i = 1; i < UPRV_LENGTHOF(fVariants); ++i) {
            fVariants[i] = nullptr;
        }
    }
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
