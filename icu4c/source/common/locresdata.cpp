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

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "ulocimp.h"
#ضمن "uresimp.h"

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
U_CAPI ثابت char16_t * U_EXPORT2
uloc_getTableStringWithFallback(ثابت حرف *path, ثابت حرف *locale,
                              ثابت حرف *table_کلید, ثابت حرف *sub_Table_کلید,
                              ثابت حرف *item_کلید,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    إذا (U_FAILURE(*pErrorCode)) { أرجع nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ثابت char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    إذا(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        أرجع nullptr;
    } وإلا إذا(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    لكل(;;){
        icu::ڈھیر_UResource_Bundle table;
        icu::ڈھیر_UResource_Bundle subTable;
        ures_get_By_کلید_With_Fallback(rb.getAlias(), table_کلید, table.getAlias(), &errorCode);

        إذا (sub_Table_کلید != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_کلید_With_Fallback(table.getAlias(), sub_Table_کلید, table.getAlias(), &errorCode);
        }
        إذا(U_SUCCESS(errorCode)){
            item = ures_get_String_By_کلید_With_Fallback(table.getAlias(), item_کلید, pLength, &errorCode);
            إذا(U_FAILURE(errorCode)){
                ثابت حرف* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                إذا(uprv_strcmp(table_کلید, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_کلید);
                }وإلا إذا(uprv_strcmp(table_کلید, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_کلید);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                إذا(replacement!=nullptr && item_کلید != replacement){
                    item = ures_get_String_By_کلید_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    إذا(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        اكسر;
                    }
                }
            }وإلا{
                اكسر;
            }
        }
        
        إذا(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ثابت char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_کلید_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            إذا(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                اكسر;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            إذا (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                اكسر;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            إذا(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                اكسر;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }وإلا{
            اكسر;
        }
    }

    أرجع item;
}

نطاق {

ULayoutType
_uloc_getOrientationHelper(ثابت حرف* localeId,
                           ثابت حرف* کلید,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    إذا (U_FAILURE(status)) { أرجع result; }

    إذا (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    إذا (U_FAILURE(status)) { أرجع result; }

    int32_t length = 0;
    ثابت char16_t* ثابت value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            کلید,
            &length,
            &status);

    إذا (U_FAILURE(status)) { أرجع result; }

    إذا (length != 0) {
        بدل(value[0])
        {
        حالة 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            اكسر;
        حالة 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            اكسر;
        حالة 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            اكسر;
        حالة 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            اكسر;
        افتراضي:
            status = U_INTERNAL_PROGRAM_ERROR;
            اكسر;
        }
    }

    أرجع result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ثابت حرف* localeId,
                             UErrorCode *status)
{
    أرجع _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_سطر_Orientation(ثابت حرف* localeId,
                        UErrorCode *status)
{
    أرجع _uloc_getOrientationHelper(localeId, "lines", *status);
}
