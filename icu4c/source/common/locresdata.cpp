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
uloc_get_جدول_String_With_Fallback(ثابت حرف *مسیر, ثابت حرف *locale,
                              ثابت حرف *جدول_کلید, ثابت حرف *sub_جدول_کلید,
                              ثابت حرف *قلم_کلید,
                              int32_t *p_درازی,
                              UError_کد *p_Error_کد)
{
    إذا (U_FAILURE(*p_Error_کد)) { أرجع اشاره_تهی; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ثابت char16_t *قلم=اشاره_تهی;
    UError_کد error_کد;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_کد=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_اشاره_گر rb(ures_open(مسیر, locale, &error_کد));

    إذا(U_FAILURE(error_کد)) {
        /* total failure, not even root could be opened */
        *p_Error_کد=error_کد;
        أرجع اشاره_تهی;
    } وإلا إذا(error_کد==U_USING_DEFAULT_WARNING ||
                (error_کد==U_USING_FALLBACK_WARNING && *p_Error_کد!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_کد=error_کد;
    }

    لكل(;;){
        icu::پشته_UResource_Bundle جدول;
        icu::پشته_UResource_Bundle sub_جدول;
        ures_get_By_کلید_With_Fallback(rb.getAlias(), جدول_کلید, جدول.getAlias(), &error_کد);

        إذا (sub_جدول_کلید != اشاره_تهی) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_کلید_With_Fallback(جدول.getAlias(), sub_جدول_کلید, جدول.getAlias(), &error_کد);
        }
        إذا(U_SUCCESS(error_کد)){
            قلم = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), قلم_کلید, p_درازی, &error_کد);
            إذا(U_FAILURE(error_کد)){
                ثابت حرف* replacement = اشاره_تهی;
                *p_Error_کد = error_کد; /*save the errorCode*/
                error_کد = U_ZERO_ERROR;
                /* may be a deprecated code */
                إذا(uprv_strcmp(جدول_کلید, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(قلم_کلید);
                }وإلا إذا(uprv_strcmp(جدول_کلید, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(قلم_کلید);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                إذا(replacement!=اشاره_تهی && قلم_کلید != replacement){
                    قلم = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), replacement, p_درازی, &error_کد);
                    إذا(U_SUCCESS(error_کد)){
                        *p_Error_کد = error_کد;
                        اكسر;
                    }
                }
            }وإلا{
                اكسر;
            }
        }
        
        إذا(U_FAILURE(error_کد)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ثابت char16_t* fallbackLocale =  اشاره_تهی;
            *p_Error_کد = error_کد;
            error_کد = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), "Fallback", &len, &error_کد);
            إذا(U_FAILURE(error_کد)){
               *p_Error_کد = error_کد;
                اكسر;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_کد);

            /* guard against recursive fallback */
            إذا (explicitFallbackName == locale) {
                *p_Error_کد = U_داخلی_برنامه_ERROR;
                اكسر;
            }
            rb.adoptInstead(ures_open(مسیر, explicitFallbackName.داده(), &error_کد));
            إذا(U_FAILURE(error_کد)){
                *p_Error_کد = error_کد;
                اكسر;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }وإلا{
            اكسر;
        }
    }

    أرجع قلم;
}

نطاق {

ULayoutType
_uloc_getOrientationHelper(ثابت حرف* localeId,
                           ثابت حرف* کلید,
                           UError_کد& وضعیت)
{
    ULayoutType نتیجه = ULOC_LAYOUT_UNKNOWN;

    إذا (U_FAILURE(وضعیت)) { أرجع نتیجه; }

    إذا (localeId == اشاره_تهی) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_میانگیر = ulocimp_canonicalize(localeId, وضعیت);

    إذا (U_FAILURE(وضعیت)) { أرجع نتیجه; }

    int32_t length = 0;
    ثابت char16_t* ثابت value =
        uloc_get_جدول_String_With_Fallback(
            اشاره_تهی,
            locale_میانگیر.داده(),
            "layout",
            اشاره_تهی,
            کلید,
            &length,
            &وضعیت);

    إذا (U_FAILURE(وضعیت)) { أرجع نتیجه; }

    إذا (length != 0) {
        بدل(value[0])
        {
        حالة 0x0062: /* 'b' */
            نتیجه = ULOC_LAYOUT_BTT;
            اكسر;
        حالة 0x006C: /* 'l' */
            نتیجه = ULOC_LAYOUT_LTR;
            اكسر;
        حالة 0x0072: /* 'r' */
            نتیجه = ULOC_LAYOUT_RTL;
            اكسر;
        حالة 0x0074: /* 't' */
            نتیجه = ULOC_LAYOUT_TTB;
            اكسر;
        افتراضي:
            وضعیت = U_داخلی_برنامه_ERROR;
            اكسر;
        }
    }

    أرجع نتیجه;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ثابت حرف* localeId,
                             UError_کد *وضعیت)
{
    أرجع _uloc_getOrientationHelper(localeId, "characters", *وضعیت);
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
                        UError_کد *وضعیت)
{
    أرجع _uloc_getOrientationHelper(localeId, "lines", *وضعیت);
}
