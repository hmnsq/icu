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

inline bool UPRV_ISDIGIT(char c) { върни c >= '0' && c <= '9'; }
inline bool UPRV_ISALPHANUM(char c) { върни uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const char* kAttributeKey = "attribute";

bool _isExtensionSubtags(char key, const char* s, int32_t len) {
    switch (uprv_tolower(key)) {
        case 'u':
            върни ultag_isUnicodeExtensionSubtags(s, len);
        case 't':
            върни ultag_isTransformedExtensionSubtags(s, len);
        case 'x':
            върни ultag_isPrivateuseValueSubtags(s, len);
        default:
            върни ultag_isExtensionSubtags(s, len);
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
    ако (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    върни *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ако (U_FAILURE(status_)) { върни *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    върни *this;
}

namespace {

void setField(StringPiece input, char* dest, UErrorCode& errorCode,
              bool (*test)(const char*, int32_t)) {
    ако (U_FAILURE(errorCode)) { върни; }
    ако (input.empty()) {
        dest[0] = '\0';
    } иначе ако (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } иначе {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    върни *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    върни *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    върни *this;
}

namespace {

void transform(char* data, int32_t len) {
    за (int32_t i = 0; i < len; i++, data++) {
        ако (*data == '_') {
            *data = '-';
        } иначе {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ако (U_FAILURE(status_)) { върни *this; }
    ако (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        върни *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ако (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        върни *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ако (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        върни *this;
    }
    delete variant_;
    variant_ = new_variant;
    върни *this;
}

namespace {

bool
_isKeywordValue(const char* key, const char* value, int32_t value_len)
{
    ако (key[1] == '\0') {
        // one char key
        върни (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } иначе ако (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        върни ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    върни unicode_locale_key.has_value() &&
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
    ако (U_FAILURE(errorCode)) { върни; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ако (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ако (U_FAILURE(errorCode) || ownedKeywords.isNull()) { върни; }
        keywords = ownedKeywords.getAlias();
    }
    const char* key;
    докато ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ако (U_FAILURE(errorCode)) { върни; }
        ако (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ако (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            върни;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ако (U_FAILURE(errorCode)) { върни; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ако (U_FAILURE(errorCode)) { върни; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ако (U_FAILURE(errorCode) || iter.isNull()) { върни; }
    const char* key;
    докато ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ако (U_FAILURE(errorCode)) { върни; }
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
    ако (U_FAILURE(status_)) { върни *this; }
    ако (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        върни *this;
    }
    CharString value_str(value, status_);
    ако (U_FAILURE(status_)) { върни *this; }
    transform(value_str.data(), value_str.length());
    ако (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        върни *this;
    }
    ако (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ако (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            върни *this;
        }
    }
    ако (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        върни *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ако (U_FAILURE(status_)) { върни *this; }
    ако (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    върни *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ако (U_FAILURE(status_)) { върни *this; }
    ако (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      върни *this;
    }
    ако (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ако (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            върни *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    върни *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ако (U_FAILURE(status_)) { върни *this; }
    transform(value_str.data(), value_str.length());
    ако (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        върни *this;
    }
    ако (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ако (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            върни *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        върни *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ако (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        върни *this;
    }

    transform(attributes.data(),attributes.length());
    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    докато (start < limit) {
        ако (!inserted) {
            цяло cmp = uprv_strcmp(start, value_str.data());
            ако (cmp == 0) { върни *this; }  // Found it in attributes: Just return
            ако (cmp > 0) {
                ако (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ако (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ако (!inserted) {
        ако (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    върни *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ако (U_FAILURE(status_)) { върни *this; }
    transform(value_str.data(), value_str.length());
    ако (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        върни *this;
    }
    ако (extensions_ == nullptr) { върни *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ако (U_FAILURE(localErrorCode)) { върни *this; }
    // Do not have any attributes, just return.
    ако (attributes.isEmpty()) { върни *this; }

    char* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    за (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    докато (start < limit) {
        ако (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } иначе {
            ако (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ако (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    върни *this;
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
    върни *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    върни *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  върни bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ако (U_FAILURE(errorCode)) { върни; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ако (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        върни;
    }
    ако (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ако (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            върни;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ако (U_FAILURE(errorCode)) {
        върни makeBogusLocale();
    }
    ако (U_FAILURE(status_)) {
        errorCode = status_;
        върни makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ако (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ако (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ако (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ако (U_FAILURE(errorCode)) {
        върни makeBogusLocale();
    }
    Locale product(locale_str.data());
    ако (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ако (U_FAILURE(errorCode)) {
        върни makeBogusLocale();
    }
    върни product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ако (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        върни true;
    }
    outErrorCode = status_;
    върни U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
