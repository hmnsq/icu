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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "uresimp.h"

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
U_CAPI மாறிலி char16_t * U_EXPORT2
uloc_getTableStringWithFallback(மாறிலி எழுத்து *path, மாறிலி எழுத்து *locale,
                              மாறிலி எழுத்து *table_சாவி, மாறிலி எழுத்து *sub_Table_சாவி,
                              மாறிலி எழுத்து *item_சாவி,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    எனில் (U_FAILURE(*pErrorCode)) { திரும்பு nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    மாறிலி char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    எனில்(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        திரும்பு nullptr;
    } இல்லையெனில் எனில்(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    க்கு(;;){
        icu::அடுக்கு_UResource_Bundle table;
        icu::அடுக்கு_UResource_Bundle subTable;
        ures_get_By_சாவி_With_Fallback(rb.getAlias(), table_சாவி, table.getAlias(), &errorCode);

        எனில் (sub_Table_சாவி != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_சாவி_With_Fallback(table.getAlias(), sub_Table_சாவி, table.getAlias(), &errorCode);
        }
        எனில்(U_SUCCESS(errorCode)){
            item = ures_get_String_By_சாவி_With_Fallback(table.getAlias(), item_சாவி, pLength, &errorCode);
            எனில்(U_FAILURE(errorCode)){
                மாறிலி எழுத்து* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                எனில்(uprv_strcmp(table_சாவி, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_சாவி);
                }இல்லையெனில் எனில்(uprv_strcmp(table_சாவி, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_சாவி);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                எனில்(replacement!=nullptr && item_சாவி != replacement){
                    item = ures_get_String_By_சாவி_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    எனில்(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        நிறுத்து;
                    }
                }
            }இல்லையெனில்{
                நிறுத்து;
            }
        }
        
        எனில்(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            மாறிலி char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_சாவி_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            எனில்(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                நிறுத்து;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            எனில் (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                நிறுத்து;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            எனில்(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                நிறுத்து;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }இல்லையெனில்{
            நிறுத்து;
        }
    }

    திரும்பு item;
}

நாமாகாஶ {

ULayoutType
_uloc_getOrientationHelper(மாறிலி எழுத்து* localeId,
                           மாறிலி எழுத்து* சாவி,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    எனில் (U_FAILURE(status)) { திரும்பு result; }

    எனில் (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    எனில் (U_FAILURE(status)) { திரும்பு result; }

    int32_t length = 0;
    மாறிலி char16_t* மாறிலி value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            சாவி,
            &length,
            &status);

    எனில் (U_FAILURE(status)) { திரும்பு result; }

    எனில் (length != 0) {
        தேர்வு(value[0])
        {
        நிலை 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            நிறுத்து;
        நிலை 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            நிறுத்து;
        நிலை 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            நிறுத்து;
        நிலை 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            நிறுத்து;
        இயல்பு:
            status = U_INTERNAL_PROGRAM_ERROR;
            நிறுத்து;
        }
    }

    திரும்பு result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(மாறிலி எழுத்து* localeId,
                             UErrorCode *status)
{
    திரும்பு _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_வரி_Orientation(மாறிலி எழுத்து* localeId,
                        UErrorCode *status)
{
    திரும்பு _uloc_getOrientationHelper(localeId, "lines", *status);
}
