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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "uresimp.h"

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
U_CAPI സ്ഥിരം char16_t * U_EXPORT2
uloc_getTableStringWithFallback(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *locale,
                              സ്ഥിരം അക്ഷരം *table_താക്കോൽ, സ്ഥിരം അക്ഷരം *sub_Table_താക്കോൽ,
                              സ്ഥിരം അക്ഷരം *item_താക്കോൽ,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    എങ്കിൽ (U_FAILURE(*pErrorCode)) { തിരികെ nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    സ്ഥിരം char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    എങ്കിൽ(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    വേണ്ടി(;;){
        icu::അട്ടി_UResource_Bundle table;
        icu::അട്ടി_UResource_Bundle subTable;
        ures_get_By_താക്കോൽ_With_Fallback(rb.getAlias(), table_താക്കോൽ, table.getAlias(), &errorCode);

        എങ്കിൽ (sub_Table_താക്കോൽ != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_താക്കോൽ_With_Fallback(table.getAlias(), sub_Table_താക്കോൽ, table.getAlias(), &errorCode);
        }
        എങ്കിൽ(U_SUCCESS(errorCode)){
            item = ures_get_String_By_താക്കോൽ_With_Fallback(table.getAlias(), item_താക്കോൽ, pLength, &errorCode);
            എങ്കിൽ(U_FAILURE(errorCode)){
                സ്ഥിരം അക്ഷരം* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                എങ്കിൽ(uprv_strcmp(table_താക്കോൽ, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_താക്കോൽ);
                }അല്ലെങ്കിൽ എങ്കിൽ(uprv_strcmp(table_താക്കോൽ, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_താക്കോൽ);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                എങ്കിൽ(replacement!=nullptr && item_താക്കോൽ != replacement){
                    item = ures_get_String_By_താക്കോൽ_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    എങ്കിൽ(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        നിർത്തുക;
                    }
                }
            }അല്ലെങ്കിൽ{
                നിർത്തുക;
            }
        }
        
        എങ്കിൽ(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            സ്ഥിരം char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_താക്കോൽ_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            എങ്കിൽ(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                നിർത്തുക;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            എങ്കിൽ (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                നിർത്തുക;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            എങ്കിൽ(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                നിർത്തുക;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }അല്ലെങ്കിൽ{
            നിർത്തുക;
        }
    }

    തിരികെ item;
}

നാമാകാശ {

ULayoutType
_uloc_getOrientationHelper(സ്ഥിരം അക്ഷരം* localeId,
                           സ്ഥിരം അക്ഷരം* താക്കോൽ,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    എങ്കിൽ (U_FAILURE(status)) { തിരികെ result; }

    എങ്കിൽ (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    എങ്കിൽ (U_FAILURE(status)) { തിരികെ result; }

    int32_t length = 0;
    സ്ഥിരം char16_t* സ്ഥിരം value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            താക്കോൽ,
            &length,
            &status);

    എങ്കിൽ (U_FAILURE(status)) { തിരികെ result; }

    എങ്കിൽ (length != 0) {
        തിരഞ്ഞെടുപ്പ്(value[0])
        {
        അവസ്ഥ 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            നിർത്തുക;
        അവസ്ഥ 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            നിർത്തുക;
        അവസ്ഥ 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            നിർത്തുക;
        അവസ്ഥ 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            status = U_INTERNAL_PROGRAM_ERROR;
            നിർത്തുക;
        }
    }

    തിരികെ result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(സ്ഥിരം അക്ഷരം* localeId,
                             UErrorCode *status)
{
    തിരികെ _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_വരി_Orientation(സ്ഥിരം അക്ഷരം* localeId,
                        UErrorCode *status)
{
    തിരികെ _uloc_getOrientationHelper(localeId, "lines", *status);
}
