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

inline bool UPRV_ISDIGIT(char c) { trả_về c >= '0' && c <= '9'; }
inline bool UPRV_ISALPHANUM(char c) { trả_về uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const char* kAttributeKey = "attribute";

bool _isExtensionSubtags(char key, const char* s, int32_t len) {
    switch (uprv_tolower(key)) {
        case 'u':
            trả_về ultag_isUnicodeExtensionSubtags(s, len);
        case 't':
            trả_về ultag_isTransformedExtensionSubtags(s, len);
        case 'x':
            trả_về ultag_isPrivateuseValueSubtags(s, len);
        default:
            trả_về ultag_isExtensionSubtags(s, len);
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
    nếu (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    nếu (U_FAILURE(status_)) { trả_về *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    trả_về *this;
}

namespace {

void setField(StringPiece input, char* dest, UErrorCode& errorCode,
              bool (*test)(const char*, int32_t)) {
    nếu (U_FAILURE(errorCode)) { trả_về; }
    nếu (input.empty()) {
        dest[0] = '\0';
    } không_thì nếu (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } không_thì {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    trả_về *this;
}

namespace {

void transform(char* data, int32_t len) {
    cho (int32_t i = 0; i < len; i++, data++) {
        nếu (*data == '_') {
            *data = '-';
        } không_thì {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    nếu (U_FAILURE(status_)) { trả_về *this; }
    nếu (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        trả_về *this;
    }
    FixedString* new_variant = new FixedString(variant);
    nếu (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        trả_về *this;
    }
    transform(new_variant->getAlias(), variant.length());
    nếu (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    delete variant_;
    variant_ = new_variant;
    trả_về *this;
}

namespace {

bool
_isKeywordValue(const char* key, const char* value, int32_t value_len)
{
    nếu (key[1] == '\0') {
        // one char key
        trả_về (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } không_thì nếu (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        trả_về ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    trả_về unicode_locale_key.has_value() &&
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
    nếu (U_FAILURE(errorCode)) { trả_về; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    nếu (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        nếu (U_FAILURE(errorCode) || ownedKeywords.isNull()) { trả_về; }
        keywords = ownedKeywords.getAlias();
    }
    const char* key;
    trong_khi ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        nếu (U_FAILURE(errorCode)) { trả_về; }
        nếu (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        nếu (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            trả_về;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        nếu (U_FAILURE(errorCode)) { trả_về; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    nếu (U_FAILURE(errorCode)) { trả_về; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    nếu (U_FAILURE(errorCode) || iter.isNull()) { trả_về; }
    const char* key;
    trong_khi ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    nếu (U_FAILURE(errorCode)) { trả_về; }
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
    nếu (U_FAILURE(status_)) { trả_về *this; }
    nếu (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    CharString value_str(value, status_);
    nếu (U_FAILURE(status_)) { trả_về *this; }
    transform(value_str.data(), value_str.length());
    nếu (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    nếu (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        nếu (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            trả_về *this;
        }
    }
    nếu (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        trả_về *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    nếu (U_FAILURE(status_)) { trả_về *this; }
    nếu (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    nếu (U_FAILURE(status_)) { trả_về *this; }
    nếu (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      trả_về *this;
    }
    nếu (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        nếu (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            trả_về *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    nếu (U_FAILURE(status_)) { trả_về *this; }
    transform(value_str.data(), value_str.length());
    nếu (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    nếu (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        nếu (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            trả_về *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        trả_về *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    nếu (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        trả_về *this;
    }

    transform(attributes.data(),attributes.length());
    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    trong_khi (start < limit) {
        nếu (!inserted) {
            số_nguyên cmp = uprv_strcmp(start, value_str.data());
            nếu (cmp == 0) { trả_về *this; }  // Found it in attributes: Just return
            nếu (cmp > 0) {
                nếu (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        nếu (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    nếu (!inserted) {
        nếu (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    nếu (U_FAILURE(status_)) { trả_về *this; }
    transform(value_str.data(), value_str.length());
    nếu (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về *this;
    }
    nếu (extensions_ == nullptr) { trả_về *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    nếu (U_FAILURE(localErrorCode)) { trả_về *this; }
    // Do not have any attributes, just return.
    nếu (attributes.isEmpty()) { trả_về *this; }

    char* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    cho (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const char* start = attributes.data();
    const char* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    trong_khi (start < limit) {
        nếu (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } không_thì {
            nếu (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    nếu (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    trả_về *this;
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
    trả_về *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    trả_về *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  trả_về bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    nếu (U_FAILURE(errorCode)) { trả_về; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    nếu (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        trả_về;
    }
    nếu (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        nếu (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            trả_về;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    nếu (U_FAILURE(errorCode)) {
        trả_về makeBogusLocale();
    }
    nếu (U_FAILURE(status_)) {
        errorCode = status_;
        trả_về makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    nếu (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    nếu (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    nếu (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    nếu (U_FAILURE(errorCode)) {
        trả_về makeBogusLocale();
    }
    Locale product(locale_str.data());
    nếu (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    nếu (U_FAILURE(errorCode)) {
        trả_về makeBogusLocale();
    }
    trả_về product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    nếu (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        trả_về true;
    }
    outErrorCode = status_;
    trả_về U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
