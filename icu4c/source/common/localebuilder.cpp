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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ပြန်ပေး c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ပြန်ပေး uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ပြန်ပေး ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ပြန်ပေး ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ပြန်ပေး ultag_isPrivateuseValueSubtags(s, len);
        default:
            ပြန်ပေး ultag_isExtensionSubtags(s, len);
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
    အကယ်၍ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ပြန်ပေး *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    အကယ်၍ (input.empty()) {
        dest[0] = '\0';
    } မဟုတ်ပါက အကယ်၍ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } မဟုတ်ပါက {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ပြန်ပေး *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    အတွက် (int32_t i = 0; i < len; i++, data++) {
        အကယ်၍ (*data == '_') {
            *data = '-';
        } မဟုတ်ပါက {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    အကယ်၍ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ပြန်ပေး *this;
    }
    FixedString* new_variant = new FixedString(variant);
    အကယ်၍ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ပြန်ပေး *this;
    }
    transform(new_variant->getAlias(), variant.length());
    အကယ်၍ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    delete variant_;
    variant_ = new_variant;
    ပြန်ပေး *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    အကယ်၍ (key[1] == '\0') {
        // one char key
        ပြန်ပေး (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } မဟုတ်ပါက အကယ်၍ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ပြန်ပေး ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ပြန်ပေး unicode_locale_key.has_value() &&
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
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    အကယ်၍ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        အကယ်၍ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ပြန်ပေး; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    နေစဉ် ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
        အကယ်၍ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        အကယ်၍ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ပြန်ပေး;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    အကယ်၍ (U_FAILURE(errorCode) || iter.isNull()) { ပြန်ပေး; }
    const var_na* key;
    နေစဉ် ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
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
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    အကယ်၍ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    CharString value_str(value, status_);
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    transform(value_str.data(), value_str.length());
    အကယ်၍ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    အကယ်၍ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        အကယ်၍ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး *this;
        }
    }
    အကယ်၍ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ပြန်ပေး *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    အကယ်၍ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    အကယ်၍ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ပြန်ပေး *this;
    }
    အကယ်၍ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        အကယ်၍ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    transform(value_str.data(), value_str.length());
    အကယ်၍ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    အကယ်၍ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        အကယ်၍ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ပြန်ပေး *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    အကယ်၍ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ပြန်ပေး *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    နေစဉ် (start < limit) {
        အကယ်၍ (!inserted) {
            ကိန်းပြည့် cmp = uprv_strcmp(start, value_str.data());
            အကယ်၍ (cmp == 0) { ပြန်ပေး *this; }  // Found it in attributes: Just return
            အကယ်၍ (cmp > 0) {
                အကယ်၍ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        အကယ်၍ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    အကယ်၍ (!inserted) {
        အကယ်၍ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    အကယ်၍ (U_FAILURE(status_)) { ပြန်ပေး *this; }
    transform(value_str.data(), value_str.length());
    အကယ်၍ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး *this;
    }
    အကယ်၍ (extensions_ == nullptr) { ပြန်ပေး *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    အကယ်၍ (U_FAILURE(localErrorCode)) { ပြန်ပေး *this; }
    // Do not have any attributes, just return.
    အကယ်၍ (attributes.isEmpty()) { ပြန်ပေး *this; }

    var_na* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    အတွက် (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    နေစဉ် (start < limit) {
        အကယ်၍ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } မဟုတ်ပါက {
            အကယ်၍ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    အကယ်၍ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ပြန်ပေး *this;
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
    ပြန်ပေး *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ပြန်ပေး *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ပြန်ပေး bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    အကယ်၍ (U_FAILURE(errorCode)) { ပြန်ပေး; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    အကယ်၍ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ပြန်ပေး;
    }
    အကယ်၍ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        အကယ်၍ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ပြန်ပေး;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    အကယ်၍ (U_FAILURE(errorCode)) {
        ပြန်ပေး makeBogusLocale();
    }
    အကယ်၍ (U_FAILURE(status_)) {
        errorCode = status_;
        ပြန်ပေး makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    အကယ်၍ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    အကယ်၍ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    အကယ်၍ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    အကယ်၍ (U_FAILURE(errorCode)) {
        ပြန်ပေး makeBogusLocale();
    }
    Locale product(locale_str.data());
    အကယ်၍ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    အကယ်၍ (U_FAILURE(errorCode)) {
        ပြန်ပေး makeBogusLocale();
    }
    ပြန်ပေး product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    အကယ်၍ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ပြန်ပေး true;
    }
    outErrorCode = status_;
    ပြန်ပေး U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
