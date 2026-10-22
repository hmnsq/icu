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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"

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
U_CAPI ਅਚਲ char16_t * U_EXPORT2
uloc_get_ਸਾਰਣੀ_String_With_Fallback(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *locale,
                              ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *sub_ਸਾਰਣੀ_ਕੁੰਜੀ,
                              ਅਚਲ ਅੱਖਰ *ਵਸਤੂ_ਕੁੰਜੀ,
                              int32_t *p_ਲੰਮਾਈ,
                              UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ਅਚਲ char16_t *ਵਸਤੂ=ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ error_ਕੋਡ;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_ਕੋਡ=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(ਰਸਤਾ, locale, &error_ਕੋਡ));

    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        /* total failure, not even root could be opened */
        *p_Error_ਕੋਡ=error_ਕੋਡ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(error_ਕੋਡ==U_USING_DEFAULT_WARNING ||
                (error_ਕੋਡ==U_USING_FALLBACK_WARNING && *p_Error_ਕੋਡ!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_ਕੋਡ=error_ਕੋਡ;
    }

    ਲਈ(;;){
        icu::ਢੇਰ_UResource_Bundle ਸਾਰਣੀ;
        icu::ਢੇਰ_UResource_Bundle sub_ਸਾਰਣੀ;
        ures_get_By_ਕੁੰਜੀ_With_Fallback(rb.getAlias(), ਸਾਰਣੀ_ਕੁੰਜੀ, ਸਾਰਣੀ.getAlias(), &error_ਕੋਡ);

        ਜੇ (sub_ਸਾਰਣੀ_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ਕੁੰਜੀ_With_Fallback(ਸਾਰਣੀ.getAlias(), sub_ਸਾਰਣੀ_ਕੁੰਜੀ, ਸਾਰਣੀ.getAlias(), &error_ਕੋਡ);
        }
        ਜੇ(U_SUCCESS(error_ਕੋਡ)){
            ਵਸਤੂ = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(ਸਾਰਣੀ.getAlias(), ਵਸਤੂ_ਕੁੰਜੀ, p_ਲੰਮਾਈ, &error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ)){
                ਅਚਲ ਅੱਖਰ* replacement = ਖਾਲੀ_ਸੂਚਕ;
                *p_Error_ਕੋਡ = error_ਕੋਡ; /*save the errorCode*/
                error_ਕੋਡ = U_ZERO_ERROR;
                /* may be a deprecated code */
                ਜੇ(uprv_strcmp(ਸਾਰਣੀ_ਕੁੰਜੀ, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(ਵਸਤੂ_ਕੁੰਜੀ);
                }ਨਹੀਂਤਾਂ ਜੇ(uprv_strcmp(ਸਾਰਣੀ_ਕੁੰਜੀ, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(ਵਸਤੂ_ਕੁੰਜੀ);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ਜੇ(replacement!=ਖਾਲੀ_ਸੂਚਕ && ਵਸਤੂ_ਕੁੰਜੀ != replacement){
                    ਵਸਤੂ = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(ਸਾਰਣੀ.getAlias(), replacement, p_ਲੰਮਾਈ, &error_ਕੋਡ);
                    ਜੇ(U_SUCCESS(error_ਕੋਡ)){
                        *p_Error_ਕੋਡ = error_ਕੋਡ;
                        ਰੋਕੋ;
                    }
                }
            }ਨਹੀਂਤਾਂ{
                ਰੋਕੋ;
            }
        }
        
        ਜੇ(U_FAILURE(error_ਕੋਡ)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ਅਚਲ char16_t* fallbackLocale =  ਖਾਲੀ_ਸੂਚਕ;
            *p_Error_ਕੋਡ = error_ਕੋਡ;
            error_ਕੋਡ = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(ਸਾਰਣੀ.getAlias(), "Fallback", &len, &error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ)){
               *p_Error_ਕੋਡ = error_ਕੋਡ;
                ਰੋਕੋ;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_ਕੋਡ);

            /* guard against recursive fallback */
            ਜੇ (explicitFallbackName == locale) {
                *p_Error_ਕੋਡ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                ਰੋਕੋ;
            }
            rb.adoptInstead(ures_open(ਰਸਤਾ, explicitFallbackName.ਅੰਕੜੇ(), &error_ਕੋਡ));
            ਜੇ(U_FAILURE(error_ਕੋਡ)){
                *p_Error_ਕੋਡ = error_ਕੋਡ;
                ਰੋਕੋ;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ਨਹੀਂਤਾਂ{
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ ਵਸਤੂ;
}

ਨਾਮਾਕਾਸ਼ {

ULayoutType
_uloc_getOrientationHelper(ਅਚਲ ਅੱਖਰ* localeId,
                           ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                           UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ULayoutType ਨਤੀਜਾ = ULOC_LAYOUT_UNKNOWN;

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਨਤੀਜਾ; }

    ਜੇ (localeId == ਖਾਲੀ_ਸੂਚਕ) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_ਬਫਰ = ulocimp_canonicalize(localeId, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਨਤੀਜਾ; }

    int32_t length = 0;
    ਅਚਲ char16_t* ਅਚਲ value =
        uloc_get_ਸਾਰਣੀ_String_With_Fallback(
            ਖਾਲੀ_ਸੂਚਕ,
            locale_ਬਫਰ.ਅੰਕੜੇ(),
            "layout",
            ਖਾਲੀ_ਸੂਚਕ,
            ਕੁੰਜੀ,
            &length,
            &ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਨਤੀਜਾ; }

    ਜੇ (length != 0) {
        ਚੋਣ(value[0])
        {
        ਹਾਲਤ 0x0062: /* 'b' */
            ਨਤੀਜਾ = ULOC_LAYOUT_BTT;
            ਰੋਕੋ;
        ਹਾਲਤ 0x006C: /* 'l' */
            ਨਤੀਜਾ = ULOC_LAYOUT_LTR;
            ਰੋਕੋ;
        ਹਾਲਤ 0x0072: /* 'r' */
            ਨਤੀਜਾ = ULOC_LAYOUT_RTL;
            ਰੋਕੋ;
        ਹਾਲਤ 0x0074: /* 't' */
            ਨਤੀਜਾ = ULOC_LAYOUT_TTB;
            ਰੋਕੋ;
        ਮੂਲ:
            ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ਅਚਲ ਅੱਖਰ* localeId,
                             UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਵਾਪਸ _uloc_getOrientationHelper(localeId, "characters", *ਸਥਿਤੀ);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_ਸਤਰ_Orientation(ਅਚਲ ਅੱਖਰ* localeId,
                        UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਵਾਪਸ _uloc_getOrientationHelper(localeId, "lines", *ਸਥਿਤੀ);
}
