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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ပွၵ်ႈ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ပွၵ်ႈ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ပွၵ်ႈ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ပွၵ်ႈ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ပွၵ်ႈ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ပွၵ်ႈ ultag_isExtensionSubtags(s, len);
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
    ပေႃး (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ပွၵ်ႈ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    ပေႃး (input.empty()) {
        dest[0] = '\0';
    } ပေႃးဢမ်ႇ ပေႃး (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ပေႃးဢမ်ႇ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ပွၵ်ႈ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    for (int32_t i = 0; i < len; i++, data++) {
        ပေႃး (*data == '_') {
            *data = '-';
        } ပေႃးဢမ်ႇ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    ပေႃး (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ပွၵ်ႈ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ပေႃး (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ပွၵ်ႈ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ပေႃး (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ပွၵ်ႈ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ပေႃး (key[1] == '\0') {
        // one char key
        ပွၵ်ႈ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ပေႃးဢမ်ႇ ပေႃး (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ပွၵ်ႈ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ပွၵ်ႈ unicode_locale_key.has_value() &&
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
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ပေႃး (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ပေႃး (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ပွၵ်ႈ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ၶၢဝ်း ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
        ပေႃး (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ပေႃး (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ပွၵ်ႈ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ပေႃး (U_FAILURE(errorCode) || iter.isNull()) { ပွၵ်ႈ; }
    const var_na* key;
    ၶၢဝ်း ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
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
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    ပေႃး (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ *this;
    }
    CharString value_str(value, status_);
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    transform(value_str.data(), value_str.length());
    ပေႃး (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ *this;
    }
    ပေႃး (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ပေႃး (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ *this;
        }
    }
    ပေႃး (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ပွၵ်ႈ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    ပေႃး (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    ပေႃး (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ပွၵ်ႈ *this;
    }
    ပေႃး (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ပေႃး (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    transform(value_str.data(), value_str.length());
    ပေႃး (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ *this;
    }
    ပေႃး (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ပေႃး (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ပွၵ်ႈ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ပေႃး (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ပွၵ်ႈ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ၶၢဝ်း (start < limit) {
        ပေႃး (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ပေႃး (cmp == 0) { ပွၵ်ႈ *this; }  // Found it in attributes: Just return
            ပေႃး (cmp > 0) {
                ပေႃး (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ပေႃး (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ပေႃး (!inserted) {
        ပေႃး (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ပေႃး (U_FAILURE(status_)) { ပွၵ်ႈ *this; }
    transform(value_str.data(), value_str.length());
    ပေႃး (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ *this;
    }
    ပေႃး (extensions_ == nullptr) { ပွၵ်ႈ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ပေႃး (U_FAILURE(localErrorCode)) { ပွၵ်ႈ *this; }
    // Do not have any attributes, just return.
    ပေႃး (attributes.isEmpty()) { ပွၵ်ႈ *this; }

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
    ၶၢဝ်း (start < limit) {
        ပေႃး (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ပေႃးဢမ်ႇ {
            ပေႃး (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ပေႃး (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ပွၵ်ႈ *this;
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
    ပွၵ်ႈ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ပွၵ်ႈ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ပွၵ်ႈ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ပေႃး (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ပွၵ်ႈ;
    }
    ပေႃး (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ပေႃး (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ပေႃး (U_FAILURE(errorCode)) {
        ပွၵ်ႈ makeBogusLocale();
    }
    ပေႃး (U_FAILURE(status_)) {
        errorCode = status_;
        ပွၵ်ႈ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ပေႃး (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ပေႃး (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ပေႃး (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ပေႃး (U_FAILURE(errorCode)) {
        ပွၵ်ႈ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ပေႃး (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ပေႃး (U_FAILURE(errorCode)) {
        ပွၵ်ႈ makeBogusLocale();
    }
    ပွၵ်ႈ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ပေႃး (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ပွၵ်ႈ true;
    }
    outErrorCode = status_;
    ပွၵ်ႈ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
