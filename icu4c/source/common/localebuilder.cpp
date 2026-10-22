// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <optional>
#ضمن <string_view>
#ضمن <utility>

#ضمن "bytesinkutil.h"  // StringByteSink<CharString>
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "ulocimp.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"

نطاق {

مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* kAttributeKey = "attribute";

منطقي _isExtensionSubtags(حرف key, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(key)) {
        حالة 'u':
            أرجع ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            أرجع ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            أرجع ultag_isPrivateuseValueSubtags(s, len);
        افتراضي:
            أرجع ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    𐤀𐤌 (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    أرجع *هذا;
}

نطاق {

فراغ setField(StringPiece input, حرف* dest, UErrorCode& errorCode,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    𐤀𐤌 (input.empty()) {
        dest[0] = '\0';
    } وإلا 𐤀𐤌 (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } وإلا {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    أرجع *هذا;
}

نطاق {

فراغ transform(حرف* data, int32_t len) {
    لكل (int32_t i = 0; i < len; i++, data++) {
        𐤀𐤌 (*data == '_') {
            *data = '-';
        } وإلا {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    𐤀𐤌 (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    𐤀𐤌 (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    𐤀𐤌 (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    delete variant_;
    variant_ = new_variant;
    أرجع *هذا;
}

نطاق {

منطقي
_isKeywordValue(ثابت حرف* key, ثابت حرف* value, int32_t value_len)
{
    𐤀𐤌 (key[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } وإلا 𐤀𐤌 (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        أرجع ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    أرجع unicode_locale_key.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_isUnicodeLocaleKey(unicode_locale_key->data(),
                                    static_cast<int32_t>(unicode_locale_key->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& to, منطقي validate, UErrorCode& errorCode)
{
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    𐤀𐤌 (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        𐤀𐤌 (U_FAILURE(errorCode) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* key;
    طالما ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        𐤀𐤌 (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        𐤀𐤌 (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    }
}

فراغ
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    𐤀𐤌 (U_FAILURE(errorCode) || iter.isNull()) { أرجع; }
    ثابت حرف* key;
    طالما ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

فراغ
_setUnicodeExtensions(Locale& locale, ثابت CharString& value, UErrorCode& errorCode)
{
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, خطأ, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(حرف key, StringPiece value) U_LIFETIME_BOUND
{
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    𐤀𐤌 (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    CharString value_str(value, status_);
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    transform(value_str.data(), value_str.length());
    𐤀𐤌 (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    𐤀𐤌 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        𐤀𐤌 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
    }
    𐤀𐤌 (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        أرجع *هذا;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    𐤀𐤌 (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    𐤀𐤌 (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      أرجع *هذا;
    }
    𐤀𐤌 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        𐤀𐤌 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    transform(value_str.data(), value_str.length());
    𐤀𐤌 (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    𐤀𐤌 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        𐤀𐤌 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        أرجع *هذا;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    𐤀𐤌 (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        أرجع *هذا;
    }

    transform(attributes.data(),attributes.length());
    ثابت حرف* start = attributes.data();
    ثابت حرف* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقي inserted = خطأ;
    طالما (start < limit) {
        𐤀𐤌 (!inserted) {
            صحيح cmp = uprv_strcmp(start, value_str.data());
            𐤀𐤌 (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            𐤀𐤌 (cmp > 0) {
                𐤀𐤌 (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = true;
            }
        }
        𐤀𐤌 (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    𐤀𐤌 (!inserted) {
        𐤀𐤌 (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    𐤀𐤌 (U_FAILURE(status_)) { أرجع *هذا; }
    transform(value_str.data(), value_str.length());
    𐤀𐤌 (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    𐤀𐤌 (extensions_ == nullptr) { أرجع *هذا; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    𐤀𐤌 (U_FAILURE(localErrorCode)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    𐤀𐤌 (attributes.isEmpty()) { أرجع *هذا; }

    حرف* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    لكل (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* start = attributes.data();
    ثابت حرف* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقي found = خطأ;
    طالما (start < limit) {
        𐤀𐤌 (uprv_strcmp(start, value_str.data()) == 0) {
            found = true;
        } وإلا {
            𐤀𐤌 (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    𐤀𐤌 (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    أرجع *هذا;
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
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    أرجع *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  أرجع bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UErrorCode& errorCode)
{
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    𐤀𐤌 (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    𐤀𐤌 (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        𐤀𐤌 (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    𐤀𐤌 (U_FAILURE(status_)) {
        errorCode = status_;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    𐤀𐤌 (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    𐤀𐤌 (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    𐤀𐤌 (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    Locale product(locale_str.data());
    𐤀𐤌 (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    أرجع product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    𐤀𐤌 (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        أرجع true;
    }
    outErrorCode = status_;
    أرجع U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
