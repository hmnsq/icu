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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

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
U_CAPI ಧ್ರುವ char16_t * U_EXPORT2
uloc_getTableStringWithFallback(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *locale,
                              ಧ್ರುವ ವರ್ಣ *table_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *sub_Table_ಕೀಲಿ,
                              ಧ್ರುವ ವರ್ಣ *item_ಕೀಲಿ,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    ಯದಿ (U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ಧ್ರುವ char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    ಯದಿ(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ ಯದಿ(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    ಪ್ರತಿ(;;){
        icu::ರಾಶಿ_UResource_Bundle table;
        icu::ರಾಶಿ_UResource_Bundle subTable;
        ures_get_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), table_ಕೀಲಿ, table.getAlias(), &errorCode);

        ಯದಿ (sub_Table_ಕೀಲಿ != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ಕೀಲಿ_With_Fallback(table.getAlias(), sub_Table_ಕೀಲಿ, table.getAlias(), &errorCode);
        }
        ಯದಿ(U_SUCCESS(errorCode)){
            item = ures_get_String_By_ಕೀಲಿ_With_Fallback(table.getAlias(), item_ಕೀಲಿ, pLength, &errorCode);
            ಯದಿ(U_FAILURE(errorCode)){
                ಧ್ರುವ ವರ್ಣ* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                ಯದಿ(uprv_strcmp(table_ಕೀಲಿ, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_ಕೀಲಿ);
                }ಅನ್ಯಥಾ ಯದಿ(uprv_strcmp(table_ಕೀಲಿ, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_ಕೀಲಿ);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ಯದಿ(replacement!=nullptr && item_ಕೀಲಿ != replacement){
                    item = ures_get_String_By_ಕೀಲಿ_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    ಯದಿ(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        ಭಙ್ಗ;
                    }
                }
            }ಅನ್ಯಥಾ{
                ಭಙ್ಗ;
            }
        }
        
        ಯದಿ(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ಧ್ರುವ char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ಕೀಲಿ_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            ಯದಿ(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                ಭಙ್ಗ;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            ಯದಿ (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                ಭಙ್ಗ;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            ಯದಿ(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                ಭಙ್ಗ;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ಅನ್ಯಥಾ{
            ಭಙ್ಗ;
        }
    }

    ಪ್ರತ್ಯಾಗಮ item;
}

ನಾಮಾಕಾಶ {

ULayoutType
_uloc_getOrientationHelper(ಧ್ರುವ ವರ್ಣ* localeId,
                           ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ result; }

    ಯದಿ (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ result; }

    int32_t length = 0;
    ಧ್ರುವ char16_t* ಧ್ರುವ value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            ಕೀಲಿ,
            &length,
            &status);

    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ result; }

    ಯದಿ (length != 0) {
        ವಿಕಲ್ಪ(value[0])
        {
        ಪಕ್ಷ 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            ಭಙ್ಗ;
        ಪಕ್ಷ 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            ಭಙ್ಗ;
        ಪಕ್ಷ 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            ಭಙ್ಗ;
        ಪಕ್ಷ 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            status = U_INTERNAL_PROGRAM_ERROR;
            ಭಙ್ಗ;
        }
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ಧ್ರುವ ವರ್ಣ* localeId,
                             UErrorCode *status)
{
    ಪ್ರತ್ಯಾಗಮ _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_ಸಾಲು_Orientation(ಧ್ರುವ ವರ್ಣ* localeId,
                        UErrorCode *status)
{
    ಪ್ರತ್ಯಾಗಮ _uloc_getOrientationHelper(localeId, "lines", *status);
}
