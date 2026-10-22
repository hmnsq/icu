// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ <optional>
#அந்தர்பாவ <string_view>
#அந்தர்பாவ <utility>

#அந்தர்பாவ "bytesinkutil.h"  // StringByteSink<CharString>
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "fixedstring.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "unicode/localebuilder.h"
#அந்தர்பாவ "unicode/locid.h"

நாமாகாஶ {

உள்வரி தருக்க UPRV_ISDIGIT(எழுத்து c) { திரும்பு c >= '0' && c <= '9'; }
உள்வரி தருக்க UPRV_ISALPHANUM(எழுத்து c) { திரும்பு uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr மாறிலி எழுத்து* k_Attribute_சாவி = "attribute";

தருக்க _isExtensionSubtags(எழுத்து சாவி, மாறிலி எழுத்து* s, int32_t len) {
    தேர்வு (uprv_tolower(சாவி)) {
        நிலை 'u':
            திரும்பு ultag_isUnicodeExtensionSubtags(s, len);
        நிலை 't':
            திரும்பு ultag_isTransformedExtensionSubtags(s, len);
        நிலை 'x':
            திரும்பு ultag_is_Privateuse_மதிப்பு_Subtags(s, len);
        இயல்பு:
            திரும்பு ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(மாறிலி Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    எனில் (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    திரும்பு *இது;
}

நாமாகாஶ {

வெறுமை setField(StringPiece input, எழுத்து* dest, UErrorCode& errorCode,
              தருக்க (*test)(மாறிலி எழுத்து*, int32_t)) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    எனில் (input.empty()) {
        dest[0] = '\0';
    } இல்லையெனில் எனில் (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } இல்லையெனில் {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    திரும்பு *இது;
}

நாமாகாஶ {

வெறுமை transform(எழுத்து* data, int32_t len) {
    க்கு (int32_t i = 0; i < len; i++, data++) {
        எனில் (*data == '_') {
            *data = '-';
        } இல்லையெனில் {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    எனில் (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        திரும்பு *இது;
    }
    FixedString* new_variant = புதிய FixedString(variant);
    எனில் (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு *இது;
    }
    transform(new_variant->getAlias(), variant.length());
    எனில் (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    delete variant_;
    variant_ = new_variant;
    திரும்பு *இது;
}

நாமாகாஶ {

தருக்க
is_Keyword_மதிப்பு(மாறிலி எழுத்து* சாவி, மாறிலி எழுத்து* value, int32_t மதிப்பு_len)
{
    எனில் (சாவி[1] == '\0') {
        // one char key
        திரும்பு (UPRV_ISALPHANUM(uprv_tolower(சாவி[0])) &&
                _isExtensionSubtags(சாவி[0], value, மதிப்பு_len));
    } இல்லையெனில் எனில் (uprv_strcmp(சாவி, k_Attribute_சாவி) == 0) {
        // unicode attributes
        திரும்பு ultag_isUnicodeLocaleAttributes(value, மதிப்பு_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_சாவி = ulocimp_to_Bcp_சாவி_With_Fallback(சாவி);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(சாவி, value);

    திரும்பு unicode_locale_சாவி.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_சாவி(unicode_locale_சாவி->data(),
                                    static_cast<int32_t>(unicode_locale_சாவி->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

வெறுமை
_copyExtensions(மாறிலி Locale& from, icu::StringEnumeration *keywords,
                Locale& to, தருக்க validate, UErrorCode& errorCode)
{
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    எனில் (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        எனில் (U_FAILURE(errorCode) || ownedKeywords.isNull()) { திரும்பு; }
        keywords = ownedKeywords.getAlias();
    }
    மாறிலி எழுத்து* சாவி;
    போது ((சாவி = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_மதிப்பு<CharString>(சாவி, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        எனில் (uprv_strcmp(சாவி, k_Attribute_சாவி) == 0) {
            transform(value.data(), value.length());
        }
        எனில் (validate &&
            !is_Keyword_மதிப்பு(சாவி, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
        to.set_Keyword_மதிப்பு(சாவி, value.data(), errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    }
}

வெறுமை
clear_UAttributes_And_சாவி_Type(Locale& locale, UErrorCode& errorCode)
{
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    // Clear Unicode attributes
    locale.set_Keyword_மதிப்பு(k_Attribute_சாவி, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    எனில் (U_FAILURE(errorCode) || iter.isNull()) { திரும்பு; }
    மாறிலி எழுத்து* சாவி;
    போது ((சாவி = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_மதிப்பு(சாவி, nullptr, errorCode);
    }
}

வெறுமை
_setUnicodeExtensions(Locale& locale, மாறிலி CharString& value, UErrorCode& errorCode)
{
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, பொய், errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(எழுத்து சாவி, StringPiece value) U_LIFETIME_BOUND
{
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    எனில் (!UPRV_ISALPHANUM(சாவி)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    CharString மதிப்பு_str(value, status_);
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    transform(மதிப்பு_str.data(), மதிப்பு_str.length());
    எனில் (!மதிப்பு_str.isEmpty() &&
            !_isExtensionSubtags(சாவி, மதிப்பு_str.data(), மதிப்பு_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    எனில் (extensions_ == nullptr) {
        extensions_ = Locale::get_வேர்().clone();
        எனில் (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
    }
    எனில் (uprv_tolower(சாவி) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_மதிப்பு(StringPiece(&சாவி, 1), மதிப்பு_str.data(),
                                     status_);
        திரும்பு *இது;
    }
    clear_UAttributes_And_சாவி_Type(*extensions_, status_);
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    எனில் (!value.empty()) {
        _setUnicodeExtensions(*extensions_, மதிப்பு_str, status_);
    }
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece சாவி,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    எனில் (!ultag_is_Unicode_Locale_சாவி(சாவி.data(), சாவி.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      திரும்பு *இது;
    }
    எனில் (extensions_ == nullptr) {
        extensions_ = Locale::get_வேர்().clone();
        எனில் (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
    }
    extensions_->set_Unicode_Keyword_மதிப்பு(சாவி, type, status_);
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString மதிப்பு_str(value, status_);
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    transform(மதிப்பு_str.data(), மதிப்பு_str.length());
    எனில் (!ultag_isUnicodeLocaleAttribute(மதிப்பு_str.data(), மதிப்பு_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    எனில் (extensions_ == nullptr) {
        extensions_ = Locale::get_வேர்().clone();
        எனில் (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
        extensions_->set_Keyword_மதிப்பு(k_Attribute_சாவி, மதிப்பு_str.data(), status_);
        திரும்பு *இது;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_மதிப்பு<CharString>(k_Attribute_சாவி, localErrorCode);
    எனில் (U_FAILURE(localErrorCode)) {
        CharString new_attributes(மதிப்பு_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_மதிப்பு(k_Attribute_சாவி, new_attributes.data(), status_);
        திரும்பு *இது;
    }

    transform(attributes.data(),attributes.length());
    மாறிலி எழுத்து* start = attributes.data();
    மாறிலி எழுத்து* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    தருக்க inserted = பொய்;
    போது (start < limit) {
        எனில் (!inserted) {
            முழு cmp = uprv_strcmp(start, மதிப்பு_str.data());
            எனில் (cmp == 0) { திரும்பு *இது; }  // Found it in attributes: Just return
            எனில் (cmp > 0) {
                எனில் (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(மதிப்பு_str.data(), status_);
                inserted = உண்மை;
            }
        }
        எனில் (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    எனில் (!inserted) {
        எனில் (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(மதிப்பு_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_மதிப்பு(k_Attribute_சாவி, new_attributes.data(), status_);
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString மதிப்பு_str(value, status_);
    எனில் (U_FAILURE(status_)) { திரும்பு *இது; }
    transform(மதிப்பு_str.data(), மதிப்பு_str.length());
    எனில் (!ultag_isUnicodeLocaleAttribute(மதிப்பு_str.data(), மதிப்பு_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு *இது;
    }
    எனில் (extensions_ == nullptr) { திரும்பு *இது; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_மதிப்பு<CharString>(k_Attribute_சாவி, localErrorCode);
    // get failure, just return
    எனில் (U_FAILURE(localErrorCode)) { திரும்பு *இது; }
    // Do not have any attributes, just return.
    எனில் (attributes.isEmpty()) { திரும்பு *இது; }

    எழுத்து* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    க்கு (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    மாறிலி எழுத்து* start = attributes.data();
    மாறிலி எழுத்து* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    தருக்க found = பொய்;
    போது (start < limit) {
        எனில் (uprv_strcmp(start, மதிப்பு_str.data()) == 0) {
            found = உண்மை;
        } இல்லையெனில் {
            எனில் (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    எனில் (found) {
        extensions_->set_Keyword_மதிப்பு(k_Attribute_சாவி, new_attributes.data(), status_);
    }
    திரும்பு *இது;
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
    திரும்பு *இது;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    திரும்பு *இது;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  திரும்பு bogus;
}

வெறுமை LocaleBuilder::copyExtensionsFrom(மாறிலி Locale& src, UErrorCode& errorCode)
{
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    எனில் (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        திரும்பு;
    }
    எனில் (extensions_ == nullptr) {
        extensions_ = Locale::get_வேர்().clone();
        எனில் (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, பொய், errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு makeBogusLocale();
    }
    எனில் (U_FAILURE(status_)) {
        errorCode = status_;
        திரும்பு makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    எனில் (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    எனில் (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    எனில் (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு makeBogusLocale();
    }
    Locale product(locale_str.data());
    எனில் (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, உண்மை, errorCode);
    }
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு makeBogusLocale();
    }
    திரும்பு product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) மாறிலி {
    எனில் (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        திரும்பு உண்மை;
    }
    outErrorCode = status_;
    திரும்பு U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
