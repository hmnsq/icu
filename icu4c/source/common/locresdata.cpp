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
U_CAPI अचर char16_t * U_EXPORT2
uloc_getTableStringWithFallback(अचर वर्ण *path, अचर वर्ण *locale,
                              अचर वर्ण *table_साँचो, अचर वर्ण *sub_Table_साँचो,
                              अचर वर्ण *item_साँचो,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    यदि (U_FAILURE(*pErrorCode)) { फर्काऊ nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    अचर char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    यदि(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        फर्काऊ nullptr;
    } अन्यथा यदि(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    लागि(;;){
        icu::थाक_UResource_Bundle table;
        icu::थाक_UResource_Bundle subTable;
        ures_get_By_साँचो_With_Fallback(rb.getAlias(), table_साँचो, table.getAlias(), &errorCode);

        यदि (sub_Table_साँचो != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_साँचो_With_Fallback(table.getAlias(), sub_Table_साँचो, table.getAlias(), &errorCode);
        }
        यदि(U_SUCCESS(errorCode)){
            item = ures_get_String_By_साँचो_With_Fallback(table.getAlias(), item_साँचो, pLength, &errorCode);
            यदि(U_FAILURE(errorCode)){
                अचर वर्ण* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                यदि(uprv_strcmp(table_साँचो, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_साँचो);
                }अन्यथा यदि(uprv_strcmp(table_साँचो, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_साँचो);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                यदि(replacement!=nullptr && item_साँचो != replacement){
                    item = ures_get_String_By_साँचो_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    यदि(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        रोक;
                    }
                }
            }अन्यथा{
                रोक;
            }
        }
        
        यदि(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            अचर char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_साँचो_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            यदि(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                रोक;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            यदि (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                रोक;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            यदि(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                रोक;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }अन्यथा{
            रोक;
        }
    }

    फर्काऊ item;
}

नामाकाश {

ULayoutType
_uloc_getOrientationHelper(अचर वर्ण* localeId,
                           अचर वर्ण* साँचो,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    यदि (U_FAILURE(status)) { फर्काऊ result; }

    यदि (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    यदि (U_FAILURE(status)) { फर्काऊ result; }

    int32_t length = 0;
    अचर char16_t* अचर value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            साँचो,
            &length,
            &status);

    यदि (U_FAILURE(status)) { फर्काऊ result; }

    यदि (length != 0) {
        विकल्प(value[0])
        {
        पक्ष 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            रोक;
        पक्ष 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            रोक;
        पक्ष 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            रोक;
        पक्ष 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            रोक;
        स्वभाव:
            status = U_INTERNAL_PROGRAM_ERROR;
            रोक;
        }
    }

    फर्काऊ result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(अचर वर्ण* localeId,
                             UErrorCode *status)
{
    फर्काऊ _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_पङ्क्ति_Orientation(अचर वर्ण* localeId,
                        UErrorCode *status)
{
    फर्काऊ _uloc_getOrientationHelper(localeId, "lines", *status);
}
