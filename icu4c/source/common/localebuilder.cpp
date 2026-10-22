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

anta_hstha bool UPRV_ISDIGIT(var_na c) { ສົ່ງຄືນ c >= '0' && c <= '9'; }
anta_hstha bool UPRV_ISALPHANUM(var_na c) { ສົ່ງຄືນ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr const var_na* kAttributeKey = "attribute";

bool _isExtensionSubtags(var_na key, const var_na* s, int32_t len) {
    switch (uprv_tolower(key)) {
        pak_sa 'u':
            ສົ່ງຄືນ ultag_isUnicodeExtensionSubtags(s, len);
        pak_sa 't':
            ສົ່ງຄືນ ultag_isTransformedExtensionSubtags(s, len);
        pak_sa 'x':
            ສົ່ງຄືນ ultag_isPrivateuseValueSubtags(s, len);
        default:
            ສົ່ງຄືນ ultag_isExtensionSubtags(s, len);
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
    ຖ້າ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ສົ່ງຄືນ *this;
}

namespace {

void setField(StringPiece input, var_na* dest, UErrorCode& errorCode,
              bool (*test)(const var_na*, int32_t)) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    ຖ້າ (input.empty()) {
        dest[0] = '\0';
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ບໍ່ດັ່ງນັ້ນ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ສົ່ງຄືນ *this;
}

namespace {

void transform(var_na* data, int32_t len) {
    ສຳລັບ (int32_t i = 0; i < len; i++, data++) {
        ຖ້າ (*data == '_') {
            *data = '-';
        } ບໍ່ດັ່ງນັ້ນ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    ຖ້າ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ສົ່ງຄືນ *this;
    }
    FixedString* new_variant = new FixedString(variant);
    ຖ້າ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ *this;
    }
    transform(new_variant->getAlias(), variant.length());
    ຖ້າ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    delete variant_;
    variant_ = new_variant;
    ສົ່ງຄືນ *this;
}

namespace {

bool
_isKeywordValue(const var_na* key, const var_na* value, int32_t value_len)
{
    ຖ້າ (key[1] == '\0') {
        // one char key
        ສົ່ງຄືນ (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        ສົ່ງຄືນ ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    ສົ່ງຄືນ unicode_locale_key.has_value() &&
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
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ຖ້າ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ຖ້າ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ສົ່ງຄືນ; }
        keywords = ownedKeywords.getAlias();
    }
    const var_na* key;
    ໃນຂະນະທີ່ ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        ຖ້າ (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        ຖ້າ (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    }
}

void
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ຖ້າ (U_FAILURE(errorCode) || iter.isNull()) { ສົ່ງຄືນ; }
    const var_na* key;
    ໃນຂະນະທີ່ ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

void
_setUnicodeExtensions(Locale& locale, const CharString& value, UErrorCode& errorCode)
{
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
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
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    ຖ້າ (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    CharString value_str(value, status_);
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    transform(value_str.data(), value_str.length());
    ຖ້າ (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    ຖ້າ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ຖ້າ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ *this;
        }
    }
    ຖ້າ (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        ສົ່ງຄືນ *this;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    ຖ້າ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    ຖ້າ (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ສົ່ງຄືນ *this;
    }
    ຖ້າ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ຖ້າ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ *this;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    transform(value_str.data(), value_str.length());
    ຖ້າ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    ຖ້າ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ຖ້າ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ *this;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        ສົ່ງຄືນ *this;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    ຖ້າ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        ສົ່ງຄືນ *this;
    }

    transform(attributes.data(),attributes.length());
    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    ໃນຂະນະທີ່ (start < limit) {
        ຖ້າ (!inserted) {
            ຈຳນວນເຕັມ cmp = uprv_strcmp(start, value_str.data());
            ຖ້າ (cmp == 0) { ສົ່ງຄືນ *this; }  // Found it in attributes: Just return
            ຖ້າ (cmp > 0) {
                ຖ້າ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        ຖ້າ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ຖ້າ (!inserted) {
        ຖ້າ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    ຖ້າ (U_FAILURE(status_)) { ສົ່ງຄືນ *this; }
    transform(value_str.data(), value_str.length());
    ຖ້າ (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ *this;
    }
    ຖ້າ (extensions_ == nullptr) { ສົ່ງຄືນ *this; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    ຖ້າ (U_FAILURE(localErrorCode)) { ສົ່ງຄືນ *this; }
    // Do not have any attributes, just return.
    ຖ້າ (attributes.isEmpty()) { ສົ່ງຄືນ *this; }

    var_na* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ສຳລັບ (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    const var_na* start = attributes.data();
    const var_na* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    bool found = false;
    ໃນຂະນະທີ່ (start < limit) {
        ຖ້າ (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ຖ້າ (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    ສົ່ງຄືນ *this;
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
    ສົ່ງຄືນ *this;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ສົ່ງຄືນ *this;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ສົ່ງຄືນ bogus;
}

void LocaleBuilder::copyExtensionsFrom(const Locale& src, UErrorCode& errorCode)
{
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ຖ້າ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ສົ່ງຄືນ;
    }
    ຖ້າ (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        ຖ້າ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ makeBogusLocale();
    }
    ຖ້າ (U_FAILURE(status_)) {
        errorCode = status_;
        ສົ່ງຄືນ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ຖ້າ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ຖ້າ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ຖ້າ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ຖ້າ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ makeBogusLocale();
    }
    ສົ່ງຄືນ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) const {
    ຖ້າ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ສົ່ງຄືນ true;
    }
    outErrorCode = status_;
    ສົ່ງຄືນ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
