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

ओळीत तार्किक UPRV_ISDIGIT(अक्षर c) { परत c >= '0' && c <= '9'; }
ओळीत तार्किक UPRV_ISALPHANUM(अक्षर c) { परत uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr स्थिरांक अक्षर* k_Attribute_किल्ली = "attribute";

तार्किक _isExtensionSubtags(अक्षर किल्ली, स्थिरांक अक्षर* s, int32_t len) {
    निवड (uprv_tolower(किल्ली)) {
        स्थिती 'u':
            परत ultag_isUnicodeExtensionSubtags(s, len);
        स्थिती 't':
            परत ultag_isTransformedExtensionSubtags(s, len);
        स्थिती 'x':
            परत ultag_is_Privateuse_मूल्य_Subtags(s, len);
        पूर्वनिर्धारित:
            परत ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(स्थिरांक Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    जर (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    परत *हे;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    जर (U_FAILURE(status_)) { परत *हे; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    परत *हे;
}

नामाकाश {

शून्य setField(StringPiece input, अक्षर* dest, UErrorCode& errorCode,
              तार्किक (*test)(स्थिरांक अक्षर*, int32_t)) {
    जर (U_FAILURE(errorCode)) { परत; }
    जर (input.empty()) {
        dest[0] = '\0';
    } अन्यथा जर (test(input.data(), input.length())) {
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
    परत *हे;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    परत *हे;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    परत *हे;
}

नामाकाश {

शून्य transform(अक्षर* data, int32_t len) {
    साठी (int32_t i = 0; i < len; i++, data++) {
        जर (*data == '_') {
            *data = '-';
        } अन्यथा {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    जर (U_FAILURE(status_)) { परत *हे; }
    जर (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        परत *हे;
    }
    FixedString* new_variant = नवीन FixedString(variant);
    जर (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        परत *हे;
    }
    transform(new_variant->getAlias(), variant.length());
    जर (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    delete variant_;
    variant_ = new_variant;
    परत *हे;
}

नामाकाश {

तार्किक
is_Keyword_मूल्य(स्थिरांक अक्षर* किल्ली, स्थिरांक अक्षर* value, int32_t मूल्य_len)
{
    जर (किल्ली[1] == '\0') {
        // one char key
        परत (UPRV_ISALPHANUM(uprv_tolower(किल्ली[0])) &&
                _isExtensionSubtags(किल्ली[0], value, मूल्य_len));
    } अन्यथा जर (uprv_strcmp(किल्ली, k_Attribute_किल्ली) == 0) {
        // unicode attributes
        परत ultag_isUnicodeLocaleAttributes(value, मूल्य_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_किल्ली = ulocimp_to_Bcp_किल्ली_With_Fallback(किल्ली);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(किल्ली, value);

    परत unicode_locale_किल्ली.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_किल्ली(unicode_locale_किल्ली->data(),
                                    static_cast<int32_t>(unicode_locale_किल्ली->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

शून्य
_copyExtensions(स्थिरांक Locale& from, icu::StringEnumeration *keywords,
                Locale& to, तार्किक validate, UErrorCode& errorCode)
{
    जर (U_FAILURE(errorCode)) { परत; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    जर (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        जर (U_FAILURE(errorCode) || ownedKeywords.isNull()) { परत; }
        keywords = ownedKeywords.getAlias();
    }
    स्थिरांक अक्षर* किल्ली;
    पर्यंत ((किल्ली = keywords->next(nullptr, errorCode)) != nullptr) {
        स्वतः value = from.get_Keyword_मूल्य<CharString>(किल्ली, errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        जर (uprv_strcmp(किल्ली, k_Attribute_किल्ली) == 0) {
            transform(value.data(), value.length());
        }
        जर (validate &&
            !is_Keyword_मूल्य(किल्ली, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }
        to.set_Keyword_मूल्य(किल्ली, value.data(), errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
    }
}

शून्य
clear_UAttributes_And_किल्ली_Type(Locale& locale, UErrorCode& errorCode)
{
    जर (U_FAILURE(errorCode)) { परत; }
    // Clear Unicode attributes
    locale.set_Keyword_मूल्य(k_Attribute_किल्ली, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    जर (U_FAILURE(errorCode) || iter.isNull()) { परत; }
    स्थिरांक अक्षर* किल्ली;
    पर्यंत ((किल्ली = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_मूल्य(किल्ली, nullptr, errorCode);
    }
}

शून्य
_setUnicodeExtensions(Locale& locale, स्थिरांक CharString& value, UErrorCode& errorCode)
{
    जर (U_FAILURE(errorCode)) { परत; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, असत्य, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(अक्षर किल्ली, StringPiece value) U_LIFETIME_BOUND
{
    जर (U_FAILURE(status_)) { परत *हे; }
    जर (!UPRV_ISALPHANUM(किल्ली)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    CharString मूल्य_str(value, status_);
    जर (U_FAILURE(status_)) { परत *हे; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    जर (!मूल्य_str.isEmpty() &&
            !_isExtensionSubtags(किल्ली, मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    जर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        जर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
    }
    जर (uprv_tolower(किल्ली) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_मूल्य(StringPiece(&किल्ली, 1), मूल्य_str.data(),
                                     status_);
        परत *हे;
    }
    clear_UAttributes_And_किल्ली_Type(*extensions_, status_);
    जर (U_FAILURE(status_)) { परत *हे; }
    जर (!value.empty()) {
        _setUnicodeExtensions(*extensions_, मूल्य_str, status_);
    }
    परत *हे;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece किल्ली,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    जर (U_FAILURE(status_)) { परत *हे; }
    जर (!ultag_is_Unicode_Locale_किल्ली(किल्ली.data(), किल्ली.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      परत *हे;
    }
    जर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        जर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
    }
    extensions_->set_Unicode_Keyword_मूल्य(किल्ली, type, status_);
    परत *हे;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    जर (U_FAILURE(status_)) { परत *हे; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    जर (!ultag_isUnicodeLocaleAttribute(मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    जर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        जर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, मूल्य_str.data(), status_);
        परत *हे;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_किल्ली, localErrorCode);
    जर (U_FAILURE(localErrorCode)) {
        CharString new_attributes(मूल्य_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
        परत *हे;
    }

    transform(attributes.data(),attributes.length());
    स्थिरांक अक्षर* start = attributes.data();
    स्थिरांक अक्षर* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक inserted = असत्य;
    पर्यंत (start < limit) {
        जर (!inserted) {
            पूर्णांक cmp = uprv_strcmp(start, मूल्य_str.data());
            जर (cmp == 0) { परत *हे; }  // Found it in attributes: Just return
            जर (cmp > 0) {
                जर (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(मूल्य_str.data(), status_);
                inserted = सत्य;
            }
        }
        जर (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    जर (!inserted) {
        जर (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(मूल्य_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
    परत *हे;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मूल्य_str(value, status_);
    जर (U_FAILURE(status_)) { परत *हे; }
    transform(मूल्य_str.data(), मूल्य_str.length());
    जर (!ultag_isUnicodeLocaleAttribute(मूल्य_str.data(), मूल्य_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        परत *हे;
    }
    जर (extensions_ == nullptr) { परत *हे; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    स्वतः attributes = extensions_->get_Keyword_मूल्य<CharString>(k_Attribute_किल्ली, localErrorCode);
    // get failure, just return
    जर (U_FAILURE(localErrorCode)) { परत *हे; }
    // Do not have any attributes, just return.
    जर (attributes.isEmpty()) { परत *हे; }

    अक्षर* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    साठी (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    स्थिरांक अक्षर* start = attributes.data();
    स्थिरांक अक्षर* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    तार्किक found = असत्य;
    पर्यंत (start < limit) {
        जर (uprv_strcmp(start, मूल्य_str.data()) == 0) {
            found = सत्य;
        } अन्यथा {
            जर (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    जर (found) {
        extensions_->set_Keyword_मूल्य(k_Attribute_किल्ली, new_attributes.data(), status_);
    }
    परत *हे;
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
    परत *हे;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    परत *हे;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  परत bogus;
}

शून्य LocaleBuilder::copyExtensionsFrom(स्थिरांक Locale& src, UErrorCode& errorCode)
{
    जर (U_FAILURE(errorCode)) { परत; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    जर (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        परत;
    }
    जर (extensions_ == nullptr) {
        extensions_ = Locale::get_मूळ().clone();
        जर (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, असत्य, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    जर (U_FAILURE(errorCode)) {
        परत makeBogusLocale();
    }
    जर (U_FAILURE(status_)) {
        errorCode = status_;
        परत makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    जर (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    जर (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    जर (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    जर (U_FAILURE(errorCode)) {
        परत makeBogusLocale();
    }
    Locale product(locale_str.data());
    जर (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, सत्य, errorCode);
    }
    जर (U_FAILURE(errorCode)) {
        परत makeBogusLocale();
    }
    परत product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) स्थिरांक {
    जर (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        परत सत्य;
    }
    outErrorCode = status_;
    परत U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
