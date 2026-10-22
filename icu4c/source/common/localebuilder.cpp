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

مضمن منطقي UPRV_ISDIGIT(حرف c) { החזר c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { החזר uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* kAttributeKey = "attribute";

منطقي _isExtensionSubtags(حرف key, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(key)) {
        حالة 'u':
            החזר ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            החזר ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            החזר ultag_isPrivateuseValueSubtags(s, len);
        افتراضي:
            החזר ultag_isExtensionSubtags(s, len);
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
    אם (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    אם (U_FAILURE(status_)) { החזר *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    החזר *هذا;
}

نطاق {

فراغ setField(StringPiece input, حرف* dest, UErrorCode& errorCode,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    אם (U_FAILURE(errorCode)) { החזר; }
    אם (input.empty()) {
        dest[0] = '\0';
    } אחרת אם (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } אחרת {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    החזר *هذا;
}

نطاق {

فراغ transform(حرف* data, int32_t len) {
    עבור (int32_t i = 0; i < len; i++, data++) {
        אם (*data == '_') {
            *data = '-';
        } אחרת {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    אם (U_FAILURE(status_)) { החזר *هذا; }
    אם (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        החזר *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    אם (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        החזר *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    אם (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    delete variant_;
    variant_ = new_variant;
    החזר *هذا;
}

نطاق {

منطقي
_isKeywordValue(ثابت حرف* key, ثابت حرف* value, int32_t value_len)
{
    אם (key[1] == '\0') {
        // one char key
        החזר (UPRV_ISALPHANUM(uprv_tolower(key[0])) &&
                _isExtensionSubtags(key[0], value, value_len));
    } אחרת אם (uprv_strcmp(key, kAttributeKey) == 0) {
        // unicode attributes
        החזר ultag_isUnicodeLocaleAttributes(value, value_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_key = ulocimp_toBcpKeyWithFallback(key);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(key, value);

    החזר unicode_locale_key.has_value() &&
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
    אם (U_FAILURE(errorCode)) { החזר; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    אם (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        אם (U_FAILURE(errorCode) || ownedKeywords.isNull()) { החזר; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* key;
    כלעוד ((key = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.getKeywordValue<CharString>(key, errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
        אם (uprv_strcmp(key, kAttributeKey) == 0) {
            transform(value.data(), value.length());
        }
        אם (validate &&
            !_isKeywordValue(key, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            החזר;
        }
        to.setKeywordValue(key, value.data(), errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
    }
}

فراغ
_clearUAttributesAndKeyType(Locale& locale, UErrorCode& errorCode)
{
    אם (U_FAILURE(errorCode)) { החזר; }
    // Clear Unicode attributes
    locale.setKeywordValue(kAttributeKey, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    אם (U_FAILURE(errorCode) || iter.isNull()) { החזר; }
    ثابت حرف* key;
    כלעוד ((key = iter->next(nullptr, errorCode)) != nullptr) {
        locale.setUnicodeKeywordValue(key, nullptr, errorCode);
    }
}

فراغ
_setUnicodeExtensions(Locale& locale, ثابت CharString& value, UErrorCode& errorCode)
{
    אם (U_FAILURE(errorCode)) { החזר; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, שקר, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(حرف key, StringPiece value) U_LIFETIME_BOUND
{
    אם (U_FAILURE(status_)) { החזר *هذا; }
    אם (!UPRV_ISALPHANUM(key)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    CharString value_str(value, status_);
    אם (U_FAILURE(status_)) { החזר *هذا; }
    transform(value_str.data(), value_str.length());
    אם (!value_str.isEmpty() &&
            !_isExtensionSubtags(key, value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    אם (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        אם (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            החזר *هذا;
        }
    }
    אם (uprv_tolower(key) != 'u') {
        // for t, x and others extension.
        extensions_->setKeywordValue(StringPiece(&key, 1), value_str.data(),
                                     status_);
        החזר *هذا;
    }
    _clearUAttributesAndKeyType(*extensions_, status_);
    אם (U_FAILURE(status_)) { החזר *هذا; }
    אם (!value.empty()) {
        _setUnicodeExtensions(*extensions_, value_str, status_);
    }
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece key,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    אם (U_FAILURE(status_)) { החזר *هذا; }
    אם (!ultag_isUnicodeLocaleKey(key.data(), key.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      החזר *هذا;
    }
    אם (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        אם (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            החזר *هذا;
        }
    }
    extensions_->setUnicodeKeywordValue(key, type, status_);
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    אם (U_FAILURE(status_)) { החזר *هذا; }
    transform(value_str.data(), value_str.length());
    אם (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    אם (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        אם (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            החזר *هذا;
        }
        extensions_->setKeywordValue(kAttributeKey, value_str.data(), status_);
        החזר *هذا;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    אם (U_FAILURE(localErrorCode)) {
        CharString new_attributes(value_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
        החזר *هذا;
    }

    transform(attributes.data(),attributes.length());
    ثابت حرف* start = attributes.data();
    ثابت حرف* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقي inserted = שקר;
    כלעוד (start < limit) {
        אם (!inserted) {
            صحيح cmp = uprv_strcmp(start, value_str.data());
            אם (cmp == 0) { החזר *هذا; }  // Found it in attributes: Just return
            אם (cmp > 0) {
                אם (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(value_str.data(), status_);
                inserted = אמת;
            }
        }
        אם (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    אם (!inserted) {
        אם (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(value_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString value_str(value, status_);
    אם (U_FAILURE(status_)) { החזר *هذا; }
    transform(value_str.data(), value_str.length());
    אם (!ultag_isUnicodeLocaleAttribute(value_str.data(), value_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        החזר *هذا;
    }
    אם (extensions_ == nullptr) { החזר *هذا; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->getKeywordValue<CharString>(kAttributeKey, localErrorCode);
    // get failure, just return
    אם (U_FAILURE(localErrorCode)) { החזר *هذا; }
    // Do not have any attributes, just return.
    אם (attributes.isEmpty()) { החזר *هذا; }

    حرف* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    עבור (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* start = attributes.data();
    ثابت حرف* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقي found = שקר;
    כלעוד (start < limit) {
        אם (uprv_strcmp(start, value_str.data()) == 0) {
            found = אמת;
        } אחרת {
            אם (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    אם (found) {
        extensions_->setKeywordValue(kAttributeKey, new_attributes.data(), status_);
    }
    החזר *هذا;
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
    החזר *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    החזר *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  החזר bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UErrorCode& errorCode)
{
    אם (U_FAILURE(errorCode)) { החזר; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    אם (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        החזר;
    }
    אם (extensions_ == nullptr) {
        extensions_ = Locale::getRoot().clone();
        אם (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            החזר;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, שקר, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    אם (U_FAILURE(errorCode)) {
        החזר makeBogusLocale();
    }
    אם (U_FAILURE(status_)) {
        errorCode = status_;
        החזר makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    אם (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    אם (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    אם (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    אם (U_FAILURE(errorCode)) {
        החזר makeBogusLocale();
    }
    Locale product(locale_str.data());
    אם (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, אמת, errorCode);
    }
    אם (U_FAILURE(errorCode)) {
        החזר makeBogusLocale();
    }
    החזר product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    אם (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        החזר אמת;
    }
    outErrorCode = status_;
    החזר U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
