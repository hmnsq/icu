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

अन्तःस्थ तार्किक UPRV_ISDIGIT(वर्ण c) { फर्काऊ c >= '0' && c <= '9'; }
अन्तःस्थ तार्किक UPRV_ISALPHANUM(वर्ण c) { फर्काऊ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr अचर वर्ण* k_Attribute_साँचो = "attribute";

तार्किक _isExtensionSubtags(वर्ण साँचो, अचर वर्ण* s, int32_t len) {
    विकल्प (uprv_tolower(साँचो)) {
        पक्ष 'u':
            फर्काऊ ultag_isUnicodeExtensionSubtags(s, len);
        पक्ष 't':
            फर्काऊ ultag_isTransformedExtensionSubtags(s, len);
        पक्ष 'x':
            फर्काऊ ultag_is_Privateuse_मान_Subtags(s, len);
        स्वभाव:
            फर्काऊ ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(अचर Locale& locale) U_LIFETIME_BOUND
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
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    फर्काऊ *एतत्;
}

नामाकाश {

शून्य setField(StringPiece input, वर्ण* dest, UErrorCode& errorCode,
              तार्किक (*test)(अचर वर्ण*, int32_t)) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
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
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    फर्काऊ *एतत्;
}

नामाकाश {

शून्य transform(वर्ण* data, int32_t len) {
    लागि (int32_t i = 0; i < len; i++, data++) {
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
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    यदि (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        फर्काऊ *एतत्;
    }
    FixedString* new_variant = नूतन FixedString(variant);
    यदि (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ *एतत्;
    }
    transform(new_variant->getAlias(), variant.length());
    यदि (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    delete variant_;
    variant_ = new_variant;
    फर्काऊ *एतत्;
}

नामाकाश {

तार्किक
is_Keyword_मान(अचर वर्ण* साँचो, अचर वर्ण* value, int32_t मान_len)
{
    यदि (साँचो[1] == '\0') {
        // one char key
        फर्काऊ (UPRV_ISALPHANUM(uprv_tolower(साँचो[0])) &&
                _isExtensionSubtags(साँचो[0], value, मान_len));
    } अन्यथा यदि (uprv_strcmp(साँचो, k_Attribute_साँचो) == 0) {
        // unicode attributes
        फर्काऊ ultag_isUnicodeLocaleAttributes(value, मान_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_साँचो = ulocimp_to_Bcp_साँचो_With_Fallback(साँचो);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(साँचो, value);

    फर्काऊ unicode_locale_साँचो.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_साँचो(unicode_locale_साँचो->data(),
                                    static_cast<int32_t>(unicode_locale_साँचो->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

शून्य
_copyExtensions(अचर Locale& from, icu::StringEnumeration *keywords,
                Locale& to, तार्किक validate, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    यदि (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        यदि (U_FAILURE(errorCode) || ownedKeywords.isNull()) { फर्काऊ; }
        keywords = ownedKeywords.getAlias();
    }
    अचर वर्ण* साँचो;
    जबसम्म ((साँचो = keywords->next(nullptr, errorCode)) != nullptr) {
        स्वतः value = from.get_Keyword_मान<CharString>(साँचो, errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        यदि (uprv_strcmp(साँचो, k_Attribute_साँचो) == 0) {
            transform(value.data(), value.length());
        }
        यदि (validate &&
            !is_Keyword_मान(साँचो, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }
        to.set_Keyword_मान(साँचो, value.data(), errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    }
}

शून्य
clear_UAttributes_And_साँचो_Type(Locale& locale, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    // Clear Unicode attributes
    locale.set_Keyword_मान(k_Attribute_साँचो, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    यदि (U_FAILURE(errorCode) || iter.isNull()) { फर्काऊ; }
    अचर वर्ण* साँचो;
    जबसम्म ((साँचो = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_मान(साँचो, nullptr, errorCode);
    }
}

शून्य
_setUnicodeExtensions(Locale& locale, अचर CharString& value, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, असत्य, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(वर्ण साँचो, StringPiece value) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    यदि (!UPRV_ISALPHANUM(साँचो)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    CharString मान_str(value, status_);
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    transform(मान_str.data(), मान_str.length());
    यदि (!मान_str.isEmpty() &&
            !_isExtensionSubtags(साँचो, मान_str.data(), मान_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_जरा().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ *एतत्;
        }
    }
    यदि (uprv_tolower(साँचो) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_मान(StringPiece(&साँचो, 1), मान_str.data(),
                                     status_);
        फर्काऊ *एतत्;
    }
    clear_UAttributes_And_साँचो_Type(*extensions_, status_);
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    यदि (!value.empty()) {
        _setUnicodeExtensions(*extensions_, मान_str, status_);
    }
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece साँचो,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    यदि (!ultag_is_Unicode_Locale_साँचो(साँचो.data(), साँचो.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      फर्काऊ *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_जरा().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ *एतत्;
        }
    }
    extensions_->set_Unicode_Keyword_मान(साँचो, type, status_);
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मान_str(value, status_);
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    transform(मान_str.data(), मान_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मान_str.data(), मान_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_जरा().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ *एतत्;
        }
        extensions_->set_Keyword_मान(k_Attribute_साँचो, मान_str.data(), status_);
        फर्काऊ *एतत्;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मान<CharString>(k_Attribute_साँचो, localErrorCode);
    यदि (U_FAILURE(localErrorCode)) {
        CharString new_attributes(मान_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_मान(k_Attribute_साँचो, new_attributes.data(), status_);
        फर्काऊ *एतत्;
    }

    transform(attributes.data(),attributes.length());
    अचर वर्ण* start = attributes.data();
    अचर वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक inserted = असत्य;
    जबसम्म (start < limit) {
        यदि (!inserted) {
            पूर्णाङ्क cmp = uprv_strcmp(start, मान_str.data());
            यदि (cmp == 0) { फर्काऊ *एतत्; }  // Found it in attributes: Just return
            यदि (cmp > 0) {
                यदि (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(मान_str.data(), status_);
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
        new_attributes.append(मान_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_मान(k_Attribute_साँचो, new_attributes.data(), status_);
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मान_str(value, status_);
    यदि (U_FAILURE(status_)) { फर्काऊ *एतत्; }
    transform(मान_str.data(), मान_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मान_str.data(), मान_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ *एतत्;
    }
    यदि (extensions_ == nullptr) { फर्काऊ *एतत्; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मान<CharString>(k_Attribute_साँचो, localErrorCode);
    // get failure, just return
    यदि (U_FAILURE(localErrorCode)) { फर्काऊ *एतत्; }
    // Do not have any attributes, just return.
    यदि (attributes.isEmpty()) { फर्काऊ *एतत्; }

    वर्ण* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    लागि (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    अचर वर्ण* start = attributes.data();
    अचर वर्ण* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक found = असत्य;
    जबसम्म (start < limit) {
        यदि (uprv_strcmp(start, मान_str.data()) == 0) {
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
        extensions_->set_Keyword_मान(k_Attribute_साँचो, new_attributes.data(), status_);
    }
    फर्काऊ *एतत्;
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
    फर्काऊ *एतत्;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    फर्काऊ *एतत्;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  फर्काऊ bogus;
}

शून्य LocaleBuilder::copyExtensionsFrom(अचर Locale& src, UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    यदि (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        फर्काऊ;
    }
    यदि (extensions_ == nullptr) {
        extensions_ = Locale::get_जरा().clone();
        यदि (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, असत्य, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ makeBogusLocale();
    }
    यदि (U_FAILURE(status_)) {
        errorCode = status_;
        फर्काऊ makeBogusLocale();
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
        फर्काऊ makeBogusLocale();
    }
    Locale product(locale_str.data());
    यदि (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, सत्य, errorCode);
    }
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ makeBogusLocale();
    }
    फर्काऊ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) अचर {
    यदि (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        फर्काऊ सत्य;
    }
    outErrorCode = status_;
    फर्काऊ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
