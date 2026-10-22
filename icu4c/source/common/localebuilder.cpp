// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "bytesinkutil.h"  // StringByteSink<CharString>
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "fixedstring.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "unicode/localebuilder.h"
#અન્તર્ભાવ "unicode/locid.h"

નામાકાશ {

અંતઃરેખ તાર્કિક UPRV_ISDIGIT(અક્ષર c) { પાછા c >= '0' && c <= '9'; }
અંતઃરેખ તાર્કિક UPRV_ISALPHANUM(અક્ષર c) { પાછા uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr અચલ અક્ષર* k_Attribute_ચાવી = "attribute";

તાર્કિક _isExtensionSubtags(અક્ષર ચાવી, અચલ અક્ષર* s, int32_t len) {
    પસંદગી (uprv_tolower(ચાવી)) {
        સ્થિતિ 'u':
            પાછા ultag_isUnicodeExtensionSubtags(s, len);
        સ્થિતિ 't':
            પાછા ultag_isTransformedExtensionSubtags(s, len);
        સ્થિતિ 'x':
            પાછા ultag_is_Privateuse_કિંમત_Subtags(s, len);
        મૂળભૂત:
            પાછા ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(અચલ Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    જો (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    જો (U_FAILURE(status_)) { પાછા *આ; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    પાછા *આ;
}

નામાકાશ {

શૂન્ય setField(StringPiece input, અક્ષર* dest, UErrorCode& errorCode,
              તાર્કિક (*test)(અચલ અક્ષર*, int32_t)) {
    જો (U_FAILURE(errorCode)) { પાછા; }
    જો (input.empty()) {
        dest[0] = '\0';
    } નહીંતર જો (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } નહીંતર {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    પાછા *આ;
}

નામાકાશ {

શૂન્ય transform(અક્ષર* data, int32_t len) {
    માટે (int32_t i = 0; i < len; i++, data++) {
        જો (*data == '_') {
            *data = '-';
        } નહીંતર {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    જો (U_FAILURE(status_)) { પાછા *આ; }
    જો (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        પાછા *આ;
    }
    FixedString* new_variant = નવું FixedString(variant);
    જો (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        પાછા *આ;
    }
    transform(new_variant->getAlias(), variant.length());
    જો (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    delete variant_;
    variant_ = new_variant;
    પાછા *આ;
}

નામાકાશ {

તાર્કિક
is_Keyword_કિંમત(અચલ અક્ષર* ચાવી, અચલ અક્ષર* value, int32_t કિંમત_len)
{
    જો (ચાવી[1] == '\0') {
        // one char key
        પાછા (UPRV_ISALPHANUM(uprv_tolower(ચાવી[0])) &&
                _isExtensionSubtags(ચાવી[0], value, કિંમત_len));
    } નહીંતર જો (uprv_strcmp(ચાવી, k_Attribute_ચાવી) == 0) {
        // unicode attributes
        પાછા ultag_isUnicodeLocaleAttributes(value, કિંમત_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ચાવી = ulocimp_to_Bcp_ચાવી_With_Fallback(ચાવી);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(ચાવી, value);

    પાછા unicode_locale_ચાવી.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ચાવી(unicode_locale_ચાવી->data(),
                                    static_cast<int32_t>(unicode_locale_ચાવી->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

શૂન્ય
_copyExtensions(અચલ Locale& from, icu::StringEnumeration *keywords,
                Locale& to, તાર્કિક validate, UErrorCode& errorCode)
{
    જો (U_FAILURE(errorCode)) { પાછા; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    જો (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        જો (U_FAILURE(errorCode) || ownedKeywords.isNull()) { પાછા; }
        keywords = ownedKeywords.getAlias();
    }
    અચલ અક્ષર* ચાવી;
    જ્યારે ((ચાવી = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_કિંમત<CharString>(ચાવી, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        જો (uprv_strcmp(ચાવી, k_Attribute_ચાવી) == 0) {
            transform(value.data(), value.length());
        }
        જો (validate &&
            !is_Keyword_કિંમત(ચાવી, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
        to.set_Keyword_કિંમત(ચાવી, value.data(), errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
    }
}

શૂન્ય
clear_UAttributes_And_ચાવી_Type(Locale& locale, UErrorCode& errorCode)
{
    જો (U_FAILURE(errorCode)) { પાછા; }
    // Clear Unicode attributes
    locale.set_Keyword_કિંમત(k_Attribute_ચાવી, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    જો (U_FAILURE(errorCode) || iter.isNull()) { પાછા; }
    અચલ અક્ષર* ચાવી;
    જ્યારે ((ચાવી = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_કિંમત(ચાવી, nullptr, errorCode);
    }
}

શૂન્ય
_setUnicodeExtensions(Locale& locale, અચલ CharString& value, UErrorCode& errorCode)
{
    જો (U_FAILURE(errorCode)) { પાછા; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, ખોટું, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(અક્ષર ચાવી, StringPiece value) U_LIFETIME_BOUND
{
    જો (U_FAILURE(status_)) { પાછા *આ; }
    જો (!UPRV_ISALPHANUM(ચાવી)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    CharString કિંમત_str(value, status_);
    જો (U_FAILURE(status_)) { પાછા *આ; }
    transform(કિંમત_str.data(), કિંમત_str.length());
    જો (!કિંમત_str.isEmpty() &&
            !_isExtensionSubtags(ચાવી, કિંમત_str.data(), કિંમત_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    જો (extensions_ == nullptr) {
        extensions_ = Locale::get_મૂળ().clone();
        જો (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ;
        }
    }
    જો (uprv_tolower(ચાવી) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_કિંમત(StringPiece(&ચાવી, 1), કિંમત_str.data(),
                                     status_);
        પાછા *આ;
    }
    clear_UAttributes_And_ચાવી_Type(*extensions_, status_);
    જો (U_FAILURE(status_)) { પાછા *આ; }
    જો (!value.empty()) {
        _setUnicodeExtensions(*extensions_, કિંમત_str, status_);
    }
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece ચાવી,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    જો (U_FAILURE(status_)) { પાછા *આ; }
    જો (!ultag_is_Unicode_Locale_ચાવી(ચાવી.data(), ચાવી.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      પાછા *આ;
    }
    જો (extensions_ == nullptr) {
        extensions_ = Locale::get_મૂળ().clone();
        જો (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ;
        }
    }
    extensions_->set_Unicode_Keyword_કિંમત(ચાવી, type, status_);
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString કિંમત_str(value, status_);
    જો (U_FAILURE(status_)) { પાછા *આ; }
    transform(કિંમત_str.data(), કિંમત_str.length());
    જો (!ultag_isUnicodeLocaleAttribute(કિંમત_str.data(), કિંમત_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    જો (extensions_ == nullptr) {
        extensions_ = Locale::get_મૂળ().clone();
        જો (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ;
        }
        extensions_->set_Keyword_કિંમત(k_Attribute_ચાવી, કિંમત_str.data(), status_);
        પાછા *આ;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_કિંમત<CharString>(k_Attribute_ચાવી, localErrorCode);
    જો (U_FAILURE(localErrorCode)) {
        CharString new_attributes(કિંમત_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_કિંમત(k_Attribute_ચાવી, new_attributes.data(), status_);
        પાછા *આ;
    }

    transform(attributes.data(),attributes.length());
    અચલ અક્ષર* start = attributes.data();
    અચલ અક્ષર* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    તાર્કિક inserted = ખોટું;
    જ્યારે (start < limit) {
        જો (!inserted) {
            પૂર્ણાંક cmp = uprv_strcmp(start, કિંમત_str.data());
            જો (cmp == 0) { પાછા *આ; }  // Found it in attributes: Just return
            જો (cmp > 0) {
                જો (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(કિંમત_str.data(), status_);
                inserted = સાચું;
            }
        }
        જો (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    જો (!inserted) {
        જો (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(કિંમત_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_કિંમત(k_Attribute_ચાવી, new_attributes.data(), status_);
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString કિંમત_str(value, status_);
    જો (U_FAILURE(status_)) { પાછા *આ; }
    transform(કિંમત_str.data(), કિંમત_str.length());
    જો (!ultag_isUnicodeLocaleAttribute(કિંમત_str.data(), કિંમત_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા *આ;
    }
    જો (extensions_ == nullptr) { પાછા *આ; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_કિંમત<CharString>(k_Attribute_ચાવી, localErrorCode);
    // get failure, just return
    જો (U_FAILURE(localErrorCode)) { પાછા *આ; }
    // Do not have any attributes, just return.
    જો (attributes.isEmpty()) { પાછા *આ; }

    અક્ષર* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    માટે (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    અચલ અક્ષર* start = attributes.data();
    અચલ અક્ષર* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    તાર્કિક found = ખોટું;
    જ્યારે (start < limit) {
        જો (uprv_strcmp(start, કિંમત_str.data()) == 0) {
            found = સાચું;
        } નહીંતર {
            જો (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    જો (found) {
        extensions_->set_Keyword_કિંમત(k_Attribute_ચાવી, new_attributes.data(), status_);
    }
    પાછા *આ;
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
    પાછા *આ;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    પાછા *આ;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  પાછા bogus;
}

શૂન્ય LocaleBuilder::copyExtensionsFrom(અચલ Locale& src, UErrorCode& errorCode)
{
    જો (U_FAILURE(errorCode)) { પાછા; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    જો (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        પાછા;
    }
    જો (extensions_ == nullptr) {
        extensions_ = Locale::get_મૂળ().clone();
        જો (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, ખોટું, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    જો (U_FAILURE(errorCode)) {
        પાછા makeBogusLocale();
    }
    જો (U_FAILURE(status_)) {
        errorCode = status_;
        પાછા makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    જો (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    જો (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    જો (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    જો (U_FAILURE(errorCode)) {
        પાછા makeBogusLocale();
    }
    Locale product(locale_str.data());
    જો (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, સાચું, errorCode);
    }
    જો (U_FAILURE(errorCode)) {
        પાછા makeBogusLocale();
    }
    પાછા product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) અચલ {
    જો (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        પાછા સાચું;
    }
    outErrorCode = status_;
    પાછા U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
