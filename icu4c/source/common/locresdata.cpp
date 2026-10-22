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
uloc_get_جدول_String_With_Fallback(ثابت حرف *رستو, ثابت حرف *locale,
                              ثابت حرف *جدول_ڪنجي, ثابت حرف *sub_جدول_ڪنجي,
                              ثابت حرف *شي_ڪنجي,
                              int32_t *p_ڊيگهائي,
                              UError_ڪوڊ *p_Error_ڪوڊ)
{
    إذا (U_FAILURE(*p_Error_ڪوڊ)) { أرجع خالي_اشارو; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ثابت char16_t *شي=خالي_اشارو;
    UError_ڪوڊ error_ڪوڊ;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_ڪوڊ=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_اشارو rb(ures_open(رستو, locale, &error_ڪوڊ));

    إذا(U_FAILURE(error_ڪوڊ)) {
        /* total failure, not even root could be opened */
        *p_Error_ڪوڊ=error_ڪوڊ;
        أرجع خالي_اشارو;
    } وإلا إذا(error_ڪوڊ==U_USING_DEFAULT_WARNING ||
                (error_ڪوڊ==U_USING_FALLBACK_WARNING && *p_Error_ڪوڊ!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_ڪوڊ=error_ڪوڊ;
    }

    لكل(;;){
        icu::ڍير_UResource_Bundle جدول;
        icu::ڍير_UResource_Bundle sub_جدول;
        ures_get_By_ڪنجي_With_Fallback(rb.getAlias(), جدول_ڪنجي, جدول.getAlias(), &error_ڪوڊ);

        إذا (sub_جدول_ڪنجي != خالي_اشارو) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ڪنجي_With_Fallback(جدول.getAlias(), sub_جدول_ڪنجي, جدول.getAlias(), &error_ڪوڊ);
        }
        إذا(U_SUCCESS(error_ڪوڊ)){
            شي = ures_get_String_By_ڪنجي_With_Fallback(جدول.getAlias(), شي_ڪنجي, p_ڊيگهائي, &error_ڪوڊ);
            إذا(U_FAILURE(error_ڪوڊ)){
                ثابت حرف* replacement = خالي_اشارو;
                *p_Error_ڪوڊ = error_ڪوڊ; /*save the errorCode*/
                error_ڪوڊ = U_ZERO_ERROR;
                /* may be a deprecated code */
                إذا(uprv_strcmp(جدول_ڪنجي, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(شي_ڪنجي);
                }وإلا إذا(uprv_strcmp(جدول_ڪنجي, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(شي_ڪنجي);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                إذا(replacement!=خالي_اشارو && شي_ڪنجي != replacement){
                    شي = ures_get_String_By_ڪنجي_With_Fallback(جدول.getAlias(), replacement, p_ڊيگهائي, &error_ڪوڊ);
                    إذا(U_SUCCESS(error_ڪوڊ)){
                        *p_Error_ڪوڊ = error_ڪوڊ;
                        اكسر;
                    }
                }
            }وإلا{
                اكسر;
            }
        }
        
        إذا(U_FAILURE(error_ڪوڊ)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ثابت char16_t* fallbackLocale =  خالي_اشارو;
            *p_Error_ڪوڊ = error_ڪوڊ;
            error_ڪوڊ = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ڪنجي_With_Fallback(جدول.getAlias(), "Fallback", &len, &error_ڪوڊ);
            إذا(U_FAILURE(error_ڪوڊ)){
               *p_Error_ڪوڊ = error_ڪوڊ;
                اكسر;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_ڪوڊ);

            /* guard against recursive fallback */
            إذا (explicitFallbackName == locale) {
                *p_Error_ڪوڊ = U_اندروني_پروگرام_ERROR;
                اكسر;
            }
            rb.adoptInstead(ures_open(رستو, explicitFallbackName.معلومات(), &error_ڪوڊ));
            إذا(U_FAILURE(error_ڪوڊ)){
                *p_Error_ڪوڊ = error_ڪوڊ;
                اكسر;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }وإلا{
            اكسر;
        }
    }

    أرجع شي;
}

نطاق {

ULayoutType
_uloc_getOrientationHelper(ثابت حرف* localeId,
                           ثابت حرف* ڪنجي,
                           UError_ڪوڊ& حالت)
{
    ULayoutType نتيجو = ULOC_LAYOUT_UNKNOWN;

    إذا (U_FAILURE(حالت)) { أرجع نتيجو; }

    إذا (localeId == خالي_اشارو) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeId, حالت);

    إذا (U_FAILURE(حالت)) { أرجع نتيجو; }

    int32_t length = 0;
    ثابت char16_t* ثابت value =
        uloc_get_جدول_String_With_Fallback(
            خالي_اشارو,
            locale_بفر.معلومات(),
            "layout",
            خالي_اشارو,
            ڪنجي,
            &length,
            &حالت);

    إذا (U_FAILURE(حالت)) { أرجع نتيجو; }

    إذا (length != 0) {
        بدل(value[0])
        {
        حالة 0x0062: /* 'b' */
            نتيجو = ULOC_LAYOUT_BTT;
            اكسر;
        حالة 0x006C: /* 'l' */
            نتيجو = ULOC_LAYOUT_LTR;
            اكسر;
        حالة 0x0072: /* 'r' */
            نتيجو = ULOC_LAYOUT_RTL;
            اكسر;
        حالة 0x0074: /* 't' */
            نتيجو = ULOC_LAYOUT_TTB;
            اكسر;
        افتراضي:
            حالت = U_اندروني_پروگرام_ERROR;
            اكسر;
        }
    }

    أرجع نتيجو;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ثابت حرف* localeId,
                             UError_ڪوڊ *حالت)
{
    أرجع _uloc_getOrientationHelper(localeId, "characters", *حالت);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_سٽ_Orientation(ثابت حرف* localeId,
                        UError_ڪوڊ *حالت)
{
    أرجع _uloc_getOrientationHelper(localeId, "lines", *حالت);
}
