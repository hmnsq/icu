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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ត្រឡប់ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ត្រឡប់ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ត្រឡប់ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ត្រឡប់ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ត្រឡប់ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ត្រឡប់ ultag_isExtensionSubtags(s, len);
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
    ប្រសិនបើ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ត្រឡប់ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    ប្រសិនបើ (input.empty()) {
        dest[0] = '\0';
    } បើមិនដូច្នេះ ប្រសិនបើ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } បើមិនដូច្នេះ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ត្រឡប់ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    សម្រាប់ (int32_t i = 0; i < len; i++, data++) {
        ប្រសិនបើ (*data == '_') {
            *data = '-';
        } បើមិនដូច្នេះ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    ប្រសិនបើ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ត្រឡប់ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ប្រសិនបើ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ប្រសិនបើ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ត្រឡប់ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ប្រសិនបើ (key[1] == '\0') {
        // one char key
        ត្រឡប់ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } បើមិនដូច្នេះ ប្រសិនបើ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ត្រឡប់ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ត្រឡប់ unicode_locale_key.has_value() &&
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
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ប្រសិនបើ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ប្រសិនបើ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ត្រឡប់; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ខណៈពេល ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        ប្រសិនបើ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ប្រសិនបើ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ត្រឡប់;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ប្រសិនបើ (U_FAILURE(errorCode) || iter.isNull()) { ត្រឡប់; }
    const var_na* key;
    ខណៈពេល ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
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
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    ប្រសិនបើ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ *this;
    }
    CharString value_str(value, status_);
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    transform(value_str.data(), value_str.length());
    ប្រសិនបើ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ប្រសិនបើ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ *this;
        }
    }
    ប្រសិនបើ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ត្រឡប់ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    ប្រសិនបើ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    ប្រសិនបើ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ត្រឡប់ *this;
    }
    ប្រសិនបើ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ប្រសិនបើ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    transform(value_str.data(), value_str.length());
    ប្រសិនបើ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ប្រសិនបើ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ត្រឡប់ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ប្រសិនបើ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ត្រឡប់ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ខណៈពេល (start < limit) {
        ប្រសិនបើ (!inserted) {
            ចំនួនគត់ cmp = uprv_strcmp(start, value_str.data());
            ប្រសិនបើ (cmp == 0) { ត្រឡប់ *this; }  // Found it in attributes: Just return
            ប្រសិនបើ (cmp > 0) {
                ប្រសិនបើ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ប្រសិនបើ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ប្រសិនបើ (!inserted) {
        ប្រសិនបើ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ប្រសិនបើ (U_FAILURE(status_)) { ត្រឡប់ *this; }
    transform(value_str.data(), value_str.length());
    ប្រសិនបើ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (extensions_ == nullptr) { ត្រឡប់ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ប្រសិនបើ (U_FAILURE(localErrorCode)) { ត្រឡប់ *this; }
    // Do not have any attributes, just return.
    ប្រសិនបើ (attributes.isEmpty()) { ត្រឡប់ *this; }

    var_na* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    សម្រាប់ (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    ខណៈពេល (start < limit) {
        ប្រសិនបើ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ប្រសិនបើ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ត្រឡប់ *this;
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
    ត្រឡប់ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ត្រឡប់ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ត្រឡប់ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ប្រសិនបើ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ត្រឡប់;
    }
    ប្រសិនបើ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ប្រសិនបើ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់ makeBogusLocale();
    }
    ប្រសិនបើ (U_FAILURE(status_)) {
        errorCode = status_;
        ត្រឡប់ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ប្រសិនបើ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ប្រសិនបើ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ប្រសិនបើ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ប្រសិនបើ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់ makeBogusLocale();
    }
    ត្រឡប់ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ប្រសិនបើ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ត្រឡប់ true;
    }
    outErrorCode = status_;
    ត្រឡប់ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
