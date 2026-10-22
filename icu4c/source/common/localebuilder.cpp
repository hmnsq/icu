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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ꦮꦭꦶ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ꦮꦭꦶ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ꦮꦭꦶ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ꦮꦭꦶ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ꦮꦭꦶ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ꦮꦭꦶ ultag_isExtensionSubtags(s, len);
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
    ꦪꦼꦤ꧀ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ꦮꦭꦶ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    ꦪꦼꦤ꧀ (input.empty()) {
        dest[0] = '\0';
    } ꦭꦶꦪ ꦪꦼꦤ꧀ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ꦭꦶꦪ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ꦮꦭꦶ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < len; i++, data++) {
        ꦪꦼꦤ꧀ (*data == '_') {
            *data = '-';
        } ꦭꦶꦪ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    ꦪꦼꦤ꧀ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ꦮꦭꦶ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ꦪꦼꦤ꧀ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ꦪꦼꦤ꧀ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ꦮꦭꦶ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ꦪꦼꦤ꧀ (key[1] == '\0') {
        // one char key
        ꦮꦭꦶ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ꦭꦶꦪ ꦪꦼꦤ꧀ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ꦮꦭꦶ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ꦮꦭꦶ unicode_locale_key.has_value() &&
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
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ꦪꦼꦤ꧀ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ꦮꦭꦶ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ꦱꦼꦢꦼꦭ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
        ꦪꦼꦤ꧀ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ꦪꦼꦤ꧀ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode) || iter.isNull()) { ꦮꦭꦶ; }
    const var_na* key;
    ꦱꦼꦢꦼꦭ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
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
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    ꦪꦼꦤ꧀ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ *this;
    }
    CharString value_str(value, status_);
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    transform(value_str.data(), value_str.length());
    ꦪꦼꦤ꧀ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ *this;
    }
    ꦪꦼꦤ꧀ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ꦪꦼꦤ꧀ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ *this;
        }
    }
    ꦪꦼꦤ꧀ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ꦮꦭꦶ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    ꦪꦼꦤ꧀ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    ꦪꦼꦤ꧀ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ꦮꦭꦶ *this;
    }
    ꦪꦼꦤ꧀ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ꦪꦼꦤ꧀ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    transform(value_str.data(), value_str.length());
    ꦪꦼꦤ꧀ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ *this;
    }
    ꦪꦼꦤ꧀ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ꦪꦼꦤ꧀ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ꦮꦭꦶ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ꦪꦼꦤ꧀ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ꦮꦭꦶ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ꦱꦼꦢꦼꦭ (start < limit) {
        ꦪꦼꦤ꧀ (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ꦪꦼꦤ꧀ (cmp == 0) { ꦮꦭꦶ *this; }  // Found it in attributes: Just return
            ꦪꦼꦤ꧀ (cmp > 0) {
                ꦪꦼꦤ꧀ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ꦪꦼꦤ꧀ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ꦪꦼꦤ꧀ (!inserted) {
        ꦪꦼꦤ꧀ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) { ꦮꦭꦶ *this; }
    transform(value_str.data(), value_str.length());
    ꦪꦼꦤ꧀ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ *this;
    }
    ꦪꦼꦤ꧀ (extensions_ == nullptr) { ꦮꦭꦶ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ꦪꦼꦤ꧀ (U_FAILURE(localErrorCode)) { ꦮꦭꦶ *this; }
    // Do not have any attributes, just return.
    ꦪꦼꦤ꧀ (attributes.isEmpty()) { ꦮꦭꦶ *this; }

    var_na* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    ꦱꦼꦢꦼꦭ (start < limit) {
        ꦪꦼꦤ꧀ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ꦪꦼꦤ꧀ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ꦮꦭꦶ *this;
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
    ꦮꦭꦶ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ꦮꦭꦶ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ꦮꦭꦶ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ꦪꦼꦤ꧀ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ makeBogusLocale();
    }
    ꦪꦼꦤ꧀ (U_FAILURE(status_)) {
        errorCode = status_;
        ꦮꦭꦶ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ꦪꦼꦤ꧀ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ꦪꦼꦤ꧀ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ꦪꦼꦤ꧀ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ꦪꦼꦤ꧀ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ makeBogusLocale();
    }
    ꦮꦭꦶ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ꦪꦼꦤ꧀ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ꦮꦭꦶ true;
    }
    outErrorCode = status_;
    ꦮꦭꦶ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
