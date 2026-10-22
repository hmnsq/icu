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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "uresimp.h"

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
U_CAPI અચલ char16_t * U_EXPORT2
uloc_getTableStringWithFallback(અચલ અક્ષર *path, અચલ અક્ષર *locale,
                              અચલ અક્ષર *table_ચાવી, અચલ અક્ષર *sub_Table_ચાવી,
                              અચલ અક્ષર *item_ચાવી,
                              int32_t *pLength,
                              UErrorCode *pErrorCode)
{
    જો (U_FAILURE(*pErrorCode)) { પાછા nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    અચલ char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    જો(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        પાછા nullptr;
    } નહીંતર જો(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    માટે(;;){
        icu::ઢગલો_UResource_Bundle table;
        icu::ઢગલો_UResource_Bundle subTable;
        ures_get_By_ચાવી_With_Fallback(rb.getAlias(), table_ચાવી, table.getAlias(), &errorCode);

        જો (sub_Table_ચાવી != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ચાવી_With_Fallback(table.getAlias(), sub_Table_ચાવી, table.getAlias(), &errorCode);
        }
        જો(U_SUCCESS(errorCode)){
            item = ures_get_String_By_ચાવી_With_Fallback(table.getAlias(), item_ચાવી, pLength, &errorCode);
            જો(U_FAILURE(errorCode)){
                અચલ અક્ષર* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                જો(uprv_strcmp(table_ચાવી, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_ચાવી);
                }નહીંતર જો(uprv_strcmp(table_ચાવી, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_ચાવી);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                જો(replacement!=nullptr && item_ચાવી != replacement){
                    item = ures_get_String_By_ચાવી_With_Fallback(table.getAlias(), replacement, pLength, &errorCode);
                    જો(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        તોડો;
                    }
                }
            }નહીંતર{
                તોડો;
            }
        }
        
        જો(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            અચલ char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ચાવી_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            જો(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                તોડો;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            જો (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                તોડો;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.data(), &errorCode));
            જો(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                તોડો;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }નહીંતર{
            તોડો;
        }
    }

    પાછા item;
}

નામાકાશ {

ULayoutType
_uloc_getOrientationHelper(અચલ અક્ષર* localeId,
                           અચલ અક્ષર* ચાવી,
                           UErrorCode& status)
{
    ULayoutType result = ULOC_LAYOUT_UNKNOWN;

    જો (U_FAILURE(status)) { પાછા result; }

    જો (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeId, status);

    જો (U_FAILURE(status)) { પાછા result; }

    int32_t length = 0;
    અચલ char16_t* અચલ value =
        uloc_getTableStringWithFallback(
            nullptr,
            localeBuffer.data(),
            "layout",
            nullptr,
            ચાવી,
            &length,
            &status);

    જો (U_FAILURE(status)) { પાછા result; }

    જો (length != 0) {
        પસંદગી(value[0])
        {
        સ્થિતિ 0x0062: /* 'b' */
            result = ULOC_LAYOUT_BTT;
            તોડો;
        સ્થિતિ 0x006C: /* 'l' */
            result = ULOC_LAYOUT_LTR;
            તોડો;
        સ્થિતિ 0x0072: /* 'r' */
            result = ULOC_LAYOUT_RTL;
            તોડો;
        સ્થિતિ 0x0074: /* 't' */
            result = ULOC_LAYOUT_TTB;
            તોડો;
        મૂળભૂત:
            status = U_INTERNAL_PROGRAM_ERROR;
            તોડો;
        }
    }

    પાછા result;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(અચલ અક્ષર* localeId,
                             UErrorCode *status)
{
    પાછા _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_લીટી_Orientation(અચલ અક્ષર* localeId,
                        UErrorCode *status)
{
    પાછા _uloc_getOrientationHelper(localeId, "lines", *status);
}
