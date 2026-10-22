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

inline bool UPRV_ISDIGIT(char c) { вернуть c >= '0' && c <= '9'; }
inline bool UPRV_ISALPHANUM(char c) { вернуть uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const char* kAttributeKey = "attribute";

bool _isExtensionSubtags(char key, const char* s, int32_t len) {
    switch (uprv_tolower(key)) {
        case 'u':
            вернуть ultag_isUnicodeExtensionSubtags(s, len);
        case 't':
            вернуть ultag_isTransformedExtensionSubtags(s, len);
        case 'x':
            вернуть ultag_isPrivateuseValueSubtags(s, len);
        default:
            вернуть ultag_isExtensionSubtags(s, len);
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
    если (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    если (U_FAILURE(status_)) { вернуть *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    вернуть *this;
}

namespace {

void setField(StringPiece input, char* dest, UErrorCode& errorCode,
              bool (*test)(const char*, int32_t)) {
    если (U_FAILURE(errorCode)) { вернуть; }
    если (input.empty()) {
        dest[0] = '\0';
    } иначе если (test(input.data(), input.length())) {
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
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    вернуть *this;
}

namespace {

void transform(char* data, int32_t len) {
    для (int32_t i = 0; i < len; i++, data++) {
        если (*data == '_') {
            *data = '-';
        } иначе {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    если (U_FAILURE(status_)) { вернуть *this; }
    если (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        вернуть *this;
    }
    FixedString* new_variant = new FixedString(variant);
    если (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        вернуть *this;
    }
    transform(new_variant->getAlias(), variant.length());
    если (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    delete variant_;
    variant_ = new_variant;
    вернуть *this;
}

namespace {

bool
_isKeywordValue(const char* key, const char* value, int32_t value_len)
{
    если (key[1] == '\0') {
        // one char key
        вернуть (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } иначе если (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        вернуть ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    вернуть unicode_locale_key.has_value() &&
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
    если (U_FAILURE(errorCode)) { вернуть; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    если (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        если (U_FAILURE(errorCode) || ownedKeywords.isNull()) { вернуть; }
        keywords = ownedKeywords.getAlias();
    }
    const char* key;
    пока ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        если (U_FAILURE(errorCode)) { вернуть; }
        если (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        если (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        если (U_FAILURE(errorCode)) { вернуть; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    если (U_FAILURE(errorCode)) { вернуть; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    если (U_FAILURE(errorCode) || iter.isNull()) { вернуть; }
    const char* key;
    пока ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    если (U_FAILURE(errorCode)) { вернуть; }
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
    если (U_FAILURE(status_)) { вернуть *this; }
    если (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    CharString value_str(value, status_);
    если (U_FAILURE(status_)) { вернуть *this; }
    transform(value_str.data(), value_str.length());
    если (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    если (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        если (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            вернуть *this;
        }
    }
    если (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        вернуть *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    если (U_FAILURE(status_)) { вернуть *this; }
    если (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    если (U_FAILURE(status_)) { вернуть *this; }
    если (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      вернуть *this;
    }
    если (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        если (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            вернуть *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    если (U_FAILURE(status_)) { вернуть *this; }
    transform(value_str.data(), value_str.length());
    если (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    если (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        если (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            вернуть *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        вернуть *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    если (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        вернуть *this;
    }

    transform(attributes.data(),attributes.length());
    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    пока (start < limit) {
        если (!inserted) {
            целое cmp = uprv_strcmp(start, value_str.data());
            если (cmp == 0) { вернуть *this; }  // Found it in attributes: Just return
            если (cmp > 0) {
                если (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        если (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    если (!inserted) {
        если (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    если (U_FAILURE(status_)) { вернуть *this; }
    transform(value_str.data(), value_str.length());
    если (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть *this;
    }
    если (extensions_ == nullptr) { вернуть *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    если (U_FAILURE(localErrorCode)) { вернуть *this; }
    // Do not have any attributes, just return.
    если (attributes.isEmpty()) { вернуть *this; }

    char* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    для (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    пока (start < limit) {
        если (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } иначе {
            если (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    если (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    вернуть *this;
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
    вернуть *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    вернуть *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  вернуть bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    если (U_FAILURE(errorCode)) { вернуть; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    если (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        вернуть;
    }
    если (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        если (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            вернуть;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    если (U_FAILURE(errorCode)) {
        вернуть makeBogusLocale();
    }
    если (U_FAILURE(status_)) {
        errorCode = status_;
        вернуть makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    если (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    если (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    если (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    если (U_FAILURE(errorCode)) {
        вернуть makeBogusLocale();
    }
    Locale product(locale_str.data());
    если (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    если (U_FAILURE(errorCode)) {
        вернуть makeBogusLocale();
    }
    вернуть product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    если (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        вернуть true;
    }
    outErrorCode = status_;
    вернуть U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
