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
uloc_get_جدول_String_With_Fallback(ثابت حرف *لار, ثابت حرف *locale,
                              ثابت حرف *جدول_کلی, ثابت حرف *sub_جدول_کلی,
                              ثابت حرف *توکی_کلی,
                              int32_t *p_اوږدتیا,
                              UError_کوډ *p_Error_کوډ)
{
    إذا (U_FAILURE(*p_Error_کوډ)) { أرجع تش_اشاره; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ثابت char16_t *توکی=تش_اشاره;
    UError_کوډ error_کوډ;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_کوډ=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_اشاره_ګر rb(ures_open(لار, locale, &error_کوډ));

    إذا(U_FAILURE(error_کوډ)) {
        /* total failure, not even root could be opened */
        *p_Error_کوډ=error_کوډ;
        أرجع تش_اشاره;
    } وإلا إذا(error_کوډ==U_USING_DEFAULT_WARNING ||
                (error_کوډ==U_USING_FALLBACK_WARNING && *p_Error_کوډ!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_کوډ=error_کوډ;
    }

    لكل(;;){
        icu::ډېری_UResource_Bundle جدول;
        icu::ډېری_UResource_Bundle sub_جدول;
        ures_get_By_کلی_With_Fallback(rb.getAlias(), جدول_کلی, جدول.getAlias(), &error_کوډ);

        إذا (sub_جدول_کلی != تش_اشاره) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_کلی_With_Fallback(جدول.getAlias(), sub_جدول_کلی, جدول.getAlias(), &error_کوډ);
        }
        إذا(U_SUCCESS(error_کوډ)){
            توکی = ures_get_String_By_کلی_With_Fallback(جدول.getAlias(), توکی_کلی, p_اوږدتیا, &error_کوډ);
            إذا(U_FAILURE(error_کوډ)){
                ثابت حرف* replacement = تش_اشاره;
                *p_Error_کوډ = error_کوډ; /*save the errorCode*/
                error_کوډ = U_ZERO_ERROR;
                /* may be a deprecated code */
                إذا(uprv_strcmp(جدول_کلی, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(توکی_کلی);
                }وإلا إذا(uprv_strcmp(جدول_کلی, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(توکی_کلی);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                إذا(replacement!=تش_اشاره && توکی_کلی != replacement){
                    توکی = ures_get_String_By_کلی_With_Fallback(جدول.getAlias(), replacement, p_اوږدتیا, &error_کوډ);
                    إذا(U_SUCCESS(error_کوډ)){
                        *p_Error_کوډ = error_کوډ;
                        اكسر;
                    }
                }
            }وإلا{
                اكسر;
            }
        }
        
        إذا(U_FAILURE(error_کوډ)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ثابت char16_t* fallbackLocale =  تش_اشاره;
            *p_Error_کوډ = error_کوډ;
            error_کوډ = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_کلی_With_Fallback(جدول.getAlias(), "Fallback", &len, &error_کوډ);
            إذا(U_FAILURE(error_کوډ)){
               *p_Error_کوډ = error_کوډ;
                اكسر;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_کوډ);

            /* guard against recursive fallback */
            إذا (explicitFallbackName == locale) {
                *p_Error_کوډ = U_دننی_پروګرام_ERROR;
                اكسر;
            }
            rb.adoptInstead(ures_open(لار, explicitFallbackName.معلومات(), &error_کوډ));
            إذا(U_FAILURE(error_کوډ)){
                *p_Error_کوډ = error_کوډ;
                اكسر;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }وإلا{
            اكسر;
        }
    }

    أرجع توکی;
}

نطاق {

ULayoutType
_uloc_getOrientationHelper(ثابت حرف* localeId,
                           ثابت حرف* کلی,
                           UError_کوډ& حالت)
{
    ULayoutType پایله = ULOC_LAYOUT_UNKNOWN;

    إذا (U_FAILURE(حالت)) { أرجع پایله; }

    إذا (localeId == تش_اشاره) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeId, حالت);

    إذا (U_FAILURE(حالت)) { أرجع پایله; }

    int32_t length = 0;
    ثابت char16_t* ثابت value =
        uloc_get_جدول_String_With_Fallback(
            تش_اشاره,
            locale_بفر.معلومات(),
            "layout",
            تش_اشاره,
            کلی,
            &length,
            &حالت);

    إذا (U_FAILURE(حالت)) { أرجع پایله; }

    إذا (length != 0) {
        بدل(value[0])
        {
        حالة 0x0062: /* 'b' */
            پایله = ULOC_LAYOUT_BTT;
            اكسر;
        حالة 0x006C: /* 'l' */
            پایله = ULOC_LAYOUT_LTR;
            اكسر;
        حالة 0x0072: /* 'r' */
            پایله = ULOC_LAYOUT_RTL;
            اكسر;
        حالة 0x0074: /* 't' */
            پایله = ULOC_LAYOUT_TTB;
            اكسر;
        افتراضي:
            حالت = U_دننی_پروګرام_ERROR;
            اكسر;
        }
    }

    أرجع پایله;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ثابت حرف* localeId,
                             UError_کوډ *حالت)
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
uloc_get_کرښه_Orientation(ثابت حرف* localeId,
                        UError_کوډ *حالت)
{
    أرجع _uloc_getOrientationHelper(localeId, "lines", *حالت);
}
