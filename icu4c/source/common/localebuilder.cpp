// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include <optional>
#include <string_view>
#include <utility>

#include "bytesinkutil.h"  // StringByteSink<CharString>
#include "charstr.h"
#include "cstring.h"
#include "fixedstring.h"
#include "ulocimp.h"
#include "unicode/localebuilder.h"
#include "unicode/locid.h"

namespace {

anta_hstha bool UPRV_ISDIGIT(var_na c) { ᮝᮜᮤ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ᮝᮜᮤ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ᮝᮜᮤ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ᮝᮜᮤ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ᮝᮜᮤ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ᮝᮜᮤ ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(const Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    ᮚᮦᮔ᮪ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ᮝᮜᮤ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    ᮚᮦᮔ᮪ (input.empty()) {
        dest[0] = '\0';
    } ᮜᮤᮚ ᮚᮦᮔ᮪ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ᮜᮤᮚ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ᮝᮜᮤ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    for (int32_t i = 0; i < len; i++, data++) {
        ᮚᮦᮔ᮪ (*data == '_') {
            *data = '-';
        } ᮜᮤᮚ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    ᮚᮦᮔ᮪ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ᮝᮜᮤ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ᮚᮦᮔ᮪ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ᮚᮦᮔ᮪ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᮝᮜᮤ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ᮝᮜᮤ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ᮚᮦᮔ᮪ (key[1] == '\0') {
        // one char key
        ᮝᮜᮤ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ᮜᮤᮚ ᮚᮦᮔ᮪ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ᮝᮜᮤ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ᮝᮜᮤ unicode_locale_key.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_isUnicodeLocaleKey(unicode_locale_key->data(),
                                    static_cast<int32_t>(unicode_locale_key->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

void
_copyExtensions(const Locale& from, icu::StringEnumeration *keywords,
                Locale& to, bool validate, UErrorCode& errorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ᮚᮦᮔ᮪ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ᮝᮜᮤ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ᮞᮨᮓᮨᮜ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
        ᮚᮦᮔ᮪ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ᮚᮦᮔ᮪ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode) || iter.isNull()) { ᮝᮜᮤ; }
    const var_na* key;
    ᮞᮨᮓᮨᮜ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, false, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(var_na key, StringPiece value) U_LIFETIME_BOUND
{
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    ᮚᮦᮔ᮪ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᮝᮜᮤ *this;
    }
    CharString value_str(value, status_);
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    transform(value_str.data(), value_str.length());
    ᮚᮦᮔ᮪ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᮚᮦᮔ᮪ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ *this;
        }
    }
    ᮚᮦᮔ᮪ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ᮝᮜᮤ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    ᮚᮦᮔ᮪ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    ᮚᮦᮔ᮪ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᮚᮦᮔ᮪ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    transform(value_str.data(), value_str.length());
    ᮚᮦᮔ᮪ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᮚᮦᮔ᮪ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ᮝᮜᮤ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ᮚᮦᮔ᮪ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ᮝᮜᮤ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ᮞᮨᮓᮨᮜ (start < limit) {
        ᮚᮦᮔ᮪ (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ᮚᮦᮔ᮪ (cmp == 0) { ᮝᮜᮤ *this; }  // Found it in attributes: Just return
            ᮚᮦᮔ᮪ (cmp > 0) {
                ᮚᮦᮔ᮪ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ᮚᮦᮔ᮪ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ᮚᮦᮔ᮪ (!inserted) {
        ᮚᮦᮔ᮪ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) { ᮝᮜᮤ *this; }
    transform(value_str.data(), value_str.length());
    ᮚᮦᮔ᮪ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪ (extensions_ == nullptr) { ᮝᮜᮤ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ᮚᮦᮔ᮪ (U_FAILURE(localErrorCode)) { ᮝᮜᮤ *this; }
    // Do not have any attributes, just return.
    ᮚᮦᮔ᮪ (attributes.isEmpty()) { ᮝᮜᮤ *this; }

    var_na* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    for (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    ᮞᮨᮓᮨᮜ (start < limit) {
        ᮚᮦᮔ᮪ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ᮜᮤᮚ {
            ᮚᮦᮔ᮪ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ᮚᮦᮔ᮪ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ᮝᮜᮤ *this;
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
    ᮝᮜᮤ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ᮝᮜᮤ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ᮝᮜᮤ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᮚᮦᮔ᮪ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) {
        ᮝᮜᮤ makeBogusLocale();
    }
    ᮚᮦᮔ᮪ (U_FAILURE(status_)) {
        errorCode = status_;
        ᮝᮜᮤ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ᮚᮦᮔ᮪ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ᮚᮦᮔ᮪ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ᮚᮦᮔ᮪ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) {
        ᮝᮜᮤ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ᮚᮦᮔ᮪ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) {
        ᮝᮜᮤ makeBogusLocale();
    }
    ᮝᮜᮤ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ᮚᮦᮔ᮪ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ᮝᮜᮤ true;
    }
    outErrorCode = status_;
    ᮝᮜᮤ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
