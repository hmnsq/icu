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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "uresimp.h"

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
U_CAPI ध्रुव char16_t * U_EXPORT2
uloc_getTableStringWithFallback(ध्रुव वर्ण *path, ध्रुव वर्ण *locale,
                              ध्रुव वर्ण *table_कुञ्चिका, ध्रुव वर्ण *sub_Table_कुञ्चिका,
                              ध्रुव वर्ण *item_कुञ्चिका,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    जइ (U_FAILURE(*pErrorCode)) { प्रत्यागम nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ध्रुव char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    जइ(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        प्रत्यागम nullptr;
    } अण्णहा जइ(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    प्रति(;;){
        icu::राशिः_UResource_Bundle table;
        icu::राशिः_UResource_Bundle subTable;
        ures_get_By_कुञ्चिका_With_Fallback(rb.getAlias(), table_कुञ्चिका, table.getAlias(), &errorCode);

        जइ (sub_Table_कुञ्चिका != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_कुञ्चिका_With_Fallback(table.getAlias(), sub_Table_कुञ्चिका, table.getAlias(), &errorCode);
        }
        जइ(U_SUCCESS(errorCode)){
            item = ures_get_String_By_कुञ्चिका_With_Fallback(table.getAlias(), item_कुञ्चिका, pLength, &errorCode);
            जइ(U_FAILURE(errorCode)){
                ध्रुव वर्ण* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                जइ(uprv_strcmp(table_कुञ्चिका, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_कुञ्चिका);
                }अण्णहा जइ(uprv_strcmp(table_कुञ्चिका, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_कुञ्चिका);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                जइ(replacement!=nullptr && item_कुञ्चिका != replacement){
                    item = ures_get_String_By_कुञ्चिका_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    जइ(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        भङ्ग;
                    }
                }
            }अण्णहा{
                भङ्ग;
            }
        }
        
        जइ(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ध्रुव char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_कुञ्चिका_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            जइ(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                भङ्ग;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            जइ (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                भङ्ग;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            जइ(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                भङ्ग;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }अण्णहा{
            भङ्ग;
        }
    }

    प्रत्यागम item;
}

नामाकाश {

ULayoutType
_uloc_getOrientationHelper(ध्रुव वर्ण* localeId,
                           ध्रुव वर्ण* कुञ्चिका,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    जइ (U_FAILURE(status)) { प्रत्यागम result; }

    जइ (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    जइ (U_FAILURE(status)) { प्रत्यागम result; }

    int32_t length = 0;
    ध्रुव char16_t* ध्रुव value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            कुञ्चिका,
            &length,
            &status);

    जइ (U_FAILURE(status)) { प्रत्यागम result; }

    जइ (length != 0) {
        विकल्प(value[0])
        {
        पक्ष 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            भङ्ग;
        पक्ष 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            भङ्ग;
        पक्ष 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            भङ्ग;
        पक्ष 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            भङ्ग;
        स्वभाव:
            status = U_INTERNAL_PROGRAM_ERROR;
            भङ्ग;
        }
    }

    प्रत्यागम result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ध्रुव वर्ण* localeId,
                             UErrorCode *status)
{
    प्रत्यागम _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_पङ्क्तिः_Orientation(ध्रुव वर्ण* localeId,
                        UErrorCode *status)
{
    प्रत्यागम _uloc_getOrientationHelper(localeId, "lines", *status);
}
