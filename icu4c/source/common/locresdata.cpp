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

#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ures.h"
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "ulocimp.h"
#समावेश "uresimp.h"

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
U_CAPI स्थिर char16_t * U_EXPORT2
uloc_getTableStringWithFallback(स्थिर अक्षर *path, स्थिर अक्षर *locale,
                              स्थिर अक्षर *table_कुंजी, स्थिर अक्षर *sub_Table_कुंजी,
                              स्थिर अक्षर *item_कुंजी,
                              int32_t *p_लंबाई,
                              UErrorCode *pErrorCode)
{
    अगर (U_FAILURE(*pErrorCode)) { वापस nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    स्थिर char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    अगर(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        वापस nullptr;
    } अन्यथा अगर(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    क्रम(;;){
        icu::ढेर_UResource_Bundle table;
        icu::ढेर_UResource_Bundle subTable;
        ures_get_By_कुंजी_With_Fallback(rb.getAlias(), table_कुंजी, table.getAlias(), &errorCode);

        अगर (sub_Table_कुंजी != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_कुंजी_With_Fallback(table.getAlias(), sub_Table_कुंजी, table.getAlias(), &errorCode);
        }
        अगर(U_SUCCESS(errorCode)){
            item = ures_get_String_By_कुंजी_With_Fallback(table.getAlias(), item_कुंजी, p_लंबाई, &errorCode);
            अगर(U_FAILURE(errorCode)){
                स्थिर अक्षर* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                अगर(uprv_strcmp(table_कुंजी, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_कुंजी);
                }अन्यथा अगर(uprv_strcmp(table_कुंजी, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_कुंजी);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                अगर(replacement!=nullptr && item_कुंजी != replacement){
                    item = ures_get_String_By_कुंजी_With_Fallback(table.getAlias(), replacement, p_लंबाई, &errorCode);
                    अगर(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        अवरोध;
                    }
                }
            }अन्यथा{
                अवरोध;
            }
        }
        
        अगर(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            स्थिर char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_कुंजी_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            अगर(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                अवरोध;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            अगर (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                अवरोध;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.तथ्य(), &errorCode));
            अगर(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                अवरोध;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }अन्यथा{
            अवरोध;
        }
    }

    वापस item;
}

namespace {

ULayoutType
_uloc_getOrientationHelper(स्थिर अक्षर* localeId,
                           स्थिर अक्षर* कुंजी,
                           UErrorCode& status)
{
    ULayoutType परिणाम = ULOC_LAYOUT_UNKNOWN;

    अगर (U_FAILURE(status)) { वापस परिणाम; }

    अगर (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeId, status);

    अगर (U_FAILURE(status)) { वापस परिणाम; }

    int32_t length = 0;
    स्थिर char16_t* स्थिर value =
        uloc_getTableStringWithFallback(
            nullptr,
            locale_बफर.तथ्य(),
            "layout",
            nullptr,
            कुंजी,
            &length,
            &status);

    अगर (U_FAILURE(status)) { वापस परिणाम; }

    अगर (length != 0) {
        चयन(value[0])
        {
        हाल 0x0062: /* 'b' */
            परिणाम = ULOC_LAYOUT_BTT;
            अवरोध;
        हाल 0x006C: /* 'l' */
            परिणाम = ULOC_LAYOUT_LTR;
            अवरोध;
        हाल 0x0072: /* 'r' */
            परिणाम = ULOC_LAYOUT_RTL;
            अवरोध;
        हाल 0x0074: /* 't' */
            परिणाम = ULOC_LAYOUT_TTB;
            अवरोध;
        शेष:
            status = U_INTERNAL_PROGRAM_ERROR;
            अवरोध;
        }
    }

    वापस परिणाम;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(स्थिर अक्षर* localeId,
                             UErrorCode *status)
{
    वापस _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_पंक्ति_Orientation(स्थिर अक्षर* localeId,
                        UErrorCode *status)
{
    वापस _uloc_getOrientationHelper(localeId, "lines", *status);
}
