// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>
#अन्तर्भाव <utility>

#अन्तर्भाव "bytesinkutil.h"  // StringByteSink<CharString>
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "fixedstring.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "unicode/localebuilder.h"
#अन्तर्भाव "unicode/locid.h"

नामाकाश {

अन्तःस्थ तार्किक UPRV_ISDIGIT(वर्ण c) { प्रत्यागम c >= '0' && c <= '9'; }
अन्तःस्थ तार्किक UPRV_ISALPHANUM(वर्ण c) { प्रत्यागम uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ध्रुव वर्ण* k_Attribute_किल्ली = "attribute";

तार्किक _isExtensionSubtags(वर्ण किल्ली, ध्रुव वर्ण* s, int32_t len) {
    विकल्प (uprv_tolower(किल्ली)) {
        पक्ष 'u':
            प्रत्यागम ultag_isUnicodeExtensionSubtags(s, len);
        पक्ष 't':
            प्रत्यागम ultag_isTransformedExtensionSubtags(s, len);
        पक्ष 'x':
            प्रत्यागम ultag_is_Privateuse_मूल्य_Subtags(s, len);
        स्वभाव:
            प्रत्यागम ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(ध्रुव Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    यदि (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    प्रत्यागम *एतत्;
}

नामाकाश {

शून्य setField(StringPiece input, वर्ण* dest, UErrorCode& errorCode,
              तार्किक (*test)(ध्रुव वर्ण*, int32_t)) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    यदि (input.empty()) {
        dest[0] = '\0';
    } अन्यथा यदि (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } अन्यथा {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    प्रत्यागम *एतत्;
}

नामाकाश {

शून्य transform(वर्ण* data, int32_t len) {
    प्रति (int32_t i = 0; i < len; i++, data++) {
        यदि (*data == '_') {
            *data = '-';
        } अन्यथा {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    यदि (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        प्रत्यागम *एतत्;
    }
    FixedString* new_variant = नूतन FixedString(variant);
    यदि (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम *एतत्;
    }
    transform(new_variant->getAlias(), variant.length());
    यदि (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    delete variant_;
    variant_ = new_variant;
    प्रत्यागम *एतत्;
}

नामाकाश {

तार्किक
is_Keyword_मूल्य(ध्रुव वर्ण* किल्ली, ध्रुव वर्ण* value, int32_t मूल्य_len)
{
    यदि (किल्ली[1] == '\0') {
        // one char key
        प्रत्यागम (UPRV_ISALPHANUM(uprv_tolower(किल्ली[0])) &&
                _isExtensionSubtags(किल्ली[0], value, मूल्य_len));
    } अन्यथा यदि (uprv_strcmp(किल्ली, k_Attribute_किल्ली) == 0) {
        // unicode attributes
        प्रत्यागम ultag_isUnicodeLocaleAttributes(value, मूल्य_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_किल्ली = ulocimp_to_Bcp_किल्ली_With_Fallback(किल्ली);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(किल्ली, value);

    प्रत्यागम unicode_locale_किल्ली.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_किल्ली(unicode_locale_किल्ली->data(),
                                    static_cast<int32_t>(unicode_locale_किल्ली->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

शून्य
_copyExtensions(ध्रुव Locale& from, icu::StringEnumeration *keywords,
                Locale& to, तार्किक validate, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    यदि (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        यदि (U_FAILURE(errorCode) || ownedKeywords.isNull()) { प्रत्यागम; }
        keywords = ownedKeywords.getAlias();
    }
    ध्रुव वर्ण* किल्ली;
    यावत् ((किल्ली = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_मूल्य<CharString>(किल्ली, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        यदि (uprv_strcmp(किल्ली, k_Attribute_किल्ली) == 0) {
            transform(value.data(), value.length());
        }
        यदि (validate &&
            !is_Keyword_मूल्य(किल्ली, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }
        to.set_Keyword_मूल्य(किल्ली, value.data(), errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    }
}

शून्य
clear_UAttributes_And_किल्ली_Type(Locale& locale, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    // Clear Unicode attributes
    locale.set_Keyword_मूल्य(k_Attribute_किल्ली, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    यदि (U_FAILURE(errorCode) || iter.isNull()) { प्रत्यागम; }
    ध्रुव वर्ण* किल्ली;
    यावत् ((किल्ली = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_मूल्य(किल्ली, nullptr, errorCode);
    }
}

शून्य
_setUnicodeExtensions(Locale& locale, ध्रुव CharString& value, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, असत्य, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(वर्ण किल्ली, StringPiece value) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    यदि (!UPRV_ISALPHANUM(किल्ली)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    CharString मूल्य_str(value, status_);
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    यदि (!मूल्य_str.isEmpty() &&
            !_isExtensionSubtags(किल्ली, मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    यदि (uprv_tolower(किल्ली) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_मूल्य(StringPiece(&किल्ली, 1), मूल्य_str.data(),
                                     status_);
        प्रत्यागम *एतत्;
    }
    clear_UAttributes_And_किल्ली_Type(*extensions_, status_);
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    यदि (!value.empty()) {
        _setUnicodeExtensions(*extensions_, मूल्य_str, status_);
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece किल्ली,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    यदि (!ultag_is_Unicode_Locale_किल्ली(किल्ली.data(), किल्ली.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      प्रत्यागम *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    extensions_->set_Unicode_Keyword_मूल्य(किल्ली, type, status_);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, मूल्य_str.data(), status_);
        प्रत्यागम *एतत्;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_किल्ली, localErrorCode);
    यदि (U_FAILURE(localErrorCode)) {
        CharString new_attributes(मूल्य_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
        प्रत्यागम *एतत्;
    }

    transform(attributes.data(),attributes.length());
    ध्रुव वर्ण* start = attributes.data();
    ध्रुव वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक inserted = असत्य;
    यावत् (start < limit) {
        यदि (!inserted) {
            पूर्णाङ्क cmp = uprv_strcmp(start, मूल्य_str.data());
            यदि (cmp == 0) { प्रत्यागम *एतत्; }  // Found it in attributes: Just return
            यदि (cmp > 0) {
                यदि (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(मूल्य_str.data(), status_);
                inserted = सत्य;
            }
        }
        यदि (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    यदि (!inserted) {
        यदि (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(मूल्य_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    यदि (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == nullptr) { प्रत्यागम *एतत्; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_किल्ली, localErrorCode);
    // get failure, just return
    यदि (U_FAILURE(localErrorCode)) { प्रत्यागम *एतत्; }
    // Do not have any attributes, just return.
    यदि (attributes.isEmpty()) { प्रत्यागम *एतत्; }

    वर्ण* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    प्रति (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ध्रुव वर्ण* start = attributes.data();
    ध्रुव वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक found = असत्य;
    यावत् (start < limit) {
        यदि (uprv_strcmp(start, मूल्य_str.data()) == 0) {
            found = सत्य;
        } अन्यथा {
            यदि (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    यदि (found) {
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
    }
    प्रत्यागम *एतत्;
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
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    प्रत्यागम *एतत्;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  प्रत्यागम bogus;
}

शून्य LocaleBuilder::copyExtensionsFrom(ध्रुव Locale& src, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    यदि (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        प्रत्यागम;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, असत्य, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    यदि (U_FAILURE(status_)) {
        errorCode = status_;
        प्रत्यागम makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    यदि (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    यदि (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    यदि (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    Locale product(locale_str.data());
    यदि (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, सत्य, errorCode);
    }
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    प्रत्यागम product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    यदि (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        प्रत्यागम सत्य;
    }
    outErrorCode = status_;
    प्रत्यागम U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
