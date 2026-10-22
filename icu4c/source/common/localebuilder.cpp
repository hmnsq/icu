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

ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ UPRV_ISDIGIT(ವರ್ಣ c) { ಪ್ರತ್ಯಾಗಮ c >= '0' && c <= '9'; }
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ UPRV_ISALPHANUM(ವರ್ಣ c) { ಪ್ರತ್ಯಾಗಮ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ಧ್ರುವ ವರ್ಣ* k_Attribute_ಕೀಲಿ = "attribute";

ತಾರ್ಕಿಕ _isExtensionSubtags(ವರ್ಣ ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* s, int32_t len) {
    ವಿಕಲ್ಪ (uprv_tolower(ಕೀಲಿ)) {
        ಪಕ್ಷ 'u':
            ಪ್ರತ್ಯಾಗಮ ultag_isUnicodeExtensionSubtags(s, len);
        ಪಕ್ಷ 't':
            ಪ್ರತ್ಯಾಗಮ ultag_isTransformedExtensionSubtags(s, len);
        ಪಕ್ಷ 'x':
            ಪ್ರತ್ಯಾಗಮ ultag_is_Privateuse_ಮೌಲ್ಯ_Subtags(s, len);
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), status_(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(nullptr), extensions_(nullptr)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    delete variant_;
    delete extensions_;
}

LocaleBuilder& LocaleBuilder::setLocale(ಧ್ರುವ Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    ಯದಿ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ setField(StringPiece input, ವರ್ಣ* dest, UErrorCode& errorCode,
              ತಾರ್ಕಿಕ (*test)(ಧ್ರುವ ವರ್ಣ*, int32_t)) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (input.empty()) {
        dest[0] = '\0';
    } ಅನ್ಯಥಾ ಯದಿ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ಅನ್ಯಥಾ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ transform(ವರ್ಣ* data, int32_t len) {
    ಪ್ರತಿ (int32_t i = 0; i < len; i++, data++) {
        ಯದಿ (*data == '_') {
            *data = '-';
        } ಅನ್ಯಥಾ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    FixedString* new_variant = ನೂತನ FixedString(variant);
    ಯದಿ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    transform(new_variant->getAlias(), variant.length());
    ಯದಿ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    delete variant_;
    variant_ = new_variant;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ನಾಮಾಕಾಶ {

ತಾರ್ಕಿಕ
is_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* value, int32_t ಮೌಲ್ಯ_len)
{
    ಯದಿ (ಕೀಲಿ[1] == '\0') {
        // one char key
        ಪ್ರತ್ಯಾಗಮ (UPRV_ISALPHANUM(uprv_tolower(ಕೀಲಿ[0])) &&
                _isExtensionSubtags(ಕೀಲಿ[0], value, ಮೌಲ್ಯ_len));
    } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(ಕೀಲಿ, k_Attribute_ಕೀಲಿ) == 0) {
        // unicode attributes
        ಪ್ರತ್ಯಾಗಮ ultag_isUnicodeLocaleAttributes(value, ಮೌಲ್ಯ_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ಕೀಲಿ = ulocimp_to_Bcp_ಕೀಲಿ_With_Fallback(ಕೀಲಿ);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(ಕೀಲಿ, value);

    ಪ್ರತ್ಯಾಗಮ unicode_locale_ಕೀಲಿ.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ಕೀಲಿ(unicode_locale_ಕೀಲಿ->data(),
                                    static_cast<int32_t>(unicode_locale_ಕೀಲಿ->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

ಶೂನ್ಯ
_copyExtensions(ಧ್ರುವ Locale& from, icu::StringEnumeration *keywords,
                Locale& to, ತಾರ್ಕಿಕ validate, UErrorCode& errorCode)
{
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ಯದಿ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ಯದಿ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ಪ್ರತ್ಯಾಗಮ; }
        keywords = ownedKeywords.getAlias();
    }
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ;
    ಯಾವತ್ ((ಕೀಲಿ = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        ಯದಿ (uprv_strcmp(ಕೀಲಿ, k_Attribute_ಕೀಲಿ) == 0) {
            transform(value.data(), value.length());
        }
        ಯದಿ (validate &&
            !is_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        to.set_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, value.data(), errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    }
}

ಶೂನ್ಯ
clear_UAttributes_And_ಕೀಲಿ_Type(Locale& locale, UErrorCode& errorCode)
{
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    // Clear Unicode attributes
    locale.set_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ಯದಿ (U_FAILURE(errorCode) || iter.isNull()) { ಪ್ರತ್ಯಾಗಮ; }
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ;
    ಯಾವತ್ ((ಕೀಲಿ = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, nullptr, errorCode);
    }
}

ಶೂನ್ಯ
_setUnicodeExtensions(Locale& locale, ಧ್ರುವ CharString& value, UErrorCode& errorCode)
{
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, ಅಸತ್ಯ, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(ವರ್ಣ ಕೀಲಿ, StringPiece value) U_LIFETIME_BOUND
{
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (!UPRV_ISALPHANUM(ಕೀಲಿ)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    CharString ಮೌಲ್ಯ_str(value, status_);
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    transform(ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length());
    ಯದಿ (!ಮೌಲ್ಯ_str.isEmpty() &&
            !_isExtensionSubtags(ಕೀಲಿ, ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (extensions_ == nullptr) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಯದಿ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }
    ಯದಿ (uprv_tolower(ಕೀಲಿ) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_ಮೌಲ್ಯ(StringPiece(&ಕೀಲಿ, 1), ಮೌಲ್ಯ_str.data(),
                                     status_);
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    clear_UAttributes_And_ಕೀಲಿ_Type(*extensions_, status_);
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, ಮೌಲ್ಯ_str, status_);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece ಕೀಲಿ,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (!ultag_is_Unicode_Locale_ಕೀಲಿ(ಕೀಲಿ.data(), ಕೀಲಿ.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (extensions_ == nullptr) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಯದಿ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }
    extensions_->set_Unicode_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, type, status_);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ಮೌಲ್ಯ_str(value, status_);
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    transform(ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length());
    ಯದಿ (!ultag_isUnicodeLocaleAttribute(ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (extensions_ == nullptr) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಯದಿ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        extensions_->set_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, ಮೌಲ್ಯ_str.data(), status_);
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_ಮೌಲ್ಯ<CharString>(k_Attribute_ಕೀಲಿ, localErrorCode);
    ಯದಿ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(ಮೌಲ್ಯ_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_attributes.data(), status_);
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }

    transform(attributes.data(),attributes.length());
    ಧ್ರುವ ವರ್ಣ* start = attributes.data();
    ಧ್ರುವ ವರ್ಣ* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    ತಾರ್ಕಿಕ inserted = ಅಸತ್ಯ;
    ಯಾವತ್ (start < limit) {
        ಯದಿ (!inserted) {
            ಪೂರ್ಣಾಙ್ಕ cmp = uprv_strcmp(start, ಮೌಲ್ಯ_str.data());
            ಯದಿ (cmp == 0) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }  // Found it in attributes: Just return
            ಯದಿ (cmp > 0) {
                ಯದಿ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(ಮೌಲ್ಯ_str.data(), status_);
                inserted = ಸತ್ಯ;
            }
        }
        ಯದಿ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ಯದಿ (!inserted) {
        ಯದಿ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(ಮೌಲ್ಯ_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_attributes.data(), status_);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ಮೌಲ್ಯ_str(value, status_);
    ಯದಿ (U_FAILURE(status_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    transform(ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length());
    ಯದಿ (!ultag_isUnicodeLocaleAttribute(ಮೌಲ್ಯ_str.data(), ಮೌಲ್ಯ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (extensions_ == nullptr) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_ಮೌಲ್ಯ<CharString>(k_Attribute_ಕೀಲಿ, localErrorCode);
    // get failure, just return
    ಯದಿ (U_FAILURE(localErrorCode)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    // Do not have any attributes, just return.
    ಯದಿ (attributes.isEmpty()) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }

    ವರ್ಣ* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ಪ್ರತಿ (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ಧ್ರುವ ವರ್ಣ* start = attributes.data();
    ಧ್ರುವ ವರ್ಣ* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    ತಾರ್ಕಿಕ found = ಅಸತ್ಯ;
    ಯಾವತ್ (start < limit) {
        ಯದಿ (uprv_strcmp(start, ಮೌಲ್ಯ_str.data()) == 0) {
            found = ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            ಯದಿ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ಯದಿ (found) {
        extensions_->set_Keyword_ಮೌಲ್ಯ(k_Attribute_ಕೀಲಿ, new_attributes.data(), status_);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    status_ = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    delete variant_;
    variant_ = nullptr;
    clearExtensions();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ಪ್ರತ್ಯಾಗಮ bogus;
}

ಶೂನ್ಯ LocaleBuilder::copyExtensionsFrom(ಧ್ರುವ Locale& src, UErrorCode& errorCode)
{
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ಯದಿ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (extensions_ == nullptr) {
        extensions_ = Locale::get_ಬೇರು().clone();
        ಯದಿ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, ಅಸತ್ಯ, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ makeBogusLocale();
    }
    ಯದಿ (U_FAILURE(status_)) {
        errorCode = status_;
        ಪ್ರತ್ಯಾಗಮ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ಯದಿ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ಯದಿ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ಯದಿ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ಯದಿ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, ಸತ್ಯ, errorCode);
    }
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ makeBogusLocale();
    }
    ಪ್ರತ್ಯಾಗಮ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    outErrorCode = status_;
    ಪ್ರತ್ಯಾಗಮ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
