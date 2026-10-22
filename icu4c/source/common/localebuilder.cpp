// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"  // StringByteSink<CharString>
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fixedstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"

ਨਾਮਾਕਾਸ਼ {

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ UPRV_ISDIGIT(ਅੱਖਰ c) { ਵਾਪਸ c >= '0' && c <= '9'; }
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ UPRV_ISALPHANUM(ਅੱਖਰ c) { ਵਾਪਸ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ਅਚਲ ਅੱਖਰ* k_Attribute_ਕੁੰਜੀ = "attribute";

ਤਾਰਕਿਕ _isExtensionSubtags(ਅੱਖਰ ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* s, int32_t len) {
    ਚੋਣ (uprv_tolower(ਕੁੰਜੀ)) {
        ਹਾਲਤ 'u':
            ਵਾਪਸ ultag_isUnicodeExtensionSubtags(s, len);
        ਹਾਲਤ 't':
            ਵਾਪਸ ultag_isTransformedExtensionSubtags(s, len);
        ਹਾਲਤ 'x':
            ਵਾਪਸ ultag_is_Privateuse_ਮੁੱਲ_Subtags(s, len);
        ਮੂਲ:
            ਵਾਪਸ ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), ਸਥਿਤੀ_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(ਖਾਲੀ_ਸੂਚਕ), extensions_(ਖਾਲੀ_ਸੂਚਕ)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    ਮਿਟਾਓ variant_;
    ਮਿਟਾਓ extensions_;
}

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Locale(ਅਚਲ Locale& locale) U_LIFETIME_BOUND
{
    clear();
    ਸੈੱਟ_Language(locale.getLanguage());
    ਸੈੱਟ_Script(locale.getScript());
    ਸੈੱਟ_Region(locale.getCountry());
    ਸੈੱਟ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Language_ਟੈਗ(StringPiece ਟੈਗ) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_ਟੈਗ(ਟੈਗ, ਸਥਿਤੀ_2);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    ਸੈੱਟ_Locale(l);
    ਵਾਪਸ *ਇਹ;
}

ਨਾਮਾਕਾਸ਼ {

ਖਾਲੀ ਸੈੱਟ_ਖੇਤਰ(StringPiece input, ਅੱਖਰ* dest, UError_ਕੋਡ& error_ਕੋਡ,
              ਤਾਰਕਿਕ (*test)(ਅਚਲ ਅੱਖਰ*, int32_t)) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਜੇ (input.empty()) {
        dest[0] = '\0';
    } ਨਹੀਂਤਾਂ ਜੇ (test(input.ਅੰਕੜੇ(), input.length())) {
        uprv_memcpy(dest, input.ਅੰਕੜੇ(), input.length());
        dest[input.length()] = '\0';
    } ਨਹੀਂਤਾਂ {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Language(StringPiece language) U_LIFETIME_BOUND
{
    ਸੈੱਟ_ਖੇਤਰ(language, language_, ਸਥਿਤੀ_2, &ultag_isLanguageSubtag);
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Script(StringPiece script) U_LIFETIME_BOUND
{
    ਸੈੱਟ_ਖੇਤਰ(script, script_, ਸਥਿਤੀ_2, &ultag_isScriptSubtag);
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Region(StringPiece region) U_LIFETIME_BOUND
{
    ਸੈੱਟ_ਖੇਤਰ(region, region_, ਸਥਿਤੀ_2, &ultag_isRegionSubtag);
    ਵਾਪਸ *ਇਹ;
}

ਨਾਮਾਕਾਸ਼ {

ਖਾਲੀ transform(ਅੱਖਰ* ਅੰਕੜੇ, int32_t len) {
    ਲਈ (int32_t i = 0; i < len; i++, ਅੰਕੜੇ++) {
        ਜੇ (*ਅੰਕੜੇ == '_') {
            *ਅੰਕੜੇ = '-';
        } ਨਹੀਂਤਾਂ {
            *ਅੰਕੜੇ = uprv_tolower(*ਅੰਕੜੇ);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (variant.empty()) {
        ਮਿਟਾਓ variant_;
        variant_ = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ *ਇਹ;
    }
    FixedString* new_variant = ਨਵਾਂ FixedString(variant);
    ਜੇ (new_variant == ਖਾਲੀ_ਸੂਚਕ || new_variant->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    transform(new_variant->getAlias(), variant.length());
    ਜੇ (!ultag_isVariantSubtags(new_variant->ਅੰਕੜੇ(), variant.length())) {
        ਮਿਟਾਓ new_variant;
        ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਮਿਟਾਓ variant_;
    variant_ = new_variant;
    ਵਾਪਸ *ਇਹ;
}

ਨਾਮਾਕਾਸ਼ {

ਤਾਰਕਿਕ
is_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* value, int32_t ਮੁੱਲ_len)
{
    ਜੇ (ਕੁੰਜੀ[1] == '\0') {
        // one char key
        ਵਾਪਸ (UPRV_ISALPHANUM(uprv_tolower(ਕੁੰਜੀ[0])) &&
                _isExtensionSubtags(ਕੁੰਜੀ[0], value, ਮੁੱਲ_len));
    } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(ਕੁੰਜੀ, k_Attribute_ਕੁੰਜੀ) == 0) {
        // unicode attributes
        ਵਾਪਸ ultag_is_Unicode_Locale_ਗੁਣ(value, ਮੁੱਲ_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ਕੁੰਜੀ = ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ_With_Fallback(ਕੁੰਜੀ);
    std::optional<std::string_view> unicode_locale_type = ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type_With_Fallback(ਕੁੰਜੀ, value);

    ਵਾਪਸ unicode_locale_ਕੁੰਜੀ.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ਕੁੰਜੀ(unicode_locale_ਕੁੰਜੀ->ਅੰਕੜੇ(),
                                    static_cast<int32_t>(unicode_locale_ਕੁੰਜੀ->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->ਅੰਕੜੇ(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

ਖਾਲੀ
_copyExtensions(ਅਚਲ Locale& from, icu::StringEnumeration *keywords,
                Locale& ਮੰਜ਼ਿਲ, ਤਾਰਕਿਕ validate, UError_ਕੋਡ& error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    Local_ਸੂਚਕ<icu::StringEnumeration> ownedKeywords;
    ਜੇ (keywords == ਖਾਲੀ_ਸੂਚਕ) {
        ownedKeywords.adoptInstead(from.createKeywords(error_ਕੋਡ));
        ਜੇ (U_FAILURE(error_ਕੋਡ) || ownedKeywords.isNull()) { ਵਾਪਸ; }
        keywords = ownedKeywords.getAlias();
    }
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ;
    ਜਦੋਂ ((ਕੁੰਜੀ = keywords->next(ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਸਵੈਚਾਲਿਤ value = from.get_Keyword_ਮੁੱਲ<CharString>(ਕੁੰਜੀ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਜੇ (uprv_strcmp(ਕੁੰਜੀ, k_Attribute_ਕੁੰਜੀ) == 0) {
            transform(value.ਅੰਕੜੇ(), value.length());
        }
        ਜੇ (validate &&
            !is_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, value.ਅੰਕੜੇ(), value.length())) {
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        ਮੰਜ਼ਿਲ.ਸੈੱਟ_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, value.ਅੰਕੜੇ(), error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    }
}

ਖਾਲੀ
clear_UAttributes_And_ਕੁੰਜੀ_Type(Locale& locale, UError_ਕੋਡ& error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    // Clear Unicode attributes
    locale.ਸੈੱਟ_Keyword_ਮੁੱਲ(k_Attribute_ਕੁੰਜੀ, "", error_ਕੋਡ);

    // Clear all Unicode keyword values
    Local_ਸੂਚਕ<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_ਕੋਡ));
    ਜੇ (U_FAILURE(error_ਕੋਡ) || iter.isNull()) { ਵਾਪਸ; }
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ;
    ਜਦੋਂ ((ਕੁੰਜੀ = iter->next(ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ)) != ਖਾਲੀ_ਸੂਚਕ) {
        locale.ਸੈੱਟ_Unicode_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    }
}

ਖਾਲੀ
ਸੈੱਟ_Unicode_Extensions(Locale& locale, ਅਚਲ CharString& value, UError_ਕੋਡ& error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_ਕੋਡ);
    locale_str.append(value, error_ਕੋਡ);
    _copyExtensions(
        Locale::for_Language_ਟੈਗ(locale_str.ਅੰਕੜੇ(), error_ਕੋਡ), ਖਾਲੀ_ਸੂਚਕ,
        locale, ਝੂਠ, error_ਕੋਡ);
}

}  // namespace

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Extension(ਅੱਖਰ ਕੁੰਜੀ, StringPiece value) U_LIFETIME_BOUND
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (!UPRV_ISALPHANUM(ਕੁੰਜੀ)) {
        ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    CharString ਮੁੱਲ_str(value, ਸਥਿਤੀ_2);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    transform(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length());
    ਜੇ (!ਮੁੱਲ_str.is_ਖਾਲੀ_ਥਾਵਾਂ() &&
            !_isExtensionSubtags(ਕੁੰਜੀ, ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length())) {
        ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
        extensions_ = Locale::get_ਜੜ੍ਹ().clone();
        ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
    }
    ਜੇ (uprv_tolower(ਕੁੰਜੀ) != 'u') {
        // for t, x and others extension.
        extensions_->ਸੈੱਟ_Keyword_ਮੁੱਲ(StringPiece(&ਕੁੰਜੀ, 1), ਮੁੱਲ_str.ਅੰਕੜੇ(),
                                     ਸਥਿਤੀ_2);
        ਵਾਪਸ *ਇਹ;
    }
    clear_UAttributes_And_ਕੁੰਜੀ_Type(*extensions_, ਸਥਿਤੀ_2);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (!value.empty()) {
        ਸੈੱਟ_Unicode_Extensions(*extensions_, ਮੁੱਲ_str, ਸਥਿਤੀ_2);
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::ਸੈੱਟ_Unicode_Locale_Keyword(StringPiece ਕੁੰਜੀ,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (!ultag_is_Unicode_Locale_ਕੁੰਜੀ(ਕੁੰਜੀ.ਅੰਕੜੇ(), ਕੁੰਜੀ.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.ਅੰਕੜੇ(), type.length()))) {
      ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
      ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
        extensions_ = Locale::get_ਜੜ੍ਹ().clone();
        ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
    }
    extensions_->ਸੈੱਟ_Unicode_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, type, ਸਥਿਤੀ_2);
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::ਜੋੜੋ_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ਮੁੱਲ_str(value, ਸਥਿਤੀ_2);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    transform(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length());
    ਜੇ (!ultag_isUnicodeLocaleAttribute(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length())) {
        ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
        extensions_ = Locale::get_ਜੜ੍ਹ().clone();
        ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
        extensions_->ਸੈੱਟ_Keyword_ਮੁੱਲ(k_Attribute_ਕੁੰਜੀ, ਮੁੱਲ_str.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
        ਵਾਪਸ *ਇਹ;
    }

    UError_ਕੋਡ local_Error_ਕੋਡ = U_ZERO_ERROR;
    ਸਵੈਚਾਲਿਤ ਗੁਣ = extensions_->get_Keyword_ਮੁੱਲ<CharString>(k_Attribute_ਕੁੰਜੀ, local_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(local_Error_ਕੋਡ)) {
        CharString new_ਗੁਣ(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
        // No attributes, set the attribute.
        extensions_->ਸੈੱਟ_Keyword_ਮੁੱਲ(k_Attribute_ਕੁੰਜੀ, new_ਗੁਣ.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
        ਵਾਪਸ *ਇਹ;
    }

    transform(ਗੁਣ.ਅੰਕੜੇ(),ਗੁਣ.length());
    ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ = ਗੁਣ.ਅੰਕੜੇ();
    ਅਚਲ ਅੱਖਰ* ਹੱਦਬੰਦੀ = ਗੁਣ.ਅੰਕੜੇ() + ਗੁਣ.length();
    CharString new_ਗੁਣ;
    ਤਾਰਕਿਕ inserted = ਝੂਠ;
    ਜਦੋਂ (ਸ਼ੁਰੂ < ਹੱਦਬੰਦੀ) {
        ਜੇ (!inserted) {
            ਪੂਰਨ cmp = uprv_strcmp(ਸ਼ੁਰੂ, ਮੁੱਲ_str.ਅੰਕੜੇ());
            ਜੇ (cmp == 0) { ਵਾਪਸ *ਇਹ; }  // Found it in attributes: Just return
            ਜੇ (cmp > 0) {
                ਜੇ (!new_ਗੁਣ.is_ਖਾਲੀ_ਥਾਵਾਂ()) new_ਗੁਣ.append('_', ਸਥਿਤੀ_2);
                new_ਗੁਣ.append(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
                inserted = ਸੱਚ;
            }
        }
        ਜੇ (!new_ਗੁਣ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            new_ਗੁਣ.append('_', ਸਥਿਤੀ_2);
        }
        new_ਗੁਣ.append(ਸ਼ੁਰੂ, ਸਥਿਤੀ_2);
        ਸ਼ੁਰੂ += uprv_strlen(ਸ਼ੁਰੂ) + 1;
    }
    ਜੇ (!inserted) {
        ਜੇ (!new_ਗੁਣ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            new_ਗੁਣ.append('_', ਸਥਿਤੀ_2);
        }
        new_ਗੁਣ.append(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->ਸੈੱਟ_Keyword_ਮੁੱਲ(k_Attribute_ਕੁੰਜੀ, new_ਗੁਣ.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ਮੁੱਲ_str(value, ਸਥਿਤੀ_2);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) { ਵਾਪਸ *ਇਹ; }
    transform(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length());
    ਜੇ (!ultag_isUnicodeLocaleAttribute(ਮੁੱਲ_str.ਅੰਕੜੇ(), ਮੁੱਲ_str.length())) {
        ਸਥਿਤੀ_2 = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ *ਇਹ; }
    UError_ਕੋਡ local_Error_ਕੋਡ = U_ZERO_ERROR;
    ਸਵੈਚਾਲਿਤ ਗੁਣ = extensions_->get_Keyword_ਮੁੱਲ<CharString>(k_Attribute_ਕੁੰਜੀ, local_Error_ਕੋਡ);
    // get failure, just return
    ਜੇ (U_FAILURE(local_Error_ਕੋਡ)) { ਵਾਪਸ *ਇਹ; }
    // Do not have any attributes, just return.
    ਜੇ (ਗੁਣ.is_ਖਾਲੀ_ਥਾਵਾਂ()) { ਵਾਪਸ *ਇਹ; }

    ਅੱਖਰ* p = ਗੁਣ.ਅੰਕੜੇ();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ਲਈ (int32_t i = 0; i < ਗੁਣ.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ = ਗੁਣ.ਅੰਕੜੇ();
    ਅਚਲ ਅੱਖਰ* ਹੱਦਬੰਦੀ = ਗੁਣ.ਅੰਕੜੇ() + ਗੁਣ.length();
    CharString new_ਗੁਣ;
    ਤਾਰਕਿਕ ਮਿਲਿਆ = ਝੂਠ;
    ਜਦੋਂ (ਸ਼ੁਰੂ < ਹੱਦਬੰਦੀ) {
        ਜੇ (uprv_strcmp(ਸ਼ੁਰੂ, ਮੁੱਲ_str.ਅੰਕੜੇ()) == 0) {
            ਮਿਲਿਆ = ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (!new_ਗੁਣ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                new_ਗੁਣ.append('_', ਸਥਿਤੀ_2);
            }
            new_ਗੁਣ.append(ਸ਼ੁਰੂ, ਸਥਿਤੀ_2);
        }
        ਸ਼ੁਰੂ += uprv_strlen(ਸ਼ੁਰੂ) + 1;
    }
    // Found the value in attributes, set the attribute.
    ਜੇ (ਮਿਲਿਆ) {
        extensions_->ਸੈੱਟ_Keyword_ਮੁੱਲ(k_Attribute_ਕੁੰਜੀ, new_ਗੁਣ.ਅੰਕੜੇ(), ਸਥਿਤੀ_2);
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    ਸਥਿਤੀ_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    ਮਿਟਾਓ variant_;
    variant_ = ਖਾਲੀ_ਸੂਚਕ;
    clearExtensions();
    ਵਾਪਸ *ਇਹ;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    ਮਿਟਾਓ extensions_;
    extensions_ = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ *ਇਹ;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  ਵਾਪਸ bogus;
}

ਖਾਲੀ LocaleBuilder::copyExtensionsFrom(ਅਚਲ Locale& src, UError_ਕੋਡ& error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    Local_ਸੂਚਕ<icu::StringEnumeration> keywords(src.createKeywords(error_ਕੋਡ));
    ਜੇ (U_FAILURE(error_ਕੋਡ) || keywords.isNull() || keywords->count(error_ਕੋਡ) == 0) {
        // Error, or no extensions to copy.
        ਵਾਪਸ;
    }
    ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
        extensions_ = Locale::get_ਜੜ੍ਹ().clone();
        ਜੇ (extensions_ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, ਝੂਠ, error_ਕੋਡ);
}

Locale LocaleBuilder::build(UError_ਕੋਡ& error_ਕੋਡ)
{
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ makeBogusLocale();
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ_2)) {
        error_ਕੋਡ = ਸਥਿਤੀ_2;
        ਵਾਪਸ makeBogusLocale();
    }
    CharString locale_str(language_, error_ਕੋਡ);
    ਜੇ (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_ਕੋਡ).append(StringPiece(script_), error_ਕੋਡ);
    }
    ਜੇ (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_ਕੋਡ).append(StringPiece(region_), error_ਕੋਡ);
    }
    ਜੇ (variant_ != ਖਾਲੀ_ਸੂਚਕ) {
        locale_str.append('-', error_ਕੋਡ).append(StringPiece(variant_->ਅੰਕੜੇ()), error_ਕੋਡ);
    }
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ makeBogusLocale();
    }
    Locale ਗੁਣਨਫਲ(locale_str.ਅੰਕੜੇ());
    ਜੇ (extensions_ != ਖਾਲੀ_ਸੂਚਕ) {
        _copyExtensions(*extensions_, ਖਾਲੀ_ਸੂਚਕ, ਗੁਣਨਫਲ, ਸੱਚ, error_ਕੋਡ);
    }
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ makeBogusLocale();
    }
    ਵਾਪਸ ਗੁਣਨਫਲ;
}

UBool LocaleBuilder::copy_Error_ਮੰਜ਼ਿਲ(UError_ਕੋਡ &ਬਾਹਰਲਾ_Error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਬਾਹਰਲਾ_Error_ਕੋਡ)) {
        // Do not overwrite the older error code
        ਵਾਪਸ ਸੱਚ;
    }
    ਬਾਹਰਲਾ_Error_ਕੋਡ = ਸਥਿਤੀ_2;
    ਵਾਪਸ U_FAILURE(ਬਾਹਰਲਾ_Error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
