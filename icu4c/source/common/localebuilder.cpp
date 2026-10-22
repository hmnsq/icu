// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব <optional>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "bytesinkutil.h"  // StringByteSink<CharString>
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "fixedstring.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/locid.h"

নামাকাশ {

অন্তঃস্থ তার্কিক UPRV_ISDIGIT(বর্ণ c) { প্রত্যাগম c >= '0' && c <= '9'; }
অন্তঃস্থ তার্কিক UPRV_ISALPHANUM(বর্ণ c) { প্রত্যাগম uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ধ্রুব বর্ণ* k_Attribute_চাবি = "attribute";

তার্কিক _isExtensionSubtags(বর্ণ চাবি, ধ্রুব বর্ণ* s, int32_t len) {
    বিকল্প (uprv_tolower(চাবি)) {
        পক্ষ 'u':
            প্রত্যাগম ultag_isUnicodeExtensionSubtags(s, len);
        পক্ষ 't':
            প্রত্যাগম ultag_isTransformedExtensionSubtags(s, len);
        পক্ষ 'x':
            প্রত্যাগম ultag_is_Privateuse_মান_Subtags(s, len);
        স্বভাব:
            প্রত্যাগম ultag_isExtensionSubtags(s, len);
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

LocaleBuilder& LocaleBuilder::setLocale(ধ্রুব Locale& locale) U_LIFETIME_BOUND
{
    clear();
    setLanguage(locale.getLanguage());
    setScript(locale.getScript());
    setRegion(locale.getCountry());
    setVariant(locale.getVariant());
    extensions_ = locale.clone();
    যদি (extensions_ == nullptr) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
    }
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::setLanguageTag(StringPiece tag) U_LIFETIME_BOUND
{
    Locale l = Locale::forLanguageTag(tag, status_);
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    setLocale(l);
    প্রত্যাগম *এতত্;
}

নামাকাশ {

শূন্য setField(StringPiece input, বর্ণ* dest, UErrorCode& errorCode,
              তার্কিক (*test)(ধ্রুব বর্ণ*, int32_t)) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    যদি (input.empty()) {
        dest[0] = '\0';
    } অন্যথা যদি (test(input.data(), input.length())) {
        uprv_memcpy(dest, input.data(), input.length());
        dest[input.length()] = '\0';
    } অন্যথা {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setLanguage(StringPiece language) U_LIFETIME_BOUND
{
    setField(language, language_, status_, &ultag_isLanguageSubtag);
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::setScript(StringPiece script) U_LIFETIME_BOUND
{
    setField(script, script_, status_, &ultag_isScriptSubtag);
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::setRegion(StringPiece region) U_LIFETIME_BOUND
{
    setField(region, region_, status_, &ultag_isRegionSubtag);
    প্রত্যাগম *এতত্;
}

নামাকাশ {

শূন্য transform(বর্ণ* data, int32_t len) {
    প্রতি (int32_t i = 0; i < len; i++, data++) {
        যদি (*data == '_') {
            *data = '-';
        } অন্যথা {
            *data = uprv_tolower(*data);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::setVariant(StringPiece variant) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    যদি (variant.empty()) {
        delete variant_;
        variant_ = nullptr;
        প্রত্যাগম *এতত্;
    }
    FixedString* new_variant = নূতন FixedString(variant);
    যদি (new_variant == nullptr || new_variant->isEmpty()) {
        status_ = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম *এতত্;
    }
    transform(new_variant->getAlias(), variant.length());
    যদি (!ultag_isVariantSubtags(new_variant->data(), variant.length())) {
        delete new_variant;
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম *এতত্;
    }
    delete variant_;
    variant_ = new_variant;
    প্রত্যাগম *এতত্;
}

নামাকাশ {

তার্কিক
is_Keyword_মান(ধ্রুব বর্ণ* চাবি, ধ্রুব বর্ণ* value, int32_t মান_len)
{
    যদি (চাবি[1] == '\0') {
        // one char key
        প্রত্যাগম (UPRV_ISALPHANUM(uprv_tolower(চাবি[0])) &&
                _isExtensionSubtags(চাবি[0], value, মান_len));
    } অন্যথা যদি (uprv_strcmp(চাবি, k_Attribute_চাবি) == 0) {
        // unicode attributes
        প্রত্যাগম ultag_isUnicodeLocaleAttributes(value, মান_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_চাবি = ulocimp_to_Bcp_চাবি_With_Fallback(চাবি);
    std::optional<std::string_view> unicode_locale_type = ulocimp_toBcpTypeWithFallback(চাবি, value);

    প্রত্যাগম unicode_locale_চাবি.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_চাবি(unicode_locale_চাবি->data(),
                                    static_cast<int32_t>(unicode_locale_চাবি->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->data(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

শূন্য
_copyExtensions(ধ্রুব Locale& from, icu::StringEnumeration *keywords,
                Locale& to, তার্কিক validate, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    যদি (keywords == nullptr) {
        ownedKeywords.adoptInstead(from.createKeywords(errorCode));
        যদি (U_FAILURE(errorCode) || ownedKeywords.isNull()) { প্রত্যাগম; }
        keywords = ownedKeywords.getAlias();
    }
    ধ্রুব বর্ণ* চাবি;
    যাবত্ ((চাবি = keywords->next(nullptr, errorCode)) != nullptr) {
        auto value = from.get_Keyword_মান<CharString>(চাবি, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        যদি (uprv_strcmp(চাবি, k_Attribute_চাবি) == 0) {
            transform(value.data(), value.length());
        }
        যদি (validate &&
            !is_Keyword_মান(চাবি, value.data(), value.length())) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }
        to.set_Keyword_মান(চাবি, value.data(), errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    }
}

শূন্য
clear_UAttributes_And_চাবি_Type(Locale& locale, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    // Clear Unicode attributes
    locale.set_Keyword_মান(k_Attribute_চাবি, "", errorCode);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(errorCode));
    যদি (U_FAILURE(errorCode) || iter.isNull()) { প্রত্যাগম; }
    ধ্রুব বর্ণ* চাবি;
    যাবত্ ((চাবি = iter->next(nullptr, errorCode)) != nullptr) {
        locale.set_Unicode_Keyword_মান(চাবি, nullptr, errorCode);
    }
}

শূন্য
_setUnicodeExtensions(Locale& locale, ধ্রুব CharString& value, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", errorCode);
    locale_str.append(value, errorCode);
    _copyExtensions(
        Locale::forLanguageTag(locale_str.data(), errorCode), nullptr,
        locale, অসত্য, errorCode);
}

}  // namespace

LocaleBuilder& LocaleBuilder::setExtension(বর্ণ চাবি, StringPiece value) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    যদি (!UPRV_ISALPHANUM(চাবি)) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম *এতত্;
    }
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    transform(মান_str.data(), মান_str.length());
    যদি (!মান_str.isEmpty() &&
            !_isExtensionSubtags(চাবি, মান_str.data(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম *এতত্;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
    }
    যদি (uprv_tolower(চাবি) != 'u') {
        // for t, x and others extension.
        extensions_->set_Keyword_মান(StringPiece(&চাবি, 1), মান_str.data(),
                                     status_);
        প্রত্যাগম *এতত্;
    }
    clear_UAttributes_And_চাবি_Type(*extensions_, status_);
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    যদি (!value.empty()) {
        _setUnicodeExtensions(*extensions_, মান_str, status_);
    }
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::setUnicodeLocaleKeyword(StringPiece চাবি,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    যদি (!ultag_is_Unicode_Locale_চাবি(চাবি.data(), চাবি.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.data(), type.length()))) {
      status_ = U_ILLEGAL_ARGUMENT_ERROR;
      প্রত্যাগম *এতত্;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
    }
    extensions_->set_Unicode_Keyword_মান(চাবি, type, status_);
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::addUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    transform(মান_str.data(), মান_str.length());
    যদি (!ultag_isUnicodeLocaleAttribute(মান_str.data(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম *এতত্;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
        extensions_->set_Keyword_মান(k_Attribute_চাবি, মান_str.data(), status_);
        প্রত্যাগম *এতত্;
    }

    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_মান<CharString>(k_Attribute_চাবি, localErrorCode);
    যদি (U_FAILURE(localErrorCode)) {
        CharString new_attributes(মান_str.data(), status_);
        // No attributes, set the attribute.
        extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.data(), status_);
        প্রত্যাগম *এতত্;
    }

    transform(attributes.data(),attributes.length());
    ধ্রুব বর্ণ* start = attributes.data();
    ধ্রুব বর্ণ* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    তার্কিক inserted = অসত্য;
    যাবত্ (start < limit) {
        যদি (!inserted) {
            পূর্ণাঙ্ক cmp = uprv_strcmp(start, মান_str.data());
            যদি (cmp == 0) { প্রত্যাগম *এতত্; }  // Found it in attributes: Just return
            যদি (cmp > 0) {
                যদি (!new_attributes.isEmpty()) new_attributes.append('_', status_);
                new_attributes.append(মান_str.data(), status_);
                inserted = সত্য;
            }
        }
        যদি (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(start, status_);
        start += uprv_strlen(start) + 1;
    }
    যদি (!inserted) {
        যদি (!new_attributes.isEmpty()) {
            new_attributes.append('_', status_);
        }
        new_attributes.append(মান_str.data(), status_);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.data(), status_);
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString মান_str(value, status_);
    যদি (U_FAILURE(status_)) { প্রত্যাগম *এতত্; }
    transform(মান_str.data(), মান_str.length());
    যদি (!ultag_isUnicodeLocaleAttribute(মান_str.data(), মান_str.length())) {
        status_ = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম *এতত্;
    }
    যদি (extensions_ == nullptr) { প্রত্যাগম *এতত্; }
    UErrorCode localErrorCode = U_ZERO_ERROR;
    auto attributes = extensions_->get_Keyword_মান<CharString>(k_Attribute_চাবি, localErrorCode);
    // get failure, just return
    যদি (U_FAILURE(localErrorCode)) { প্রত্যাগম *এতত্; }
    // Do not have any attributes, just return.
    যদি (attributes.isEmpty()) { প্রত্যাগম *এতত্; }

    বর্ণ* p = attributes.data();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    প্রতি (int32_t i = 0; i < attributes.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ধ্রুব বর্ণ* start = attributes.data();
    ধ্রুব বর্ণ* limit = attributes.data() + attributes.length();
    CharString new_attributes;
    তার্কিক found = অসত্য;
    যাবত্ (start < limit) {
        যদি (uprv_strcmp(start, মান_str.data()) == 0) {
            found = সত্য;
        } অন্যথা {
            যদি (!new_attributes.isEmpty()) {
                new_attributes.append('_', status_);
            }
            new_attributes.append(start, status_);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    যদি (found) {
        extensions_->set_Keyword_মান(k_Attribute_চাবি, new_attributes.data(), status_);
    }
    প্রত্যাগম *এতত্;
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
    প্রত্যাগম *এতত্;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    delete extensions_;
    extensions_ = nullptr;
    প্রত্যাগম *এতত্;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.setToBogus();
  প্রত্যাগম bogus;
}

শূন্য LocaleBuilder::copyExtensionsFrom(ধ্রুব Locale& src, UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(errorCode));
    যদি (U_FAILURE(errorCode) || keywords.isNull() || keywords->count(errorCode) == 0) {
        // Error, or no extensions to copy.
        প্রত্যাগম;
    }
    যদি (extensions_ == nullptr) {
        extensions_ = Locale::get_মূল().clone();
        যদি (extensions_ == nullptr) {
            status_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, অসত্য, errorCode);
}

Locale LocaleBuilder::build(UErrorCode& errorCode)
{
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম makeBogusLocale();
    }
    যদি (U_FAILURE(status_)) {
        errorCode = status_;
        প্রত্যাগম makeBogusLocale();
    }
    CharString locale_str(language_, errorCode);
    যদি (uprv_strlen(script_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(script_), errorCode);
    }
    যদি (uprv_strlen(region_) > 0) {
        locale_str.append('-', errorCode).append(StringPiece(region_), errorCode);
    }
    যদি (variant_ != nullptr) {
        locale_str.append('-', errorCode).append(StringPiece(variant_->data()), errorCode);
    }
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম makeBogusLocale();
    }
    Locale product(locale_str.data());
    যদি (extensions_ != nullptr) {
        _copyExtensions(*extensions_, nullptr, product, সত্য, errorCode);
    }
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম makeBogusLocale();
    }
    প্রত্যাগম product;
}

UBool LocaleBuilder::copyErrorTo(UErrorCode &outErrorCode) ধ্রুব {
    যদি (U_FAILURE(outErrorCode)) {
        // Do not overwrite the older error code
        প্রত্যাগম সত্য;
    }
    outErrorCode = status_;
    প্রত্যাগম U_FAILURE(outErrorCode);
}

U_NAMESPACE_END
