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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ᤀᤡᤴ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ᤀᤡᤴ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ᤀᤡᤴ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ᤀᤡᤴ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ᤀᤡᤴ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ᤀᤡᤴ ultag_isExtensionSubtags(s, len);
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
    ᤀᤡ᤺ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ᤀᤡᤴ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    ᤀᤡ᤺ (input.empty()) {
        dest[0] = '\0';
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ᤀᤡ᤺ᤆᤡ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ᤀᤡᤴ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    for (int32_t i = 0; i < len; i++, data++) {
        ᤀᤡ᤺ (*data == '_') {
            *data = '-';
        } ᤀᤡ᤺ᤆᤡ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    ᤀᤡ᤺ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ᤀᤡᤴ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ᤀᤡ᤺ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ᤀᤡᤴ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ᤀᤡ᤺ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ᤀᤡᤴ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ᤀᤡ᤺ (key[1] == '\0') {
        // one char key
        ᤀᤡᤴ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ᤀᤡᤴ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ᤀᤡᤴ unicode_locale_key.has_value() &&
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
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ᤀᤡ᤺ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ᤀᤡ᤺ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ᤀᤡᤴ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ᤀᤠ᤺ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
        ᤀᤡ᤺ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ᤀᤡ᤺ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᤀᤡᤴ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ᤀᤡ᤺ (U_FAILURE(errorCode) || iter.isNull()) { ᤀᤡᤴ; }
    const var_na* key;
    ᤀᤠ᤺ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
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
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    ᤀᤡ᤺ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ *this;
    }
    CharString value_str(value, status_);
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    transform(value_str.data(), value_str.length());
    ᤀᤡ᤺ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ *this;
    }
    ᤀᤡ᤺ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᤀᤡ᤺ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ *this;
        }
    }
    ᤀᤡ᤺ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ᤀᤡᤴ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    ᤀᤡ᤺ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    ᤀᤡ᤺ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ᤀᤡᤴ *this;
    }
    ᤀᤡ᤺ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᤀᤡ᤺ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    transform(value_str.data(), value_str.length());
    ᤀᤡ᤺ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ *this;
    }
    ᤀᤡ᤺ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᤀᤡ᤺ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ᤀᤡᤴ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ᤀᤡ᤺ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ᤀᤡᤴ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ᤀᤠ᤺ (start < limit) {
        ᤀᤡ᤺ (!inserted) {
            puur_naa_nka cmp = uprv_strcmp(start, value_str.data());
            ᤀᤡ᤺ (cmp == 0) { ᤀᤡᤴ *this; }  // Found it in attributes: Just return
            ᤀᤡ᤺ (cmp > 0) {
                ᤀᤡ᤺ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ᤀᤡ᤺ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ᤀᤡ᤺ (!inserted) {
        ᤀᤡ᤺ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ᤀᤡ᤺ (U_FAILURE(status_)) { ᤀᤡᤴ *this; }
    transform(value_str.data(), value_str.length());
    ᤀᤡ᤺ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ *this;
    }
    ᤀᤡ᤺ (extensions_ == nullptr) { ᤀᤡᤴ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ᤀᤡ᤺ (U_FAILURE(localErrorCode)) { ᤀᤡᤴ *this; }
    // Do not have any attributes, just return.
    ᤀᤡ᤺ (attributes.isEmpty()) { ᤀᤡᤴ *this; }

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
    ᤀᤠ᤺ (start < limit) {
        ᤀᤡ᤺ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ᤀᤡ᤺ᤆᤡ {
            ᤀᤡ᤺ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ᤀᤡ᤺ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ᤀᤡᤴ *this;
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
    ᤀᤡᤴ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ᤀᤡᤴ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ᤀᤡᤴ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ᤀᤡ᤺ (U_FAILURE(errorCode)) { ᤀᤡᤴ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ᤀᤡ᤺ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ᤀᤡᤴ;
    }
    ᤀᤡ᤺ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ᤀᤡ᤺ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ᤀᤡᤴ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ᤀᤡ᤺ (U_FAILURE(errorCode)) {
        ᤀᤡᤴ makeBogusLocale();
    }
    ᤀᤡ᤺ (U_FAILURE(status_)) {
        errorCode = status_;
        ᤀᤡᤴ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ᤀᤡ᤺ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ᤀᤡ᤺ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ᤀᤡ᤺ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ᤀᤡ᤺ (U_FAILURE(errorCode)) {
        ᤀᤡᤴ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ᤀᤡ᤺ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ᤀᤡ᤺ (U_FAILURE(errorCode)) {
        ᤀᤡᤴ makeBogusLocale();
    }
    ᤀᤡᤴ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ᤀᤡ᤺ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ᤀᤡᤴ true;
    }
    outErrorCode = status_;
    ᤀᤡᤴ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
