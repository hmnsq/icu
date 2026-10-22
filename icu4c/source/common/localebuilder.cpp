// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"  // StringByteSink<CharString>
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "fixedstring.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"

ನಾಮಾಕಾಶ {

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ UPRV_ISDIGIT(ಅಕ್ಷರ c) { ಹಿಂತಿರುಗಿ c >= '0' && c <= '9'; }
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ UPRV_ISALPHANUM(ಅಕ್ಷರ c) { ಹಿಂತಿರುಗಿ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* k_Attribute_ಕೀಲಿ = "attribute";

ತಾರ್ಕಿಕ _isExtensionSubtags(ಅಕ್ಷರ ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len) {
    ಆಯ್ಕೆ (uprv_tolower(ಕೀಲಿ)) {
        ಸ್ಥಿತಿ 'u':
            ಹಿಂತಿರುಗಿ ultag_isUnicodeExtensionSubtags(s, len);
        ಸ್ಥಿತಿ 't':
            ಹಿಂತಿರುಗಿ ultag_isTransformedExtensionSubtags(s, len);
        ಸ್ಥಿತಿ 'x':
            ಹಿಂತಿರುಗಿ ultag_is_Privateuse_ಮೌಲ್ಯ_Subtags(s, len);
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), ಸ್ಥಿತಿ_3(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(ಶೂನ್ಯಸೂಚಕ), extensions_(ಶೂನ್ಯಸೂಚಕ)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    ಅಳಿಸು variant_;
    ಅಳಿಸು extensions_;
}

LocaleBuilder& LocaleBuilder::ಗಣ_Locale(ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND
{
    clear();
    ಗಣ_Language(locale.getLanguage());
    ಗಣ_Script(locale.getScript());
    ಗಣ_Region(locale.getCountry());
    ಗಣ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::ಗಣ_Language_ಟ್ಯಾಗ್(StringPiece ಟ್ಯಾಗ್) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_ಟ್ಯಾಗ್(ಟ್ಯಾಗ್, ಸ್ಥಿತಿ_3);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    ಗಣ_Locale(l);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ ಗಣ_ಕ್ಷೇತ್ರ(StringPiece input, ಅಕ್ಷರ* dest, UError_ಸಂಕೇತ& error_ಸಂಕೇತ,
              ತಾರ್ಕಿಕ (*test)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, int32_t)) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (input.empty()) {
        dest[0] = '\0';
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (test(input.ದತ್ತಾಂಶ(), input.length())) {
        uprv_memcpy(dest, input.ದತ್ತಾಂಶ(), input.length());
        dest[input.length()] = '\0';
    } ಇಲ್ಲದಿದ್ದರೆ {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::ಗಣ_Language(StringPiece language) U_LIFETIME_BOUND
{
    ಗಣ_ಕ್ಷೇತ್ರ(language, language_, ಸ್ಥಿತಿ_3, &ultag_isLanguageSubtag);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::ಗಣ_Script(StringPiece script) U_LIFETIME_BOUND
{
    ಗಣ_ಕ್ಷೇತ್ರ(script, script_, ಸ್ಥಿತಿ_3, &ultag_isScriptSubtag);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::ಗಣ_Region(StringPiece region) U_LIFETIME_BOUND
{
    ಗಣ_ಕ್ಷೇತ್ರ(region, region_, ಸ್ಥಿತಿ_3, &ultag_isRegionSubtag);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ transform(ಅಕ್ಷರ* ದತ್ತಾಂಶ, int32_t len) {
    ಗಾಗಿ (int32_t i = 0; i < len; i++, ದತ್ತಾಂಶ++) {
        ಆದರೆ (*ದತ್ತಾಂಶ == '_') {
            *ದತ್ತಾಂಶ = '-';
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ದತ್ತಾಂಶ = uprv_tolower(*ದತ್ತಾಂಶ);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::ಗಣ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (variant.empty()) {
        ಅಳಿಸು variant_;
        variant_ = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    FixedString* new_variant = ಹೊಸ FixedString(variant);
    ಆದರೆ (new_variant == ಶೂನ್ಯಸೂಚಕ || new_variant->is_ಖಾಲಿಗಳು()) {
        ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    transform(new_variant->getAlias(), variant.length());
    ಆದರೆ (!ultag_isVariantSubtags(new_variant->ದತ್ತಾಂಶ(), variant.length())) {
        ಅಳಿಸು new_variant;
        ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಅಳಿಸು variant_;
    variant_ = new_variant;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ನಾಮಾಕಾಶ {

ತಾರ್ಕಿಕ
is_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value, int32_t ಮೌಲ್ಯ_len)
{
    ಆದರೆ (ಕೀಲಿ[1] == '\0') {
        // one char key
        ಹಿಂತಿರುಗಿ (UPRV_ISALPHANUM(uprv_tolower(ಕೀಲಿ[0])) &&
                _isExtensionSubtags(ಕೀಲಿ[0], value, ಮೌಲ್ಯ_len));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(ಕೀಲಿ, k_Attribute_ಕೀಲಿ) == 0) {
        // unicode attributes
        ಹಿಂತಿರುಗಿ ultag_is_Unicode_Locale_ಗುಣಲಕ್ಷಣಗಳು(value, ಮೌಲ್ಯ_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ಕೀಲಿ = ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ_With_Fallback(ಕೀಲಿ);
    std::optional<std::string_view> unicode_locale_type = ulocimp_ಗಮ್ಯ_Bcp_Type_With_Fallback(ಕೀಲಿ, value);

    ಹಿಂತಿರುಗಿ unicode_locale_ಕೀಲಿ.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ಕೀಲಿ(unicode_locale_ಕೀಲಿ->ದತ್ತಾಂಶ(),
                                    static_cast<int32_t>(unicode_locale_ಕೀಲಿ->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->ದತ್ತಾಂಶ(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

ಶೂನ್ಯ
_copyExtensions(ಸ್ಥಿರಾಂಕ Locale& from, icu::StringEnumeration *keywords,
                Locale& ಗಮ್ಯ, ತಾರ್ಕಿಕ validate, UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    Local_ಸೂಚಕ<icu::StringEnumeration> ownedKeywords;
    ಆದರೆ (keywords == ಶೂನ್ಯಸೂಚಕ) {
        ownedKeywords.adoptInstead(from.createKeywords(error_ಸಂಕೇತ));
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || ownedKeywords.isNull()) { ಹಿಂತಿರುಗಿ; }
        keywords = ownedKeywords.getAlias();
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ;
    ಆಗ ((ಕೀಲಿ = keywords->next(ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ)) != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ವಯಂಚಾಲಿತ value = from.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ಆದರೆ (uprv_strcmp(ಕೀಲಿ, k_Attribute_ಕೀಲಿ) == 0) {
            transform(value.ದತ್ತಾಂಶ(), value.length());
        }
        ಆದರೆ (validate &&
            !is_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, value.ದತ್ತಾಂಶ(), value.length())) {
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಗಮ್ಯ.ಗಣ_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, value.ದತ್ತಾಂಶ(), error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    }
}

ಶೂನ್ಯ
clear_UAttributes_And_ಕೀಲಿ_Type(Locale& locale, UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    // Clear Unicode attributes
    locale.ಗಣ_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, "", error_ಸಂಕೇತ);

    // Clear all Unicode keyword values
    Local_ಸೂಚಕ<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_ಸಂಕೇತ));
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || iter.isNull()) { ಹಿಂತಿರುಗಿ; }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ;
    ಆಗ ((ಕೀಲಿ = iter->next(ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ)) != ಶೂನ್ಯಸೂಚಕ) {
        locale.ಗಣ_Unicode_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    }
}

ಶೂನ್ಯ
ಗಣ_Unicode_Extensions(Locale& locale, ಸ್ಥಿರಾಂಕ CharString& value, UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_ಸಂಕೇತ);
    locale_str.append(value, error_ಸಂಕೇತ);
    _copyExtensions(
        Locale::for_Language_ಟ್ಯಾಗ್(locale_str.ದತ್ತಾಂಶ(), error_ಸಂಕೇತ), ಶೂನ್ಯಸೂಚಕ,
        locale, ಸುಳ್ಳು, error_ಸಂಕೇತ);
}

}  // namespace

LocaleBuilder& LocaleBuilder::ಗಣ_Extension(ಅಕ್ಷರ ಕೀಲಿ, StringPiece value) U_LIFETIME_BOUND
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (!UPRV_ISALPHANUM(ಕೀಲಿ)) {
        ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    CharString ಮೌಲ್ಯ_str(value, ಸ್ಥಿತಿ_3);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    transform(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length());
    ಆದರೆ (!ಮೌಲ್ಯ_str.is_ಖಾಲಿಗಳು() &&
            !_isExtensionSubtags(ಕೀಲಿ, ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length())) {
        ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }
    ಆದರೆ (uprv_tolower(ಕೀಲಿ) != 'u') {
        // for t, x and others extension.
        extensions_->ಗಣ_Keyword_ಮೌಲ್ಯ(StringPiece(&ಕೀಲಿ, 1), ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(),
                                     ಸ್ಥಿತಿ_3);
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    clear_UAttributes_And_ಕೀಲಿ_Type(*extensions_, ಸ್ಥಿತಿ_3);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (!value.empty()) {
        ಗಣ_Unicode_Extensions(*extensions_, ಮೌಲ್ಯ_str, ಸ್ಥಿತಿ_3);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::ಗಣ_Unicode_Locale_Keyword(StringPiece ಕೀಲಿ,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (!ultag_is_Unicode_Locale_ಕೀಲಿ(ಕೀಲಿ.ದತ್ತಾಂಶ(), ಕೀಲಿ.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.ದತ್ತಾಂಶ(), type.length()))) {
      ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }
    extensions_->ಗಣ_Unicode_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, type, ಸ್ಥಿತಿ_3);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::ಕೂಡಿಸು_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ಮೌಲ್ಯ_str(value, ಸ್ಥಿತಿ_3);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    transform(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length());
    ಆದರೆ (!ultag_isUnicodeLocaleAttribute(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length())) {
        ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        extensions_->ಗಣ_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    UError_ಸಂಕೇತ local_Error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಸ್ವಯಂಚಾಲಿತ ಗುಣಲಕ್ಷಣಗಳು = extensions_->get_Keyword_ಮೌಲ್ಯ<CharString>(k_Attribute_ಕೀಲಿ, local_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(local_Error_ಸಂಕೇತ)) {
        CharString new_ಗುಣಲಕ್ಷಣಗಳು(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
        // No attributes, set the attribute.
        extensions_->ಗಣ_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    transform(ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ(),ಗುಣಲಕ್ಷಣಗಳು.length());
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ = ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಎಲ್ಲೆ = ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ() + ಗುಣಲಕ್ಷಣಗಳು.length();
    CharString new_ಗುಣಲಕ್ಷಣಗಳು;
    ತಾರ್ಕಿಕ inserted = ಸುಳ್ಳು;
    ಆಗ (ಆರಂಭ < ಎಲ್ಲೆ) {
        ಆದರೆ (!inserted) {
            ಪೂರ್ಣಾಂಕ cmp = uprv_strcmp(ಆರಂಭ, ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ());
            ಆದರೆ (cmp == 0) { ಹಿಂತಿರುಗಿ *ಇದು; }  // Found it in attributes: Just return
            ಆದರೆ (cmp > 0) {
                ಆದರೆ (!new_ಗುಣಲಕ್ಷಣಗಳು.is_ಖಾಲಿಗಳು()) new_ಗುಣಲಕ್ಷಣಗಳು.append('_', ಸ್ಥಿತಿ_3);
                new_ಗುಣಲಕ್ಷಣಗಳು.append(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
                inserted = ನಿಜ;
            }
        }
        ಆದರೆ (!new_ಗುಣಲಕ್ಷಣಗಳು.is_ಖಾಲಿಗಳು()) {
            new_ಗುಣಲಕ್ಷಣಗಳು.append('_', ಸ್ಥಿತಿ_3);
        }
        new_ಗುಣಲಕ್ಷಣಗಳು.append(ಆರಂಭ, ಸ್ಥಿತಿ_3);
        ಆರಂಭ += uprv_strlen(ಆರಂಭ) + 1;
    }
    ಆದರೆ (!inserted) {
        ಆದರೆ (!new_ಗುಣಲಕ್ಷಣಗಳು.is_ಖಾಲಿಗಳು()) {
            new_ಗುಣಲಕ್ಷಣಗಳು.append('_', ಸ್ಥಿತಿ_3);
        }
        new_ಗುಣಲಕ್ಷಣಗಳು.append(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->ಗಣ_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ಮೌಲ್ಯ_str(value, ಸ್ಥಿತಿ_3);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    transform(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length());
    ಆದರೆ (!ultag_isUnicodeLocaleAttribute(ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ(), ಮೌಲ್ಯ_str.length())) {
        ಸ್ಥಿತಿ_3 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ *ಇದು; }
    UError_ಸಂಕೇತ local_Error_ಸಂಕೇತ = U_ZERO_ERROR;
    ಸ್ವಯಂಚಾಲಿತ ಗುಣಲಕ್ಷಣಗಳು = extensions_->get_Keyword_ಮೌಲ್ಯ<CharString>(k_Attribute_ಕೀಲಿ, local_Error_ಸಂಕೇತ);
    // get failure, just return
    ಆದರೆ (U_FAILURE(local_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    // Do not have any attributes, just return.
    ಆದರೆ (ಗುಣಲಕ್ಷಣಗಳು.is_ಖಾಲಿಗಳು()) { ಹಿಂತಿರುಗಿ *ಇದು; }

    ಅಕ್ಷರ* p = ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ಗಾಗಿ (int32_t i = 0; i < ಗುಣಲಕ್ಷಣಗಳು.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ = ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಎಲ್ಲೆ = ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ() + ಗುಣಲಕ್ಷಣಗಳು.length();
    CharString new_ಗುಣಲಕ್ಷಣಗಳು;
    ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
    ಆಗ (ಆರಂಭ < ಎಲ್ಲೆ) {
        ಆದರೆ (uprv_strcmp(ಆರಂಭ, ಮೌಲ್ಯ_str.ದತ್ತಾಂಶ()) == 0) {
            ಸಿಕ್ಕಿತು = ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (!new_ಗುಣಲಕ್ಷಣಗಳು.is_ಖಾಲಿಗಳು()) {
                new_ಗುಣಲಕ್ಷಣಗಳು.append('_', ಸ್ಥಿತಿ_3);
            }
            new_ಗುಣಲಕ್ಷಣಗಳು.append(ಆರಂಭ, ಸ್ಥಿತಿ_3);
        }
        ಆರಂಭ += uprv_strlen(ಆರಂಭ) + 1;
    }
    // Found the value in attributes, set the attribute.
    ಆದರೆ (ಸಿಕ್ಕಿತು) {
        extensions_->ಗಣ_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_ಗುಣಲಕ್ಷಣಗಳು.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_3);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    ಸ್ಥಿತಿ_3 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    ಅಳಿಸು variant_;
    variant_ = ಶೂನ್ಯಸೂಚಕ;
    clearExtensions();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    ಅಳಿಸು extensions_;
    extensions_ = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.ಗಣ_ಗಮ್ಯ_Bogus();
  ಹಿಂತಿರುಗಿ bogus;
}

ಶೂನ್ಯ LocaleBuilder::copyExtensionsFrom(ಸ್ಥಿರಾಂಕ Locale& src, UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    Local_ಸೂಚಕ<icu::StringEnumeration> keywords(src.createKeywords(error_ಸಂಕೇತ));
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || keywords.isNull() || keywords->count(error_ಸಂಕೇತ) == 0) {
        // Error, or no extensions to copy.
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಆದರೆ (extensions_ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_3 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, ಸುಳ್ಳು, error_ಸಂಕೇತ);
}

Locale LocaleBuilder::build(UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
{
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ makeBogusLocale();
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_3)) {
        error_ಸಂಕೇತ = ಸ್ಥಿತಿ_3;
        ಹಿಂತಿರುಗಿ makeBogusLocale();
    }
    CharString locale_str(language_, error_ಸಂಕೇತ);
    ಆದರೆ (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_ಸಂಕೇತ).append(StringPiece(script_), error_ಸಂಕೇತ);
    }
    ಆದರೆ (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_ಸಂಕೇತ).append(StringPiece(region_), error_ಸಂಕೇತ);
    }
    ಆದರೆ (variant_ != ಶೂನ್ಯಸೂಚಕ) {
        locale_str.append('-', error_ಸಂಕೇತ).append(StringPiece(variant_->ದತ್ತಾಂಶ()), error_ಸಂಕೇತ);
    }
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ makeBogusLocale();
    }
    Locale ಗುಣಲಬ್ಧ(locale_str.ದತ್ತಾಂಶ());
    ಆದರೆ (extensions_ != ಶೂನ್ಯಸೂಚಕ) {
        _copyExtensions(*extensions_, ಶೂನ್ಯಸೂಚಕ, ಗುಣಲಬ್ಧ, ನಿಜ, error_ಸಂಕೇತ);
    }
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ makeBogusLocale();
    }
    ಹಿಂತಿರುಗಿ ಗುಣಲಬ್ಧ;
}

UBool LocaleBuilder::copy_Error_ಗಮ್ಯ(UError_ಸಂಕೇತ &ಹೊರಹೋಗು_Error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಹೊರಹೋಗು_Error_ಸಂಕೇತ)) {
        // Do not overwrite the older error code
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹೊರಹೋಗು_Error_ಸಂಕೇತ = ಸ್ಥಿತಿ_3;
    ಹಿಂತಿರುಗಿ U_FAILURE(ಹೊರಹೋಗು_Error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
