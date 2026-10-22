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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

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
U_CAPI ಸ್ಥಿರಾಂಕ char16_t * U_EXPORT2
uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sub_ಕೋಷ್ಟಕ_ಕೀಲಿ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ_ಕೀಲಿ,
                              int32_t *p_ಉದ್ದಳತೆ,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ಸ್ಥಿರಾಂಕ char16_t *ಅಂಶ=ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_ಸಂಕೇತ=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(ಮಾರ್ಗ, locale, &error_ಸಂಕೇತ));

    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        /* total failure, not even root could be opened */
        *p_Error_ಸಂಕೇತ=error_ಸಂಕೇತ;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(error_ಸಂಕೇತ==U_USING_DEFAULT_WARNING ||
                (error_ಸಂಕೇತ==U_USING_FALLBACK_WARNING && *p_Error_ಸಂಕೇತ!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_ಸಂಕೇತ=error_ಸಂಕೇತ;
    }

    ಗಾಗಿ(;;){
        icu::ರಾಶಿ_UResource_Bundle ಕೋಷ್ಟಕ;
        icu::ರಾಶಿ_UResource_Bundle sub_ಕೋಷ್ಟಕ;
        ures_get_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), ಕೋಷ್ಟಕ_ಕೀಲಿ, ಕೋಷ್ಟಕ.getAlias(), &error_ಸಂಕೇತ);

        ಆದರೆ (sub_ಕೋಷ್ಟಕ_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_ಕೀಲಿ_With_Fallback(ಕೋಷ್ಟಕ.getAlias(), sub_ಕೋಷ್ಟಕ_ಕೀಲಿ, ಕೋಷ್ಟಕ.getAlias(), &error_ಸಂಕೇತ);
        }
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)){
            ಅಂಶ = ures_get_String_By_ಕೀಲಿ_With_Fallback(ಕೋಷ್ಟಕ.getAlias(), ಅಂಶ_ಕೀಲಿ, p_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)){
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacement = ಶೂನ್ಯಸೂಚಕ;
                *p_Error_ಸಂಕೇತ = error_ಸಂಕೇತ; /*save the errorCode*/
                error_ಸಂಕೇತ = U_ZERO_ERROR;
                /* may be a deprecated code */
                ಆದರೆ(uprv_strcmp(ಕೋಷ್ಟಕ_ಕೀಲಿ, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(ಅಂಶ_ಕೀಲಿ);
                }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uprv_strcmp(ಕೋಷ್ಟಕ_ಕೀಲಿ, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(ಅಂಶ_ಕೀಲಿ);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ಆದರೆ(replacement!=ಶೂನ್ಯಸೂಚಕ && ಅಂಶ_ಕೀಲಿ != replacement){
                    ಅಂಶ = ures_get_String_By_ಕೀಲಿ_With_Fallback(ಕೋಷ್ಟಕ.getAlias(), replacement, p_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
                    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)){
                        *p_Error_ಸಂಕೇತ = error_ಸಂಕೇತ;
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }ಇಲ್ಲದಿದ್ದರೆ{
                ನಿಲ್ಲಿಸು;
            }
        }
        
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ಸ್ಥಿರಾಂಕ char16_t* fallbackLocale =  ಶೂನ್ಯಸೂಚಕ;
            *p_Error_ಸಂಕೇತ = error_ಸಂಕೇತ;
            error_ಸಂಕೇತ = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_ಕೀಲಿ_With_Fallback(ಕೋಷ್ಟಕ.getAlias(), "Fallback", &len, &error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)){
               *p_Error_ಸಂಕೇತ = error_ಸಂಕೇತ;
                ನಿಲ್ಲಿಸು;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_ಸಂಕೇತ);

            /* guard against recursive fallback */
            ಆದರೆ (explicitFallbackName == locale) {
                *p_Error_ಸಂಕೇತ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            rb.adoptInstead(ures_open(ಮಾರ್ಗ, explicitFallbackName.ದತ್ತಾಂಶ(), &error_ಸಂಕೇತ));
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)){
                *p_Error_ಸಂಕೇತ = error_ಸಂಕೇತ;
                ನಿಲ್ಲಿಸು;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ಇಲ್ಲದಿದ್ದರೆ{
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ ಅಂಶ;
}

ನಾಮಾಕಾಶ {

ULayoutType
_uloc_getOrientationHelper(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ,
                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ULayoutType ಫಲಿತಾಂಶ = ULOC_LAYOUT_UNKNOWN;

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }

    ಆದರೆ (localeId == ಶೂನ್ಯಸೂಚಕ) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_ಬಫರ್ = ulocimp_canonicalize(localeId, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }

    int32_t length = 0;
    ಸ್ಥಿರಾಂಕ char16_t* ಸ್ಥಿರಾಂಕ value =
        uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(
            ಶೂನ್ಯಸೂಚಕ,
            locale_ಬಫರ್.ದತ್ತಾಂಶ(),
            "layout",
            ಶೂನ್ಯಸೂಚಕ,
            ಕೀಲಿ,
            &length,
            &ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }

    ಆದರೆ (length != 0) {
        ಆಯ್ಕೆ(value[0])
        {
        ಸ್ಥಿತಿ 0x0062: /* 'b' */
            ಫಲಿತಾಂಶ = ULOC_LAYOUT_BTT;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x006C: /* 'l' */
            ಫಲಿತಾಂಶ = ULOC_LAYOUT_LTR;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x0072: /* 'r' */
            ಫಲಿತಾಂಶ = ULOC_LAYOUT_RTL;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x0074: /* 't' */
            ಫಲಿತಾಂಶ = ULOC_LAYOUT_TTB;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                             UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ _uloc_getOrientationHelper(localeId, "characters", *ಸ್ಥಿತಿ_2);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_ಸಾಲು_Orientation(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeId,
                        UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ _uloc_getOrientationHelper(localeId, "lines", *ಸ್ಥಿತಿ_2);
}
