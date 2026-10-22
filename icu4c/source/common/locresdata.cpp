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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"

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
U_CAPI ଅଚଳ char16_t * U_EXPORT2
uloc_getTableStringWithFallback(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *locale,
                              ଅଚଳ ଅକ୍ଷର *table_ଚାବି, ଅଚଳ ଅକ୍ଷର *sub_Table_ଚାବି,
                              ଅଚଳ ଅକ୍ଷର *item_ଚାବି,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    ଯଦି (U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ଅଚଳ char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    ଯଦି(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    ପାଇଁ(;;){
        icu::ଗଦା_UResource_Bundle table;
        icu::ଗଦା_UResource_Bundle subTable;
        ures_get_By_ଚାବି_With_Fallback(rb.getAlias(), table_ଚାବି, table.getAlias(), &errorCode);

        ଯଦି (sub_Table_ଚାବି != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ଚାବି_With_Fallback(table.getAlias(), sub_Table_ଚାବି, table.getAlias(), &errorCode);
        }
        ଯଦି(U_SUCCESS(errorCode)){
            item = ures_get_String_By_ଚାବି_With_Fallback(table.getAlias(), item_ଚାବି, pLength, &errorCode);
            ଯଦି(U_FAILURE(errorCode)){
                ଅଚଳ ଅକ୍ଷର* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                ଯଦି(uprv_strcmp(table_ଚାବି, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_ଚାବି);
                }ଅନ୍ୟଥା ଯଦି(uprv_strcmp(table_ଚାବି, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_ଚାବି);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ଯଦି(replacement!=nullptr && item_ଚାବି != replacement){
                    item = ures_get_String_By_ଚାବି_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    ଯଦି(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        ଭାଙ୍ଗ;
                    }
                }
            }ଅନ୍ୟଥା{
                ଭାଙ୍ଗ;
            }
        }
        
        ଯଦି(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ଅଚଳ char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ଚାବି_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            ଯଦି(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                ଭାଙ୍ଗ;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            ଯଦି (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                ଭାଙ୍ଗ;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            ଯଦି(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                ଭାଙ୍ଗ;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ଅନ୍ୟଥା{
            ଭାଙ୍ଗ;
        }
    }

    ଫେରନ୍ତୁ item;
}

ନାମାକାଶ {

ULayoutType
_uloc_getOrientationHelper(ଅଚଳ ଅକ୍ଷର* localeId,
                           ଅଚଳ ଅକ୍ଷର* ଚାବି,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ result; }

    ଯଦି (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ result; }

    int32_t length = 0;
    ଅଚଳ char16_t* ଅଚଳ value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            ଚାବି,
            &length,
            &status);

    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ result; }

    ଯଦି (length != 0) {
        ବାଛ(value[0])
        {
        ସ୍ଥିତି 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            status = U_INTERNAL_PROGRAM_ERROR;
            ଭାଙ୍ଗ;
        }
    }

    ଫେରନ୍ତୁ result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ଅଚଳ ଅକ୍ଷର* localeId,
                             UErrorCode *status)
{
    ଫେରନ୍ତୁ _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_ଧାଡ଼ି_Orientation(ଅଚଳ ଅକ୍ଷର* localeId,
                        UErrorCode *status)
{
    ଫେରନ୍ତୁ _uloc_getOrientationHelper(localeId, "lines", *status);
}
