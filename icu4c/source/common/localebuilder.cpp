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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ᰀᰧᰌ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ᰀᰧᰌ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ᰀᰧᰌ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ᰀᰧᰌ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ᰀᰧᰌ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ᰀᰧᰌ ultag_isExtensionSubtags(s, len);
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
    ᰀᰧ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ᰀᰧᰌ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
    ᰀᰧ (input.empty()) {
        dest[0] = '\0';
    } ᰀᰧᰀ ᰀᰧ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ᰀᰧᰀ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ᰀᰧᰌ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    for (int32_t i = 0; i < len; i++, data++) {
        ᰀᰧ (*data == '_') {
            *data = '-';
        } ᰀᰧᰀ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    ᰀᰧ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ᰀᰧᰌ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ᰀᰧ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ᰀᰧᰌ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ᰀᰧ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ᰀᰧᰌ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ᰀᰧ (key[1] == '\0') {
        // one char key
        ᰀᰧᰌ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ᰀᰧᰀ ᰀᰧ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ᰀᰧᰌ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ᰀᰧᰌ unicode_locale_key.has_value() &&
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
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ᰀᰧ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ᰀᰧ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ᰀᰧᰌ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ᰀᰬ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
        ᰀᰧ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ᰀᰧ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᰀᰧᰌ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ᰀᰧ (U_FAILURE(errorCode) || iter.isNull()) { ᰀᰧᰌ; }
    const var_na* key;
    ᰀᰬ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
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
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    ᰀᰧ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    CharString value_str(value, status_);
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    transform(value_str.data(), value_str.length());
    ᰀᰧ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    ᰀᰧ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᰀᰧ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᰀᰧᰌ *this;
        }
    }
    ᰀᰧ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ᰀᰧᰌ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    ᰀᰧ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    ᰀᰧ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ᰀᰧᰌ *this;
    }
    ᰀᰧ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᰀᰧ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᰀᰧᰌ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    transform(value_str.data(), value_str.length());
    ᰀᰧ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    ᰀᰧ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᰀᰧ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᰀᰧᰌ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ᰀᰧᰌ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ᰀᰧ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ᰀᰧᰌ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ᰀᰬ (start < limit) {
        ᰀᰧ (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ᰀᰧ (cmp == 0) { ᰀᰧᰌ *this; }  // Found it in attributes: Just return
            ᰀᰧ (cmp > 0) {
                ᰀᰧ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ᰀᰧ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ᰀᰧ (!inserted) {
        ᰀᰧ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᰀᰧ (U_FAILURE(status_)) { ᰀᰧᰌ *this; }
    transform(value_str.data(), value_str.length());
    ᰀᰧ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ *this;
    }
    ᰀᰧ (extensions_ == nullptr) { ᰀᰧᰌ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ᰀᰧ (U_FAILURE(localErrorCode)) { ᰀᰧᰌ *this; }
    // Do not have any attributes, just return.
    ᰀᰧ (attributes.isEmpty()) { ᰀᰧᰌ *this; }

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
    ᰀᰬ (start < limit) {
        ᰀᰧ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ᰀᰧᰀ {
            ᰀᰧ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ᰀᰧ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ᰀᰧᰌ *this;
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
    ᰀᰧᰌ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ᰀᰧᰌ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ᰀᰧᰌ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ᰀᰧ (U_FAILURE(errorCode)) { ᰀᰧᰌ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ᰀᰧ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ᰀᰧᰌ;
    }
    ᰀᰧ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᰀᰧ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᰀᰧᰌ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ᰀᰧ (U_FAILURE(errorCode)) {
        ᰀᰧᰌ makeBogusLocale();
    }
    ᰀᰧ (U_FAILURE(status_)) {
        errorCode = status_;
        ᰀᰧᰌ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ᰀᰧ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ᰀᰧ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ᰀᰧ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ᰀᰧ (U_FAILURE(errorCode)) {
        ᰀᰧᰌ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ᰀᰧ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ᰀᰧ (U_FAILURE(errorCode)) {
        ᰀᰧᰌ makeBogusLocale();
    }
    ᰀᰧᰌ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ᰀᰧ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ᰀᰧᰌ true;
    }
    outErrorCode = status_;
    ᰀᰧᰌ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
