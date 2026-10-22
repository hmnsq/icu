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

constexpr ध्रुव वर्ण* k_Attribute_कुञ्चिका = "attribute";

तार्किक _isExtensionSubtags(वर्ण कुञ्चिका, ध्रुव वर्ण* s, int32_t len) {
    विकल्प (uprv_tolower(कुञ्चिका)) {
        पक्ष 'u':
            प्रत्यागम ultag_isUnicodeExtensionSubtags(s, len);
        पक्ष 't':
            प्रत्यागम ultag_isTransformedExtensionSubtags(s, len);
        पक्ष 'x':
            प्रत्यागम ultag_is_Privateuse_मानम्_Subtags(s, len);
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
    जइ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    प्रत्यागम *एतत्;
}

नामाकाश {

शून्य setField(StringPiece input, वर्ण* dest, UErrorCode& errorCode,
              तार्किक (*test)(ध्रुव वर्ण*, int32_t)) {
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    जइ (input.empty()) {
        dest[0] = '\0';
    } अण्णहा जइ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } अण्णहा {
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
        जइ (*data == '_') {
            *data = '-';
        } अण्णहा {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    जइ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        प्रत्यागम *एतत्;
    }
    FixedString* new_variant = नूतन FixedString(variant);
    जइ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम *एतत्;
    }
    transform(new_variant->getAlias(), variant.length());
    जइ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
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
is_Keyword_मानम्(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value, int32_t मानम्_len)
{
    जइ (कुञ्चिका[1] == '\0') {
        // one char key
        प्रत्यागम (UPRV_ISALPHANUM(uprv_tolower(कुञ्चिका[0])) &&
                _isExtensionSubtags(कुञ्चिका[0], value, मानम्_len));
    } अण्णहा जइ (uprv_strcmp(कुञ्चिका, k_Attribute_कुञ्चिका) == 0) {
        // unicode attributes
        प्रत्यागम ultag_isUnicodeLocaleAttributes(value, मानम्_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_कुञ्चिका = ulocimp_to_Bcp_कुञ्चिका_With_Fallback(कुञ्चिका);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(कुञ्चिका, value);

    प्रत्यागम unicode_locale_कुञ्चिका.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_कुञ्चिका(unicode_locale_कुञ्चिका->data(),
                                    static_cast<int32_t>(unicode_locale_कुञ्चिका->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

शून्य
_copyExtensions(ध्रुव Locale& from, icu::StringEnumeration *keywords,
                Locale& to, तार्किक validate, UErrorCode& errorCode)
{
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    जइ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        जइ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { प्रत्यागम; }
        keywords = ownedKeywords.getAlias();
    }
    ध्रुव वर्ण* कुञ्चिका;
    जाव ((कुञ्चिका = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_मानम्<CharString>(कुञ्चिका, errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
        जइ (uprv_strcmp(कुञ्चिका, k_Attribute_कुञ्चिका) == 0) {
            transform(value.data(), value.length());
        }
        जइ (validate &&
            !is_Keyword_मानम्(कुञ्चिका, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }
        to.set_Keyword_मानम्(कुञ्चिका, value.data(), errorCode);
        जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    }
}

शून्य
clear_UAttributes_And_कुञ्चिका_Type(Locale& locale, UErrorCode& errorCode)
{
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    // Clear Unicode attributes
    locale.set_Keyword_मानम्(k_Attribute_कुञ्चिका, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    जइ (U_FAILURE(errorCode) || iter.isNull()) { प्रत्यागम; }
    ध्रुव वर्ण* कुञ्चिका;
    जाव ((कुञ्चिका = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_मानम्(कुञ्चिका, nullptr, errorCode);
    }
}

शून्य
_setUnicodeExtensions(Locale& locale, ध्रुव CharString& value, UErrorCode& errorCode)
{
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, मिच्छ, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(वर्ण कुञ्चिका, StringPiece value) U_LIFETIME_BOUND
{
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    जइ (!UPRV_ISALPHANUM(कुञ्चिका)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    CharString मानम्_str(value, status_);
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.data(), मानम्_str.length());
    जइ (!मानम्_str.isEmpty() &&
            !_isExtensionSubtags(कुञ्चिका, मानम्_str.data(), मानम्_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    जइ (extensions_ == nullptr) {
        extensions_ = Locale::get_मूलम्().clone();
        जइ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    जइ (uprv_tolower(कुञ्चिका) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_मानम्(StringPiece(&कुञ्चिका, 1), मानम्_str.data(),
                                     status_);
        प्रत्यागम *एतत्;
    }
    clear_UAttributes_And_कुञ्चिका_Type(*extensions_, status_);
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    जइ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, मानम्_str, status_);
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece कुञ्चिका,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    जइ (!ultag_is_Unicode_Locale_कुञ्चिका(कुञ्चिका.data(), कुञ्चिका.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      प्रत्यागम *एतत्;
    }
    जइ (extensions_ == nullptr) {
        extensions_ = Locale::get_मूलम्().clone();
        जइ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    extensions_->set_Unicode_Keyword_मानम्(कुञ्चिका, type, status_);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मानम्_str(value, status_);
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.data(), मानम्_str.length());
    जइ (!ultag_isUnicodeLocaleAttribute(मानम्_str.data(), मानम्_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    जइ (extensions_ == nullptr) {
        extensions_ = Locale::get_मूलम्().clone();
        जइ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
        extensions_->set_Keyword_मानम्(k_Attribute_कुञ्चिका, मानम्_str.data(), status_);
        प्रत्यागम *एतत्;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_मानम्<CharString>(k_Attribute_कुञ्चिका, localErrorCode);
    जइ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(मानम्_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_मानम्(k_Attribute_कुञ्चिका, new_attributes.data(), status_);
        प्रत्यागम *एतत्;
    }

    transform(attributes.data(),attributes.length());
    ध्रुव वर्ण* start = attributes.data();
    ध्रुव वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक inserted = मिच्छ;
    जाव (start < limit) {
        जइ (!inserted) {
            पूर्णाङ्क cmp = uprv_strcmp(start, मानम्_str.data());
            जइ (cmp == 0) { प्रत्यागम *एतत्; }  // Found it in attributes: Just return
            जइ (cmp > 0) {
                जइ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(मानम्_str.data(), status_);
                inserted = सच्च;
            }
        }
        जइ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    जइ (!inserted) {
        जइ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(मानम्_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_मानम्(k_Attribute_कुञ्चिका, new_attributes.data(), status_);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मानम्_str(value, status_);
    जइ (U_FAILURE(status_)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.data(), मानम्_str.length());
    जइ (!ultag_isUnicodeLocaleAttribute(मानम्_str.data(), मानम्_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम *एतत्;
    }
    जइ (extensions_ == nullptr) { प्रत्यागम *एतत्; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_मानम्<CharString>(k_Attribute_कुञ्चिका, localErrorCode);
    // get failure, just return
    जइ (U_FAILURE(localErrorCode)) { प्रत्यागम *एतत्; }
    // Do not have any attributes, just return.
    जइ (attributes.isEmpty()) { प्रत्यागम *एतत्; }

    वर्ण* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    प्रति (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ध्रुव वर्ण* start = attributes.data();
    ध्रुव वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक found = मिच्छ;
    जाव (start < limit) {
        जइ (uprv_strcmp(start, मानम्_str.data()) == 0) {
            found = सच्च;
        } अण्णहा {
            जइ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    जइ (found) {
        extensions_->set_Keyword_मानम्(k_Attribute_कुञ्चिका, new_attributes.data(), status_);
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
    जइ (U_FAILURE(errorCode)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    जइ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        प्रत्यागम;
    }
    जइ (extensions_ == nullptr) {
        extensions_ = Locale::get_मूलम्().clone();
        जइ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, मिच्छ, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    जइ (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    जइ (U_FAILURE(status_)) {
        errorCode = status_;
        प्रत्यागम makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    जइ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    जइ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    जइ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    जइ (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    Locale product(locale_str.data());
    जइ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, सच्च, errorCode);
    }
    जइ (U_FAILURE(errorCode)) {
        प्रत्यागम makeBogusLocale();
    }
    प्रत्यागम product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    जइ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        प्रत्यागम सच्च;
    }
    outErrorCode = status_;
    प्रत्यागम U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
