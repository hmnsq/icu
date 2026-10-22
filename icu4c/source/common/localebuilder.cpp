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

constexpr ثابت حرف* k_Attribute_مبودي = "attribute";

منطقي _isExtensionSubtags(حرف مبودي, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(مبودي)) {
        حالة 'u':
            أرجع ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            أرجع ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            أرجع ultag_is_Privateuse_درج_Subtags(s, len);
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
    إذا (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    أرجع *هذا;
}

نطاق {

فراغ setField(StringPiece input, حرف* dest, UErrorCode& errorCode,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    إذا (U_FAILURE(errorCode)) { أرجع; }
    إذا (input.empty()) {
        dest[0] = '\0';
    } وإلا إذا (test(input.data(), input.length())) {
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
        إذا (*data == '_') {
            *data = '-';
        } وإلا {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    إذا (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    إذا (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    إذا (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
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
is_Keyword_درج(ثابت حرف* مبودي, ثابت حرف* value, int32_t درج_len)
{
    إذا (مبودي[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(مبودي[0])) &&
                _isExtensionSubtags(مبودي[0], value, درج_len));
    } وإلا إذا (uprv_strcmp(مبودي, k_Attribute_مبودي) == 0) {
        // unicode attributes
        أرجع ultag_isUnicodeLocaleAttributes(value, درج_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_مبودي = ulocimp_to_Bcp_مبودي_With_Fallback(مبودي);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(مبودي, value);

    أرجع unicode_locale_مبودي.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_مبودي(unicode_locale_مبودي->data(),
                                    static_cast<int32_t>(unicode_locale_مبودي->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& to, منطقي validate, UErrorCode& errorCode)
{
    إذا (U_FAILURE(errorCode)) { أرجع; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    إذا (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        إذا (U_FAILURE(errorCode) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* مبودي;
    طالما ((مبودي = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_درج<CharString>(مبودي, errorCode);
        إذا (U_FAILURE(errorCode)) { أرجع; }
        إذا (uprv_strcmp(مبودي, k_Attribute_مبودي) == 0) {
            transform(value.data(), value.length());
        }
        إذا (validate &&
            !is_Keyword_درج(مبودي, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
        to.set_Keyword_درج(مبودي, value.data(), errorCode);
        إذا (U_FAILURE(errorCode)) { أرجع; }
    }
}

فراغ
clear_UAttributes_And_مبودي_Type(Locale& locale, UErrorCode& errorCode)
{
    إذا (U_FAILURE(errorCode)) { أرجع; }
    // Clear Unicode attributes
    locale.set_Keyword_درج(k_Attribute_مبودي, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    إذا (U_FAILURE(errorCode) || iter.isNull()) { أرجع; }
    ثابت حرف* مبودي;
    طالما ((مبودي = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_درج(مبودي, nullptr, errorCode);
    }
}

فراغ
_setUnicodeExtensions(Locale& locale, ثابت CharString& value, UErrorCode& errorCode)
{
    إذا (U_FAILURE(errorCode)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, خطأ, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(حرف مبودي, StringPiece value) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    إذا (!UPRV_ISALPHANUM(مبودي)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    CharString درج_str(value, status_);
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    transform(درج_str.data(), درج_str.length());
    إذا (!درج_str.isEmpty() &&
            !_isExtensionSubtags(مبودي, درج_str.data(), درج_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == nullptr) {
        extensions_ = Locale::get_سيو().clone();
        إذا (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
    }
    إذا (uprv_tolower(مبودي) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_درج(StringPiece(&مبودي, 1), درج_str.data(),
                                     status_);
        أرجع *هذا;
    }
    clear_UAttributes_And_مبودي_Type(*extensions_, status_);
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    إذا (!value.empty()) {
        _setUnicodeExtensions(*extensions_, درج_str, status_);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece مبودي,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    إذا (!ultag_is_Unicode_Locale_مبودي(مبودي.data(), مبودي.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      أرجع *هذا;
    }
    إذا (extensions_ == nullptr) {
        extensions_ = Locale::get_سيو().clone();
        إذا (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->set_Unicode_Keyword_درج(مبودي, type, status_);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString درج_str(value, status_);
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    transform(درج_str.data(), درج_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(درج_str.data(), درج_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == nullptr) {
        extensions_ = Locale::get_سيو().clone();
        إذا (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
        extensions_->set_Keyword_درج(k_Attribute_مبودي, درج_str.data(), status_);
        أرجع *هذا;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_درج<CharString>(k_Attribute_مبودي, localErrorCode);
    إذا (U_FAILURE(localErrorCode)) {
        CharString new_attributes(درج_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_درج(k_Attribute_مبودي, new_attributes.data(), status_);
        أرجع *هذا;
    }

    transform(attributes.data(),attributes.length());
    ثابت حرف* start = attributes.data();
    ثابت حرف* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    منطقي inserted = خطأ;
    طالما (start < limit) {
        إذا (!inserted) {
            صحيح cmp = uprv_strcmp(start, درج_str.data());
            إذا (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            إذا (cmp > 0) {
                إذا (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(درج_str.data(), status_);
                inserted = true;
            }
        }
        إذا (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    إذا (!inserted) {
        إذا (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(درج_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_درج(k_Attribute_مبودي, new_attributes.data(), status_);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString درج_str(value, status_);
    إذا (U_FAILURE(status_)) { أرجع *هذا; }
    transform(درج_str.data(), درج_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(درج_str.data(), درج_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == nullptr) { أرجع *هذا; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_درج<CharString>(k_Attribute_مبودي, localErrorCode);
    // get failure, just return
    إذا (U_FAILURE(localErrorCode)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    إذا (attributes.isEmpty()) { أرجع *هذا; }

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
        إذا (uprv_strcmp(start, درج_str.data()) == 0) {
            found = true;
        } وإلا {
            إذا (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    إذا (found) {
        extensions_->set_Keyword_درج(k_Attribute_مبودي, new_attributes.data(), status_);
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
    إذا (U_FAILURE(errorCode)) { أرجع; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    إذا (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    إذا (extensions_ == nullptr) {
        extensions_ = Locale::get_سيو().clone();
        إذا (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    إذا (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    إذا (U_FAILURE(status_)) {
        errorCode = status_;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    إذا (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    إذا (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    إذا (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    إذا (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    Locale product(locale_str.data());
    إذا (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    إذا (U_FAILURE(errorCode)) {
        أرجع makeBogusLocale();
    }
    أرجع product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    إذا (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        أرجع true;
    }
    outErrorCode = status_;
    أرجع U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
