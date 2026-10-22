// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश <optional>
#समावेश <string_view>
#समावेश <utility>

#समावेश "bytesinkutil.h"  // StringByteSink<CharString>
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "fixedstring.h"
#समावेश "ulocimp.h"
#समावेश "unicode/localebuilder.h"
#समावेश "unicode/locid.h"

namespace {

अंतरभूत bool UPRV_ISDIGIT(अक्षर c) { वापस c >= '0' && c <= '9'; }
अंतरभूत bool UPRV_ISALPHANUM(अक्षर c) { वापस uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr स्थिर अक्षर* k_Attribute_कुंजी = "attribute";

bool _isExtensionSubtags(अक्षर कुंजी, स्थिर अक्षर* s, int32_t len) {
    चयन (uprv_tolower(कुंजी)) {
        हाल 'u':
            वापस ultag_isUnicodeExtensionSubtags(s, len);
        हाल 't':
            वापस ultag_isTransformedExtensionSubtags(s, len);
        हाल 'x':
            वापस ultag_is_Privateuse_मूल्य_Subtags(s, len);
        शेष:
            वापस ultag_isExtensionSubtags(s, len);
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
    मिटाओ variant_;
    मिटाओ extensions_;
}

LocaleBuilder& LocaleBuilder::setLocale(स्थिर Locale& locale) U_LIFETIME_BOUND
{
    सफा();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    अगर (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    अगर (U_FAILURE(status_)) { वापस *यह; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    वापस *यह;
}

namespace {

व्योम setField(StringPiece input, अक्षर* dest, UErrorCode& errorCode,
              bool (*test)(स्थिर अक्षर*, int32_t)) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    अगर (input.empty()) {
        dest[0] = '\0';
    } अन्यथा अगर (test(input.तथ्य(), input.length())) {
        uprv_memcpy(dest, input.तथ्य(), input.length());
        dest[input.length()] = '\0';
    } अन्यथा {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    वापस *यह;
}

namespace {

व्योम transform(अक्षर* तथ्य, int32_t len) {
    क्रम (int32_t i = 0; i < len; i++, तथ्य++) {
        अगर (*तथ्य == '_') {
            *तथ्य = '-';
        } अन्यथा {
            *तथ्य = uprv_tolower(*तथ्य);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    अगर (U_FAILURE(status_)) { वापस *यह; }
    अगर (variant.empty()) {
        मिटाओ variant_;
        variant_ = nullptr;
        वापस *यह;
    }
    FixedString* new_variant = नया FixedString(variant);
    अगर (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        वापस *यह;
    }
    transform(new_variant->getAlias(), variant.length());
    अगर (!ultag_isVariantSubtags(new_variant->तथ्य(), variant.length())) {
        मिटाओ new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    मिटाओ variant_;
    variant_ = new_variant;
    वापस *यह;
}

namespace {

bool
is_Keyword_मूल्य(स्थिर अक्षर* कुंजी, स्थिर अक्षर* value, int32_t मूल्य_len)
{
    अगर (कुंजी[1] == '\0') {
        // one char key
        वापस (UPRV_ISALPHANUM(uprv_tolower(कुंजी[0])) &&
                _isExtensionSubtags(कुंजी[0], value, मूल्य_len));
    } अन्यथा अगर (uprv_strcmp(कुंजी, k_Attribute_कुंजी) == 0) {
        // unicode attributes
        वापस ultag_isUnicodeLocaleAttributes(value, मूल्य_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_कुंजी = ulocimp_to_Bcp_कुंजी_With_Fallback(कुंजी);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(कुंजी, value);

    वापस unicode_locale_कुंजी.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_कुंजी(unicode_locale_कुंजी->तथ्य(),
                                    static_cast<int32_t>(unicode_locale_कुंजी->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->तथ्य(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

व्योम
_copyExtensions(स्थिर Locale& from, icu::StringEnumeration *keywords,
                Locale& to, bool validate, UErrorCode& errorCode)
{
    अगर (U_FAILURE(errorCode)) { वापस; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    अगर (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        अगर (U_FAILURE(errorCode) || ownedKeywords.isNull()) { वापस; }
        keywords = ownedKeywords.getAlias();
    }
    स्थिर अक्षर* कुंजी;
    जबतक ((कुंजी = keywords->next(nullptr, errorCode)) != nullptr) {
        स्वतः value = from.get_Keyword_मूल्य<CharString>(कुंजी, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        अगर (uprv_strcmp(कुंजी, k_Attribute_कुंजी) == 0) {
            transform(value.तथ्य(), value.length());
        }
        अगर (validate &&
            !is_Keyword_मूल्य(कुंजी, value.तथ्य(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        to.set_Keyword_मूल्य(कुंजी, value.तथ्य(), errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
    }
}

व्योम
clear_UAttributes_And_कुंजी_Type(Locale& locale, UErrorCode& errorCode)
{
    अगर (U_FAILURE(errorCode)) { वापस; }
    // Clear Unicode attributes
    locale.set_Keyword_मूल्य(k_Attribute_कुंजी, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    अगर (U_FAILURE(errorCode) || iter.isNull()) { वापस; }
    स्थिर अक्षर* कुंजी;
    जबतक ((कुंजी = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_मूल्य(कुंजी, nullptr, errorCode);
    }
}

व्योम
_setUnicodeExtensions(Locale& locale, स्थिर CharString& value, UErrorCode& errorCode)
{
    अगर (U_FAILURE(errorCode)) { वापस; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.तथ्य(), errorCode), nullptr,
        locale, false, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(अक्षर कुंजी, StringPiece value) U_LIFETIME_BOUND
{
    अगर (U_FAILURE(status_)) { वापस *यह; }
    अगर (!UPRV_ISALPHANUM(कुंजी)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    CharString मूल्य_str(value, status_);
    अगर (U_FAILURE(status_)) { वापस *यह; }
    transform(मूल्य_str.तथ्य(), मूल्य_str.length());
    अगर (!मूल्य_str.isEmpty() &&
            !_isExtensionSubtags(कुंजी, मूल्य_str.तथ्य(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    अगर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूल().clone();
        अगर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
    }
    अगर (uprv_tolower(कुंजी) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_मूल्य(StringPiece(&कुंजी, 1), मूल्य_str.तथ्य(),
                                     status_);
        वापस *यह;
    }
    clear_UAttributes_And_कुंजी_Type(*extensions_, status_);
    अगर (U_FAILURE(status_)) { वापस *यह; }
    अगर (!value.empty()) {
        _setUnicodeExtensions(*extensions_, मूल्य_str, status_);
    }
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece कुंजी,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    अगर (U_FAILURE(status_)) { वापस *यह; }
    अगर (!ultag_is_Unicode_Locale_कुंजी(कुंजी.तथ्य(), कुंजी.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.तथ्य(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      वापस *यह;
    }
    अगर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूल().clone();
        अगर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
    }
    extensions_->set_Unicode_Keyword_मूल्य(कुंजी, type, status_);
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    अगर (U_FAILURE(status_)) { वापस *यह; }
    transform(मूल्य_str.तथ्य(), मूल्य_str.length());
    अगर (!ultag_isUnicodeLocaleAttribute(मूल्य_str.तथ्य(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    अगर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूल().clone();
        अगर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
        extensions_->set_Keyword_मूल्य(k_Attribute_कुंजी, मूल्य_str.तथ्य(), status_);
        वापस *यह;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_कुंजी, localErrorCode);
    अगर (U_FAILURE(localErrorCode)) {
        CharString new_attributes(मूल्य_str.तथ्य(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_मूल्य(k_Attribute_कुंजी, new_attributes.तथ्य(), status_);
        वापस *यह;
    }

    transform(attributes.तथ्य(),attributes.length());
    स्थिर अक्षर* start = attributes.तथ्य();
    स्थिर अक्षर* limit = attributes.तथ्य() + attributes.length();
    CharString new_attributes;
    bool inserted = false;
    जबतक (start < limit) {
        अगर (!inserted) {
            पूर्णांक cmp = uprv_strcmp(start, मूल्य_str.तथ्य());
            अगर (cmp == 0) { वापस *यह; }  // Found it in attributes: Just return
            अगर (cmp > 0) {
                अगर (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(मूल्य_str.तथ्य(), status_);
                inserted = true;
            }
        }
        अगर (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    अगर (!inserted) {
        अगर (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(मूल्य_str.तथ्य(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_मूल्य(k_Attribute_कुंजी, new_attributes.तथ्य(), status_);
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    अगर (U_FAILURE(status_)) { वापस *यह; }
    transform(मूल्य_str.तथ्य(), मूल्य_str.length());
    अगर (!ultag_isUnicodeLocaleAttribute(मूल्य_str.तथ्य(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        वापस *यह;
    }
    अगर (extensions_ == nullptr) { वापस *यह; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_कुंजी, localErrorCode);
    // get failure, just return
    अगर (U_FAILURE(localErrorCode)) { वापस *यह; }
    // Do not have any attributes, just return.
    अगर (attributes.isEmpty()) { वापस *यह; }

    अक्षर* p = attributes.तथ्य();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    क्रम (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    स्थिर अक्षर* start = attributes.तथ्य();
    स्थिर अक्षर* limit = attributes.तथ्य() + attributes.length();
    CharString new_attributes;
    bool found = false;
    जबतक (start < limit) {
        अगर (uprv_strcmp(start, मूल्य_str.तथ्य()) == 0) {
            found = true;
        } अन्यथा {
            अगर (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    अगर (found) {
        extensions_->set_Keyword_मूल्य(k_Attribute_कुंजी, new_attributes.तथ्य(), status_);
    }
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::सफा() U_LIFETIME_BOUND
{
    status_ = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    मिटाओ variant_;
    variant_ = nullptr;
    clearExtensions();
    वापस *यह;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    मिटाओ extensions_;
    extensions_ = nullptr;
    वापस *यह;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  वापस bogus;
}

व्योम LocaleBuilder::copyExtensionsFrom(स्थिर Locale& src, UErrorCode& errorCode)
{
    अगर (U_FAILURE(errorCode)) { वापस; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    अगर (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        वापस;
    }
    अगर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूल().clone();
        अगर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, false, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    अगर (U_FAILURE(errorCode)) {
        वापस makeBogusLocale();
    }
    अगर (U_FAILURE(status_)) {
        errorCode = status_;
        वापस makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    अगर (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    अगर (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    अगर (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->तथ्य()), errorCode);
    }
    अगर (U_FAILURE(errorCode)) {
        वापस makeBogusLocale();
    }
    Locale product(locale_str.तथ्य());
    अगर (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, true, errorCode);
    }
    अगर (U_FAILURE(errorCode)) {
        वापस makeBogusLocale();
    }
    वापस product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) स्थिर {
    अगर (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        वापस true;
    }
    outErrorCode = status_;
    वापस U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
