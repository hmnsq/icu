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
uloc_get_सारणी_String_With_Fallback(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *locale,
                              ध्रुव वर्ण *सारणी_कुञ्चिका, ध्रुव वर्ण *sub_सारणी_कुञ्चिका,
                              ध्रुव वर्ण *item_कुञ्चिका,
                              int32_t *p_दीर्घता,
                              UError_संहिता *p_Error_संहिता)
{
    यदि (U_FAILURE(*p_Error_संहिता)) { प्रत्यागम शून्यसूचकः; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ध्रुव char16_t *item=शून्यसूचकः;
    UError_संहिता error_संहिता;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_संहिता=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(मार्गः, locale, &error_संहिता));

    यदि(U_FAILURE(error_संहिता)) {
        /* total failure, not even root could be opened */
        *p_Error_संहिता=error_संहिता;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि(error_संहिता==U_USING_DEFAULT_WARNING ||
                (error_संहिता==U_USING_FALLBACK_WARNING && *p_Error_संहिता!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_संहिता=error_संहिता;
    }

    प्रति(;;){
        icu::राशिः_UResource_Bundle सारणी;
        icu::राशिः_UResource_Bundle sub_सारणी;
        ures_get_By_कुञ्चिका_With_Fallback(rb.getAlias(), सारणी_कुञ्चिका, सारणी.getAlias(), &error_संहिता);

        यदि (sub_सारणी_कुञ्चिका != शून्यसूचकः) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_कुञ्चिका_With_Fallback(सारणी.getAlias(), sub_सारणी_कुञ्चिका, सारणी.getAlias(), &error_संहिता);
        }
        यदि(U_SUCCESS(error_संहिता)){
            item = ures_get_String_By_कुञ्चिका_With_Fallback(सारणी.getAlias(), item_कुञ्चिका, p_दीर्घता, &error_संहिता);
            यदि(U_FAILURE(error_संहिता)){
                ध्रुव वर्ण* replacement = शून्यसूचकः;
                *p_Error_संहिता = error_संहिता; /*save the errorCode*/
                error_संहिता = U_ZERO_ERROR;
                /* may be a deprecated code */
                यदि(uprv_strcmp(सारणी_कुञ्चिका, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_कुञ्चिका);
                }अन्यथा यदि(uprv_strcmp(सारणी_कुञ्चिका, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_कुञ्चिका);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                यदि(replacement!=शून्यसूचकः && item_कुञ्चिका != replacement){
                    item = ures_get_String_By_कुञ्चिका_With_Fallback(सारणी.getAlias(), replacement, p_दीर्घता, &error_संहिता);
                    यदि(U_SUCCESS(error_संहिता)){
                        *p_Error_संहिता = error_संहिता;
                        भङ्ग;
                    }
                }
            }अन्यथा{
                भङ्ग;
            }
        }
        
        यदि(U_FAILURE(error_संहिता)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ध्रुव char16_t* fallbackLocale =  शून्यसूचकः;
            *p_Error_संहिता = error_संहिता;
            error_संहिता = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_कुञ्चिका_With_Fallback(सारणी.getAlias(), "Fallback", &len, &error_संहिता);
            यदि(U_FAILURE(error_संहिता)){
               *p_Error_संहिता = error_संहिता;
                भङ्ग;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_संहिता);

            /* guard against recursive fallback */
            यदि (explicitFallbackName == locale) {
                *p_Error_संहिता = U_आन्तरिकम्_क्रमादेशः_ERROR;
                भङ्ग;
            }
            rb.adoptInstead(ures_open(मार्गः, explicitFallbackName.दत्त(), &error_संहिता));
            यदि(U_FAILURE(error_संहिता)){
                *p_Error_संहिता = error_संहिता;
                भङ्ग;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }अन्यथा{
            भङ्ग;
        }
    }

    प्रत्यागम item;
}

नामाकाश {

ULayoutType
_uloc_getOrientationHelper(ध्रुव वर्ण* localeId,
                           ध्रुव वर्ण* कुञ्चिका,
                           UError_संहिता& स्थितिः)
{
    ULayoutType परिणाम = ULOC_LAYOUT_UNKNOWN;

    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम परिणाम; }

    यदि (localeId == शून्यसूचकः) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeId, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम परिणाम; }

    int32_t length = 0;
    ध्रुव char16_t* ध्रुव value =
        uloc_get_सारणी_String_With_Fallback(
            शून्यसूचकः,
            locale_बफर.दत्त(),
            "layout",
            शून्यसूचकः,
            कुञ्चिका,
            &length,
            &स्थितिः);

    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम परिणाम; }

    यदि (length != 0) {
        विकल्प(value[0])
        {
        पक्ष 0x0062: /* 'b' */
            परिणाम = ULOC_LAYOUT_BTT;
            भङ्ग;
        पक्ष 0x006C: /* 'l' */
            परिणाम = ULOC_LAYOUT_LTR;
            भङ्ग;
        पक्ष 0x0072: /* 'r' */
            परिणाम = ULOC_LAYOUT_RTL;
            भङ्ग;
        पक्ष 0x0074: /* 't' */
            परिणाम = ULOC_LAYOUT_TTB;
            भङ्ग;
        स्वभाव:
            स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
            भङ्ग;
        }
    }

    प्रत्यागम परिणाम;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ध्रुव वर्ण* localeId,
                             UError_संहिता *स्थितिः)
{
    प्रत्यागम _uloc_getOrientationHelper(localeId, "characters", *स्थितिः);
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
                        UError_संहिता *स्थितिः)
{
    प्रत्यागम _uloc_getOrientationHelper(localeId, "lines", *स्थितिः);
}
