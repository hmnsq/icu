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

inline bool UPRV_ISDIGIT(char c) { 반환 c >= '0' && c <= '9'; }
inline bool UPRV_ISALPHANUM(char c) { 반환 uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const char* kAttributeKey = "attribute";

bool _isExtensionSubtags(char key, const char* s, int32_t len) {
    switch (uprv_tolower(key)) {
        case 'u':
            반환 ultag_isUnicodeExtensionSubtags(s, len);
        case 't':
            반환 ultag_isTransformedExtensionSubtags(s, len);
        case 'x':
            반환 ultag_isPrivateuseValueSubtags(s, len);
        default:
            반환 ultag_isExtensionSubtags(s, len);
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
    만약 (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    반환 *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    만약 (U_FAILURE(status_)) { 반환 *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    반환 *this;
}

namespace {

void setField(StringPiece input, char* dest, UErrorCode& errorCode,
              bool (*test)(const char*, int32_t)) {
    만약 (U_FAILURE(errorCode)) { 반환; }
    만약 (input.empty()) {
        dest[0] = '\0';
    } 아니면 만약 (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } 아니면 {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    반환 *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    반환 *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    반환 *this;
}

namespace {

void transform(char* data, int32_t len) {
    위해 (int32_t i = 0; i < len; i++, data++) {
        만약 (*data == '_') {
            *data = '-';
        } 아니면 {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    만약 (U_FAILURE(status_)) { 반환 *this; }
    만약 (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        반환 *this;
    }
    FixedString* new_variant = new FixedString(variant);
    만약 (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        반환 *this;
    }
    transform(new_variant->getAlias(), variant.length());
    만약 (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        반환 *this;
    }
    delete variant_;
    variant_ = new_variant;
    반환 *this;
}

namespace {

bool
_isKeywordValue(const char* key, const char* value, int32_t value_len)
{
    만약 (key[1] == '\0') {
        // one char key
        반환 (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } 아니면 만약 (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        반환 ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    반환 unicode_locale_key.has_value() &&
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
    만약 (U_FAILURE(errorCode)) { 반환; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    만약 (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        만약 (U_FAILURE(errorCode) || ownedKeywords.isNull()) { 반환; }
        keywords = ownedKeywords.getAlias();
    }
    const char* key;
    동안 ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
        만약 (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        만약 (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            반환;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    만약 (U_FAILURE(errorCode)) { 반환; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    만약 (U_FAILURE(errorCode) || iter.isNull()) { 반환; }
    const char* key;
    동안 ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    만약 (U_FAILURE(errorCode)) { 반환; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, false, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(char key, StringPiece value) U_LIFETIME_BOUND
{
    만약 (U_FAILURE(status_)) { 반환 *this; }
    만약 (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        반환 *this;
    }
    CharString value_str(value, status_);
    만약 (U_FAILURE(status_)) { 반환 *this; }
    transform(value_str.data(), value_str.length());
    만약 (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        반환 *this;
    }
    만약 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        만약 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            반환 *this;
        }
    }
    만약 (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        반환 *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    만약 (U_FAILURE(status_)) { 반환 *this; }
    만약 (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    반환 *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    만약 (U_FAILURE(status_)) { 반환 *this; }
    만약 (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      반환 *this;
    }
    만약 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        만약 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            반환 *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    반환 *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    만약 (U_FAILURE(status_)) { 반환 *this; }
    transform(value_str.data(), value_str.length());
    만약 (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        반환 *this;
    }
    만약 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        만약 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            반환 *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        반환 *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    만약 (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        반환 *this;
    }

    transform(attributes.data(),attributes.length());
    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    동안 (start < limit) {
        만약 (!inserted) {
            정수 cmp = uprv_strcmp(start, value_str.data());
            만약 (cmp == 0) { 반환 *this; }  // Found it in attributes: Just return
            만약 (cmp > 0) {
                만약 (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        만약 (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    만약 (!inserted) {
        만약 (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    반환 *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    만약 (U_FAILURE(status_)) { 반환 *this; }
    transform(value_str.data(), value_str.length());
    만약 (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        반환 *this;
    }
    만약 (extensions_ == nullptr) { 반환 *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    만약 (U_FAILURE(localErrorCode)) { 반환 *this; }
    // Do not have any attributes, just return.
    만약 (attributes.isEmpty()) { 반환 *this; }

    char* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    위해 (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    동안 (start < limit) {
        만약 (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } 아니면 {
            만약 (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    만약 (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    반환 *this;
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
    반환 *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    반환 *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  반환 bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    만약 (U_FAILURE(errorCode)) { 반환; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    만약 (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        반환;
    }
    만약 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        만약 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            반환;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    만약 (U_FAILURE(errorCode)) {
        반환 makeBogusLocale();
    }
    만약 (U_FAILURE(status_)) {
        errorCode = status_;
        반환 makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    만약 (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    만약 (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    만약 (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    만약 (U_FAILURE(errorCode)) {
        반환 makeBogusLocale();
    }
    Locale product(locale_str.data());
    만약 (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    만약 (U_FAILURE(errorCode)) {
        반환 makeBogusLocale();
    }
    반환 product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    만약 (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        반환 true;
    }
    outErrorCode = status_;
    반환 U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
