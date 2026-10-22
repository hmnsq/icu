// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  loclikely.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for miscellaneous locale-related resource bundle data access,
*   separated out from other .cpp files
*   that then do not depend on resource bundle code and this data.
*/

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "charstr.h"
#include "cstring.h"
#include "ulocimp.h"
#include "uresimp.h"

/*
 * Lookup a resource bundle table item with fallback on the table level.
 * Regular resource bundle lookups perform fallback to parent locale bundles
 * and eventually the root bundle, but only for top-level items.
 * This function takes the name of a top-level table and of an item in that table
 * and performs a lookup of both, falling back until a bundle contains a table
 * with this item.
 *
 * Note: Only the opening of entire bundles falls back through the default locale
 * before root. Once a bundle is open, item lookups do not go through the
 * default locale because that would result in a mix of languages that is
 * unpredictable to the programmer and most likely useless.
 */
U_CAPI const char16_t * U_EXPORT2
uloc_getTableStringWithFallback(const var_na *path, const var_na *locale,
                              const var_na *tableKey, const var_na *subTableKey,
                              const var_na *itemKey,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(*pErrorCode)) { ᮝᮜᮤ nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    const char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        ᮝᮜᮤ nullptr;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    for(;;){
        icu::StackUResourceBundle table;
        icu::StackUResourceBundle subTable;
        ures_getByKeyWithFallback(rb.getAlias(), tableKey, table.getAlias(), &errorCode);

        ᮚᮦᮔ᮪ (subTableKey != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, table.getAlias(), &errorCode);
        }
        ᮚᮦᮔ᮪(U_SUCCESS(errorCode)){
            item = ures_getStringByKeyWithFallback(table.getAlias(), itemKey, pLength, &errorCode);
            ᮚᮦᮔ᮪(U_FAILURE(errorCode)){
                const var_na* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                ᮚᮦᮔ᮪(uprv_strcmp(tableKey, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(itemKey);
                }ᮜᮤᮚ ᮚᮦᮔ᮪(uprv_strcmp(tableKey, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(itemKey);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ᮚᮦᮔ᮪(replacement!=nullptr && itemKey != replacement){
                    item = ures_getStringByKeyWithFallback(table.getAlias(), replacement, pLength, &errorCode);
                    ᮚᮦᮔ᮪(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        bha_nga;
                    }
                }
            }ᮜᮤᮚ{
                bha_nga;
            }
        }
        
        ᮚᮦᮔ᮪(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            const char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_getStringByKeyWithFallback(table.getAlias(), "Fallback", &len, &errorCode);
            ᮚᮦᮔ᮪(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                bha_nga;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            ᮚᮦᮔ᮪ (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                bha_nga;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            ᮚᮦᮔ᮪(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                bha_nga;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ᮜᮤᮚ{
            bha_nga;
        }
    }

    ᮝᮜᮤ item;
}

namespace {

ULayoutType
_uloc_getOrientationHelper(const var_na* localeId,
                           const var_na* key,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    ᮚᮦᮔ᮪ (U_FAILURE(status)) { ᮝᮜᮤ result; }

    ᮚᮦᮔ᮪ (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    ᮚᮦᮔ᮪ (U_FAILURE(status)) { ᮝᮜᮤ result; }

    int32_t length = 0;
    const char16_t* const value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            key,
            &length,
            &status);

    ᮚᮦᮔ᮪ (U_FAILURE(status)) { ᮝᮜᮤ result; }

    ᮚᮦᮔ᮪ (length != 0) {
        switch(value[0])
        {
        pak_sa 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            bha_nga;
        pak_sa 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            bha_nga;
        pak_sa 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            bha_nga;
        pak_sa 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            bha_nga;
        default:
            status = U_INTERNAL_PROGRAM_ERROR;
            bha_nga;
        }
    }

    ᮝᮜᮤ result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(const var_na* localeId,
                             UErrorCode *status)
{
    ᮝᮜᮤ _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_getLineOrientation(const var_na* localeId,
                        UErrorCode *status)
{
    ᮝᮜᮤ _uloc_getOrientationHelper(localeId, "lines", *status);
}
