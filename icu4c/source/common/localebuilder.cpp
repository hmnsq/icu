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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ᨀᨘᨒ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ᨀᨘᨒ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ᨀᨘᨒ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ᨀᨘᨒ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ᨀᨘᨒ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ᨀᨘᨒ ultag_isExtensionSubtags(s, len);
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
    ᨀᨘ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ᨀᨘᨒ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    ᨀᨘ (input.empty()) {
        dest[0] = '\0';
    } ᨀᨘᨁ ᨀᨘ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ᨀᨘᨁ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ᨀᨘᨒ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    for (int32_t i = 0; i < len; i++, data++) {
        ᨀᨘ (*data == '_') {
            *data = '-';
        } ᨀᨘᨁ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    ᨀᨘ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ᨀᨘᨒ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ᨀᨘ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ᨀᨘᨒ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ᨀᨘ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ᨀᨘᨒ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ᨀᨘ (key[1] == '\0') {
        // one char key
        ᨀᨘᨒ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ᨀᨘᨁ ᨀᨘ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ᨀᨘᨒ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ᨀᨘᨒ unicode_locale_key.has_value() &&
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
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ᨀᨘ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ᨀᨘ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ᨀᨘᨒ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ᨀᨛ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
        ᨀᨘ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ᨀᨘ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᨀᨘᨒ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ᨀᨘ (U_FAILURE(errorCode) || iter.isNull()) { ᨀᨘᨒ; }
    const var_na* key;
    ᨀᨛ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
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
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    ᨀᨘ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ *this;
    }
    CharString value_str(value, status_);
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    transform(value_str.data(), value_str.length());
    ᨀᨘ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ *this;
    }
    ᨀᨘ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᨀᨘ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᨀᨘᨒ *this;
        }
    }
    ᨀᨘ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ᨀᨘᨒ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    ᨀᨘ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    ᨀᨘ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ᨀᨘᨒ *this;
    }
    ᨀᨘ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᨀᨘ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᨀᨘᨒ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    transform(value_str.data(), value_str.length());
    ᨀᨘ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ *this;
    }
    ᨀᨘ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᨀᨘ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᨀᨘᨒ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ᨀᨘᨒ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ᨀᨘ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ᨀᨘᨒ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ᨀᨛ (start < limit) {
        ᨀᨘ (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ᨀᨘ (cmp == 0) { ᨀᨘᨒ *this; }  // Found it in attributes: Just return
            ᨀᨘ (cmp > 0) {
                ᨀᨘ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ᨀᨘ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ᨀᨘ (!inserted) {
        ᨀᨘ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᨀᨘ (U_FAILURE(status_)) { ᨀᨘᨒ *this; }
    transform(value_str.data(), value_str.length());
    ᨀᨘ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ *this;
    }
    ᨀᨘ (extensions_ == nullptr) { ᨀᨘᨒ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ᨀᨘ (U_FAILURE(localErrorCode)) { ᨀᨘᨒ *this; }
    // Do not have any attributes, just return.
    ᨀᨘ (attributes.isEmpty()) { ᨀᨘᨒ *this; }

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
    ᨀᨛ (start < limit) {
        ᨀᨘ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ᨀᨘᨁ {
            ᨀᨘ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ᨀᨘ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ᨀᨘᨒ *this;
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
    ᨀᨘᨒ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ᨀᨘᨒ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ᨀᨘᨒ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ᨀᨘ (U_FAILURE(errorCode)) { ᨀᨘᨒ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ᨀᨘ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ᨀᨘᨒ;
    }
    ᨀᨘ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᨀᨘ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᨀᨘᨒ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ᨀᨘ (U_FAILURE(errorCode)) {
        ᨀᨘᨒ makeBogusLocale();
    }
    ᨀᨘ (U_FAILURE(status_)) {
        errorCode = status_;
        ᨀᨘᨒ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ᨀᨘ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ᨀᨘ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ᨀᨘ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ᨀᨘ (U_FAILURE(errorCode)) {
        ᨀᨘᨒ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ᨀᨘ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ᨀᨘ (U_FAILURE(errorCode)) {
        ᨀᨘᨒ makeBogusLocale();
    }
    ᨀᨘᨒ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ᨀᨘ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ᨀᨘᨒ true;
    }
    outErrorCode = status_;
    ᨀᨘᨒ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
