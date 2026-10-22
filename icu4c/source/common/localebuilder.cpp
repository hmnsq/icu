// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "bytesinkutil.h"  // StringByteSink<CharString>
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "fixedstring.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "unicode/localebuilder.h"
#അന്തര്ഭാവ "unicode/locid.h"

നാമാകാശ {

വരിയിൽ തർക്കം UPRV_ISDIGIT(അക്ഷരം c) { തിരികെ c >= '0' && c <= '9'; }
വരിയിൽ തർക്കം UPRV_ISALPHANUM(അക്ഷരം c) { തിരികെ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr സ്ഥിരം അക്ഷരം* k_Attribute_താക്കോൽ = "attribute";

തർക്കം _isExtensionSubtags(അക്ഷരം താക്കോൽ, സ്ഥിരം അക്ഷരം* s, int32_t len) {
    തിരഞ്ഞെടുപ്പ് (uprv_tolower(താക്കോൽ)) {
        അവസ്ഥ 'u':
            തിരികെ ultag_isUnicodeExtensionSubtags(s, len);
        അവസ്ഥ 't':
            തിരികെ ultag_isTransformedExtensionSubtags(s, len);
        അവസ്ഥ 'x':
            തിരികെ ultag_is_Privateuse_മൂല്യം_Subtags(s, len);
        സ്ഥിരസ്ഥിതി:
            തിരികെ ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(സ്ഥിരം Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    എങ്കിൽ (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    തിരികെ *ഇത്;
}

നാമാകാശ {

ശൂന്യം setField(StringPiece input, അക്ഷരം* dest, UErrorCode& errorCode,
              തർക്കം (*test)(സ്ഥിരം അക്ഷരം*, int32_t)) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    എങ്കിൽ (input.empty()) {
        dest[0] = '\0';
    } അല്ലെങ്കിൽ എങ്കിൽ (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } അല്ലെങ്കിൽ {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    തിരികെ *ഇത്;
}

നാമാകാശ {

ശൂന്യം transform(അക്ഷരം* data, int32_t len) {
    വേണ്ടി (int32_t i = 0; i < len; i++, data++) {
        എങ്കിൽ (*data == '_') {
            *data = '-';
        } അല്ലെങ്കിൽ {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    എങ്കിൽ (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        തിരികെ *ഇത്;
    }
    FixedString* new_variant = പുതിയ FixedString(variant);
    എങ്കിൽ (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ *ഇത്;
    }
    transform(new_variant->getAlias(), variant.length());
    എങ്കിൽ (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    delete variant_;
    variant_ = new_variant;
    തിരികെ *ഇത്;
}

നാമാകാശ {

തർക്കം
is_Keyword_മൂല്യം(സ്ഥിരം അക്ഷരം* താക്കോൽ, സ്ഥിരം അക്ഷരം* value, int32_t മൂല്യം_len)
{
    എങ്കിൽ (താക്കോൽ[1] == '\0') {
        // one char key
        തിരികെ (UPRV_ISALPHANUM(uprv_tolower(താക്കോൽ[0])) &&
                _isExtensionSubtags(താക്കോൽ[0], value, മൂല്യം_len));
    } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(താക്കോൽ, k_Attribute_താക്കോൽ) == 0) {
        // unicode attributes
        തിരികെ ultag_isUnicodeLocaleAttributes(value, മൂല്യം_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_താക്കോൽ = ulocimp_to_Bcp_താക്കോൽ_With_Fallback(താക്കോൽ);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(താക്കോൽ, value);

    തിരികെ unicode_locale_താക്കോൽ.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_താക്കോൽ(unicode_locale_താക്കോൽ->data(),
                                    static_cast<int32_t>(unicode_locale_താക്കോൽ->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

ശൂന്യം
_copyExtensions(സ്ഥിരം Locale& from, icu::StringEnumeration *keywords,
                Locale& to, തർക്കം validate, UErrorCode& errorCode)
{
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    എങ്കിൽ (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        എങ്കിൽ (U_FAILURE(errorCode) || ownedKeywords.isNull()) { തിരികെ; }
        keywords = ownedKeywords.getAlias();
    }
    സ്ഥിരം അക്ഷരം* താക്കോൽ;
    ആകുമ്പോൾ ((താക്കോൽ = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_മൂല്യം<CharString>(താക്കോൽ, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        എങ്കിൽ (uprv_strcmp(താക്കോൽ, k_Attribute_താക്കോൽ) == 0) {
            transform(value.data(), value.length());
        }
        എങ്കിൽ (validate &&
            !is_Keyword_മൂല്യം(താക്കോൽ, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
        to.set_Keyword_മൂല്യം(താക്കോൽ, value.data(), errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    }
}

ശൂന്യം
clear_UAttributes_And_താക്കോൽ_Type(Locale& locale, UErrorCode& errorCode)
{
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    // Clear Unicode attributes
    locale.set_Keyword_മൂല്യം(k_Attribute_താക്കോൽ, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    എങ്കിൽ (U_FAILURE(errorCode) || iter.isNull()) { തിരികെ; }
    സ്ഥിരം അക്ഷരം* താക്കോൽ;
    ആകുമ്പോൾ ((താക്കോൽ = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_മൂല്യം(താക്കോൽ, nullptr, errorCode);
    }
}

ശൂന്യം
_setUnicodeExtensions(Locale& locale, സ്ഥിരം CharString& value, UErrorCode& errorCode)
{
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, അസത്യം, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(അക്ഷരം താക്കോൽ, StringPiece value) U_LIFETIME_BOUND
{
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    എങ്കിൽ (!UPRV_ISALPHANUM(താക്കോൽ)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    CharString മൂല്യം_str(value, status_);
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    transform(മൂല്യം_str.data(), മൂല്യം_str.length());
    എങ്കിൽ (!മൂല്യം_str.isEmpty() &&
            !_isExtensionSubtags(താക്കോൽ, മൂല്യം_str.data(), മൂല്യം_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (extensions_ == nullptr) {
        extensions_ = Locale::get_വേര്().clone();
        എങ്കിൽ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
    }
    എങ്കിൽ (uprv_tolower(താക്കോൽ) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_മൂല്യം(StringPiece(&താക്കോൽ, 1), മൂല്യം_str.data(),
                                     status_);
        തിരികെ *ഇത്;
    }
    clear_UAttributes_And_താക്കോൽ_Type(*extensions_, status_);
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    എങ്കിൽ (!value.empty()) {
        _setUnicodeExtensions(*extensions_, മൂല്യം_str, status_);
    }
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece താക്കോൽ,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    എങ്കിൽ (!ultag_is_Unicode_Locale_താക്കോൽ(താക്കോൽ.data(), താക്കോൽ.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      തിരികെ *ഇത്;
    }
    എങ്കിൽ (extensions_ == nullptr) {
        extensions_ = Locale::get_വേര്().clone();
        എങ്കിൽ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
    }
    extensions_->set_Unicode_Keyword_മൂല്യം(താക്കോൽ, type, status_);
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString മൂല്യം_str(value, status_);
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    transform(മൂല്യം_str.data(), മൂല്യം_str.length());
    എങ്കിൽ (!ultag_isUnicodeLocaleAttribute(മൂല്യം_str.data(), മൂല്യം_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (extensions_ == nullptr) {
        extensions_ = Locale::get_വേര്().clone();
        എങ്കിൽ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
        extensions_->set_Keyword_മൂല്യം(k_Attribute_താക്കോൽ, മൂല്യം_str.data(), status_);
        തിരികെ *ഇത്;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_മൂല്യം<CharString>(k_Attribute_താക്കോൽ, localErrorCode);
    എങ്കിൽ (U_FAILURE(localErrorCode)) {
        CharString new_attributes(മൂല്യം_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_മൂല്യം(k_Attribute_താക്കോൽ, new_attributes.data(), status_);
        തിരികെ *ഇത്;
    }

    transform(attributes.data(),attributes.length());
    സ്ഥിരം അക്ഷരം* start = attributes.data();
    സ്ഥിരം അക്ഷരം* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    തർക്കം inserted = അസത്യം;
    ആകുമ്പോൾ (start < limit) {
        എങ്കിൽ (!inserted) {
            പൂർണ്ണസംഖ്യ cmp = uprv_strcmp(start, മൂല്യം_str.data());
            എങ്കിൽ (cmp == 0) { തിരികെ *ഇത്; }  // Found it in attributes: Just return
            എങ്കിൽ (cmp > 0) {
                എങ്കിൽ (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(മൂല്യം_str.data(), status_);
                inserted = സത്യം;
            }
        }
        എങ്കിൽ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    എങ്കിൽ (!inserted) {
        എങ്കിൽ (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(മൂല്യം_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_മൂല്യം(k_Attribute_താക്കോൽ, new_attributes.data(), status_);
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString മൂല്യം_str(value, status_);
    എങ്കിൽ (U_FAILURE(status_)) { തിരികെ *ഇത്; }
    transform(മൂല്യം_str.data(), മൂല്യം_str.length());
    എങ്കിൽ (!ultag_isUnicodeLocaleAttribute(മൂല്യം_str.data(), മൂല്യം_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (extensions_ == nullptr) { തിരികെ *ഇത്; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_മൂല്യം<CharString>(k_Attribute_താക്കോൽ, localErrorCode);
    // get failure, just return
    എങ്കിൽ (U_FAILURE(localErrorCode)) { തിരികെ *ഇത്; }
    // Do not have any attributes, just return.
    എങ്കിൽ (attributes.isEmpty()) { തിരികെ *ഇത്; }

    അക്ഷരം* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    വേണ്ടി (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    സ്ഥിരം അക്ഷരം* start = attributes.data();
    സ്ഥിരം അക്ഷരം* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    തർക്കം found = അസത്യം;
    ആകുമ്പോൾ (start < limit) {
        എങ്കിൽ (uprv_strcmp(start, മൂല്യം_str.data()) == 0) {
            found = സത്യം;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    എങ്കിൽ (found) {
        extensions_->set_Keyword_മൂല്യം(k_Attribute_താക്കോൽ, new_attributes.data(), status_);
    }
    തിരികെ *ഇത്;
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
    തിരികെ *ഇത്;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    തിരികെ *ഇത്;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  തിരികെ bogus;
}

ശൂന്യം LocaleBuilder::copyExtensionsFrom(സ്ഥിരം Locale& src, UErrorCode& errorCode)
{
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    എങ്കിൽ (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        തിരികെ;
    }
    എങ്കിൽ (extensions_ == nullptr) {
        extensions_ = Locale::get_വേര്().clone();
        എങ്കിൽ (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, അസത്യം, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ makeBogusLocale();
    }
    എങ്കിൽ (U_FAILURE(status_)) {
        errorCode = status_;
        തിരികെ makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    എങ്കിൽ (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    എങ്കിൽ (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    എങ്കിൽ (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ makeBogusLocale();
    }
    Locale product(locale_str.data());
    എങ്കിൽ (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, സത്യം, errorCode);
    }
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ makeBogusLocale();
    }
    തിരികെ product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        തിരികെ സത്യം;
    }
    outErrorCode = status_;
    തിരികെ U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
