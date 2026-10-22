// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"  // StringByteSink<CharString>
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "fixedstring.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"

ନାମାକାଶ {

ଧାଡ଼ିଭିତରେ ତାର୍କିକ UPRV_ISDIGIT(ଅକ୍ଷର c) { ଫେରନ୍ତୁ c >= '0' && c <= '9'; }
ଧାଡ଼ିଭିତରେ ତାର୍କିକ UPRV_ISALPHANUM(ଅକ୍ଷର c) { ଫେରନ୍ତୁ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ଅଚଳ ଅକ୍ଷର* k_Attribute_ଚାବି = "attribute";

ତାର୍କିକ _isExtensionSubtags(ଅକ୍ଷର ଚାବି, ଅଚଳ ଅକ୍ଷର* s, int32_t len) {
    ବାଛ (uprv_tolower(ଚାବି)) {
        ସ୍ଥିତି 'u':
            ଫେରନ୍ତୁ ultag_isUnicodeExtensionSubtags(s, len);
        ସ୍ଥିତି 't':
            ଫେରନ୍ତୁ ultag_isTransformedExtensionSubtags(s, len);
        ସ୍ଥିତି 'x':
            ଫେରନ୍ତୁ ultag_is_Privateuse_ମୂଲ୍ୟ_Subtags(s, len);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(ଅଚଳ Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    ଯଦି (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    ଫେରନ୍ତୁ *ଏହି;
}

ନାମାକାଶ {

ଶୂନ୍ୟ setField(StringPiece input, ଅକ୍ଷର* dest, UErrorCode& errorCode,
              ତାର୍କିକ (*test)(ଅଚଳ ଅକ୍ଷର*, int32_t)) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଯଦି (input.empty()) {
        dest[0] = '\0';
    } ଅନ୍ୟଥା ଯଦି (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } ଅନ୍ୟଥା {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    ଫେରନ୍ତୁ *ଏହି;
}

ନାମାକାଶ {

ଶୂନ୍ୟ transform(ଅକ୍ଷର* data, int32_t len) {
    ପାଇଁ (int32_t i = 0; i < len; i++, data++) {
        ଯଦି (*data == '_') {
            *data = '-';
        } ଅନ୍ୟଥା {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        ଫେରନ୍ତୁ *ଏହି;
    }
    FixedString* new_variant = ନୂତନ FixedString(variant);
    ଯଦି (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    transform(new_variant->getAlias(), variant.length());
    ଯଦି (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    delete variant_;
    variant_ = new_variant;
    ଫେରନ୍ତୁ *ଏହି;
}

ନାମାକାଶ {

ତାର୍କିକ
is_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* ଚାବି, ଅଚଳ ଅକ୍ଷର* value, int32_t ମୂଲ୍ୟ_len)
{
    ଯଦି (ଚାବି[1] == '\0') {
        // one char key
        ଫେରନ୍ତୁ (UPRV_ISALPHANUM(uprv_tolower(ଚାବି[0])) &&
                _isExtensionSubtags(ଚାବି[0], value, ମୂଲ୍ୟ_len));
    } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(ଚାବି, k_Attribute_ଚାବି) == 0) {
        // unicode attributes
        ଫେରନ୍ତୁ ultag_isUnicodeLocaleAttributes(value, ମୂଲ୍ୟ_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_ଚାବି = ulocimp_to_Bcp_ଚାବି_With_Fallback(ଚାବି);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(ଚାବି, value);

    ଫେରନ୍ତୁ unicode_locale_ଚାବି.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_ଚାବି(unicode_locale_ଚାବି->data(),
                                    static_cast<int32_t>(unicode_locale_ଚାବି->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

ଶୂନ୍ୟ
_copyExtensions(ଅଚଳ Locale& from, icu::StringEnumeration *keywords,
                Locale& to, ତାର୍କିକ validate, UErrorCode& errorCode)
{
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    ଯଦି (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        ଯଦି (U_FAILURE(errorCode) || ownedKeywords.isNull()) { ଫେରନ୍ତୁ; }
        keywords = ownedKeywords.getAlias();
    }
    ଅଚଳ ଅକ୍ଷର* ଚାବି;
    ଯେତେବେଳେ ((ଚାବି = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_ମୂଲ୍ୟ<CharString>(ଚାବି, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        ଯଦି (uprv_strcmp(ଚାବି, k_Attribute_ଚାବି) == 0) {
            transform(value.data(), value.length());
        }
        ଯଦି (validate &&
            !is_Keyword_ମୂଲ୍ୟ(ଚାବି, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
        to.set_Keyword_ମୂଲ୍ୟ(ଚାବି, value.data(), errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    }
}

ଶୂନ୍ୟ
clear_UAttributes_And_ଚାବି_Type(Locale& locale, UErrorCode& errorCode)
{
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    // Clear Unicode attributes
    locale.set_Keyword_ମୂଲ୍ୟ(k_Attribute_ଚାବି, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    ଯଦି (U_FAILURE(errorCode) || iter.isNull()) { ଫେରନ୍ତୁ; }
    ଅଚଳ ଅକ୍ଷର* ଚାବି;
    ଯେତେବେଳେ ((ଚାବି = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_ମୂଲ୍ୟ(ଚାବି, nullptr, errorCode);
    }
}

ଶୂନ୍ୟ
_setUnicodeExtensions(Locale& locale, ଅଚଳ CharString& value, UErrorCode& errorCode)
{
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, ମିଥ୍ୟା, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(ଅକ୍ଷର ଚାବି, StringPiece value) U_LIFETIME_BOUND
{
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (!UPRV_ISALPHANUM(ଚାବି)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    CharString ମୂଲ୍ୟ_str(value, status_);
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    transform(ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length());
    ଯଦି (!ମୂଲ୍ୟ_str.isEmpty() &&
            !_isExtensionSubtags(ଚାବି, ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (extensions_ == nullptr) {
        extensions_ = Locale::get_ମୂଳ().clone();
        ଯଦି (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
    }
    ଯଦି (uprv_tolower(ଚାବି) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_ମୂଲ୍ୟ(StringPiece(&ଚାବି, 1), ମୂଲ୍ୟ_str.data(),
                                     status_);
        ଫେରନ୍ତୁ *ଏହି;
    }
    clear_UAttributes_And_ଚାବି_Type(*extensions_, status_);
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (!value.empty()) {
        _setUnicodeExtensions(*extensions_, ମୂଲ୍ୟ_str, status_);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece ଚାବି,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (!ultag_is_Unicode_Locale_ଚାବି(ଚାବି.data(), ଚାବି.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (extensions_ == nullptr) {
        extensions_ = Locale::get_ମୂଳ().clone();
        ଯଦି (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
    }
    extensions_->set_Unicode_Keyword_ମୂଲ୍ୟ(ଚାବି, type, status_);
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ମୂଲ୍ୟ_str(value, status_);
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    transform(ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length());
    ଯଦି (!ultag_isUnicodeLocaleAttribute(ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (extensions_ == nullptr) {
        extensions_ = Locale::get_ମୂଳ().clone();
        ଯଦି (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
        extensions_->set_Keyword_ମୂଲ୍ୟ(k_Attribute_ଚାବି, ମୂଲ୍ୟ_str.data(), status_);
        ଫେରନ୍ତୁ *ଏହି;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_ମୂଲ୍ୟ<CharString>(k_Attribute_ଚାବି, localErrorCode);
    ଯଦି (U_FAILURE(localErrorCode)) {
        CharString new_attributes(ମୂଲ୍ୟ_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_ମୂଲ୍ୟ(k_Attribute_ଚାବି, new_attributes.data(), status_);
        ଫେରନ୍ତୁ *ଏହି;
    }

    transform(attributes.data(),attributes.length());
    ଅଚଳ ଅକ୍ଷର* start = attributes.data();
    ଅଚଳ ଅକ୍ଷର* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    ତାର୍କିକ inserted = ମିଥ୍ୟା;
    ଯେତେବେଳେ (start < limit) {
        ଯଦି (!inserted) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp = uprv_strcmp(start, ମୂଲ୍ୟ_str.data());
            ଯଦି (cmp == 0) { ଫେରନ୍ତୁ *ଏହି; }  // Found it in attributes: Just return
            ଯଦି (cmp > 0) {
                ଯଦି (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(ମୂଲ୍ୟ_str.data(), status_);
                inserted = ସତ୍ୟ;
            }
        }
        ଯଦି (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    ଯଦି (!inserted) {
        ଯଦି (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(ମୂଲ୍ୟ_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_ମୂଲ୍ୟ(k_Attribute_ଚାବି, new_attributes.data(), status_);
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString ମୂଲ୍ୟ_str(value, status_);
    ଯଦି (U_FAILURE(status_)) { ଫେରନ୍ତୁ *ଏହି; }
    transform(ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length());
    ଯଦି (!ultag_isUnicodeLocaleAttribute(ମୂଲ୍ୟ_str.data(), ମୂଲ୍ୟ_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (extensions_ == nullptr) { ଫେରନ୍ତୁ *ଏହି; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_ମୂଲ୍ୟ<CharString>(k_Attribute_ଚାବି, localErrorCode);
    // get failure, just return
    ଯଦି (U_FAILURE(localErrorCode)) { ଫେରନ୍ତୁ *ଏହି; }
    // Do not have any attributes, just return.
    ଯଦି (attributes.isEmpty()) { ଫେରନ୍ତୁ *ଏହି; }

    ଅକ୍ଷର* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    ପାଇଁ (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ଅଚଳ ଅକ୍ଷର* start = attributes.data();
    ଅଚଳ ଅକ୍ଷର* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    ତାର୍କିକ found = ମିଥ୍ୟା;
    ଯେତେବେଳେ (start < limit) {
        ଯଦି (uprv_strcmp(start, ମୂଲ୍ୟ_str.data()) == 0) {
            found = ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            ଯଦି (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    ଯଦି (found) {
        extensions_->set_Keyword_ମୂଲ୍ୟ(k_Attribute_ଚାବି, new_attributes.data(), status_);
    }
    ଫେରନ୍ତୁ *ଏହି;
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
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    ଫେରନ୍ତୁ *ଏହି;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  ଫେରନ୍ତୁ bogus;
}

ଶୂନ୍ୟ LocaleBuilder::copyExtensionsFrom(ଅଚଳ Locale& src, UErrorCode& errorCode)
{
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    ଯଦି (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        ଫେରନ୍ତୁ;
    }
    ଯଦି (extensions_ == nullptr) {
        extensions_ = Locale::get_ମୂଳ().clone();
        ଯଦି (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, ମିଥ୍ୟା, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ makeBogusLocale();
    }
    ଯଦି (U_FAILURE(status_)) {
        errorCode = status_;
        ଫେରନ୍ତୁ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    ଯଦି (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    ଯଦି (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    ଯଦି (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ makeBogusLocale();
    }
    Locale product(locale_str.data());
    ଯଦି (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, ସତ୍ୟ, errorCode);
    }
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ makeBogusLocale();
    }
    ଫେରନ୍ତୁ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    outErrorCode = status_;
    ଫେରନ୍ତୁ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
