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

LocaleBuilder::LocaleBuilder() : UObject(), स्थितिः_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(शून्यसूचकः), extensions_(शून्यसूचकः)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    विलोपय variant_;
    विलोपय extensions_;
}

LocaleBuilder& LocaleBuilder::समुच्चयः_Locale(ध्रुव Locale& locale) U_LIFETIME_BOUND
{
    clear();
    समुच्चयः_Language(locale.getLanguage());
    समुच्चयः_Script(locale.getScript());
    समुच्चयः_Region(locale.getCountry());
    समुच्चयः_Variant(locale.getVariant());
    extensions_ = locale.clone();
    यदि (extensions_ == शून्यसूचकः) {
        स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::समुच्चयः_Language_चिह्नकम्(StringPiece चिह्नकम्) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_चिह्नकम्(चिह्नकम्, स्थितिः_2);
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    समुच्चयः_Locale(l);
    प्रत्यागम *एतत्;
}

नामाकाश {

शून्य समुच्चयः_क्षेत्रम्(StringPiece input, वर्ण* dest, UError_संहिता& error_संहिता,
              तार्किक (*test)(ध्रुव वर्ण*, int32_t)) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    यदि (input.empty()) {
        dest[0] = '\0';
    } अन्यथा यदि (test(input.दत्त(), input.length())) {
        uprv_memcpy(dest, input.दत्त(), input.length());
        dest[input.length()] = '\0';
    } अन्यथा {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::समुच्चयः_Language(StringPiece language) U_LIFETIME_BOUND
{
    समुच्चयः_क्षेत्रम्(language, language_, स्थितिः_2, &ultag_isLanguageSubtag);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::समुच्चयः_Script(StringPiece script) U_LIFETIME_BOUND
{
    समुच्चयः_क्षेत्रम्(script, script_, स्थितिः_2, &ultag_isScriptSubtag);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::समुच्चयः_Region(StringPiece region) U_LIFETIME_BOUND
{
    समुच्चयः_क्षेत्रम्(region, region_, स्थितिः_2, &ultag_isRegionSubtag);
    प्रत्यागम *एतत्;
}

नामाकाश {

शून्य transform(वर्ण* दत्त, int32_t len) {
    प्रति (int32_t i = 0; i < len; i++, दत्त++) {
        यदि (*दत्त == '_') {
            *दत्त = '-';
        } अन्यथा {
            *दत्त = uprv_tolower(*दत्त);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::समुच्चयः_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    यदि (variant.empty()) {
        विलोपय variant_;
        variant_ = शून्यसूचकः;
        प्रत्यागम *एतत्;
    }
    FixedString* new_variant = नूतन FixedString(variant);
    यदि (new_variant == शून्यसूचकः || new_variant->is_रिक्तानि()) {
        स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम *एतत्;
    }
    transform(new_variant->getAlias(), variant.length());
    यदि (!ultag_isVariantSubtags(new_variant->दत्त(), variant.length())) {
        विलोपय new_variant;
        स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    विलोपय variant_;
    variant_ = new_variant;
    प्रत्यागम *एतत्;
}

नामाकाश {

तार्किक
is_Keyword_मानम्(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* value, int32_t मानम्_len)
{
    यदि (कुञ्चिका[1] == '\0') {
        // one char key
        प्रत्यागम (UPRV_ISALPHANUM(uprv_tolower(कुञ्चिका[0])) &&
                _isExtensionSubtags(कुञ्चिका[0], value, मानम्_len));
    } अन्यथा यदि (uprv_strcmp(कुञ्चिका, k_Attribute_कुञ्चिका) == 0) {
        // unicode attributes
        प्रत्यागम ultag_is_Unicode_Locale_गुणाः(value, मानम्_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_कुञ्चिका = ulocimp_गन्तव्यम्_Bcp_कुञ्चिका_With_Fallback(कुञ्चिका);
    std::optional<std::string_view> unicode_locale_type = ulocimp_गन्तव्यम्_Bcp_Type_With_Fallback(कुञ्चिका, value);

    प्रत्यागम unicode_locale_कुञ्चिका.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_कुञ्चिका(unicode_locale_कुञ्चिका->दत्त(),
                                    static_cast<int32_t>(unicode_locale_कुञ्चिका->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->दत्त(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

शून्य
_copyExtensions(ध्रुव Locale& from, icu::StringEnumeration *keywords,
                Locale& गन्तव्यम्, तार्किक validate, UError_संहिता& error_संहिता)
{
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> ownedKeywords;
    यदि (keywords == शून्यसूचकः) {
        ownedKeywords.adoptInstead(from.createKeywords(error_संहिता));
        यदि (U_FAILURE(error_संहिता) || ownedKeywords.isNull()) { प्रत्यागम; }
        keywords = ownedKeywords.getAlias();
    }
    ध्रुव वर्ण* कुञ्चिका;
    यावत् ((कुञ्चिका = keywords->next(शून्यसूचकः, error_संहिता)) != शून्यसूचकः) {
        स्वचालित value = from.get_Keyword_मानम्<CharString>(कुञ्चिका, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        यदि (uprv_strcmp(कुञ्चिका, k_Attribute_कुञ्चिका) == 0) {
            transform(value.दत्त(), value.length());
        }
        यदि (validate &&
            !is_Keyword_मानम्(कुञ्चिका, value.दत्त(), value.length())) {
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        गन्तव्यम्.समुच्चयः_Keyword_मानम्(कुञ्चिका, value.दत्त(), error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    }
}

शून्य
clear_UAttributes_And_कुञ्चिका_Type(Locale& locale, UError_संहिता& error_संहिता)
{
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    // Clear Unicode attributes
    locale.समुच्चयः_Keyword_मानम्(k_Attribute_कुञ्चिका, "", error_संहिता);

    // Clear all Unicode keyword values
    LocalPointer<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_संहिता));
    यदि (U_FAILURE(error_संहिता) || iter.isNull()) { प्रत्यागम; }
    ध्रुव वर्ण* कुञ्चिका;
    यावत् ((कुञ्चिका = iter->next(शून्यसूचकः, error_संहिता)) != शून्यसूचकः) {
        locale.समुच्चयः_Unicode_Keyword_मानम्(कुञ्चिका, शून्यसूचकः, error_संहिता);
    }
}

शून्य
समुच्चयः_Unicode_Extensions(Locale& locale, ध्रुव CharString& value, UError_संहिता& error_संहिता)
{
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_संहिता);
    locale_str.append(value, error_संहिता);
    _copyExtensions(
        Locale::for_Language_चिह्नकम्(locale_str.दत्त(), error_संहिता), शून्यसूचकः,
        locale, असत्य, error_संहिता);
}

}  // namespace

LocaleBuilder& LocaleBuilder::समुच्चयः_Extension(वर्ण कुञ्चिका, StringPiece value) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    यदि (!UPRV_ISALPHANUM(कुञ्चिका)) {
        स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    CharString मानम्_str(value, स्थितिः_2);
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.दत्त(), मानम्_str.length());
    यदि (!मानम्_str.is_रिक्तानि() &&
            !_isExtensionSubtags(कुञ्चिका, मानम्_str.दत्त(), मानम्_str.length())) {
        स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == शून्यसूचकः) {
        extensions_ = Locale::get_मूलम्().clone();
        यदि (extensions_ == शून्यसूचकः) {
            स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    यदि (uprv_tolower(कुञ्चिका) != 'u') {
        // for t, x and others extension.
        extensions_->समुच्चयः_Keyword_मानम्(StringPiece(&कुञ्चिका, 1), मानम्_str.दत्त(),
                                     स्थितिः_2);
        प्रत्यागम *एतत्;
    }
    clear_UAttributes_And_कुञ्चिका_Type(*extensions_, स्थितिः_2);
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    यदि (!value.empty()) {
        समुच्चयः_Unicode_Extensions(*extensions_, मानम्_str, स्थितिः_2);
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::समुच्चयः_Unicode_Locale_Keyword(StringPiece कुञ्चिका,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    यदि (!ultag_is_Unicode_Locale_कुञ्चिका(कुञ्चिका.दत्त(), कुञ्चिका.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.दत्त(), type.length()))) {
      स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
      प्रत्यागम *एतत्;
    }
    यदि (extensions_ == शून्यसूचकः) {
        extensions_ = Locale::get_मूलम्().clone();
        यदि (extensions_ == शून्यसूचकः) {
            स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    extensions_->समुच्चयः_Unicode_Keyword_मानम्(कुञ्चिका, type, स्थितिः_2);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::सङ्कलय_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मानम्_str(value, स्थितिः_2);
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.दत्त(), मानम्_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मानम्_str.दत्त(), मानम्_str.length())) {
        स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == शून्यसूचकः) {
        extensions_ = Locale::get_मूलम्().clone();
        यदि (extensions_ == शून्यसूचकः) {
            स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
        extensions_->समुच्चयः_Keyword_मानम्(k_Attribute_कुञ्चिका, मानम्_str.दत्त(), स्थितिः_2);
        प्रत्यागम *एतत्;
    }

    UError_संहिता local_Error_संहिता = U_ZERO_ERROR;
    स्वचालित गुणाः = extensions_->get_Keyword_मानम्<CharString>(k_Attribute_कुञ्चिका, local_Error_संहिता);
    यदि (U_FAILURE(local_Error_संहिता)) {
        CharString new_गुणाः(मानम्_str.दत्त(), स्थितिः_2);
        // No attributes, set the attribute.
        extensions_->समुच्चयः_Keyword_मानम्(k_Attribute_कुञ्चिका, new_गुणाः.दत्त(), स्थितिः_2);
        प्रत्यागम *एतत्;
    }

    transform(गुणाः.दत्त(),गुणाः.length());
    ध्रुव वर्ण* start = गुणाः.दत्त();
    ध्रुव वर्ण* सीमा = गुणाः.दत्त() + गुणाः.length();
    CharString new_गुणाः;
    तार्किक inserted = असत्य;
    यावत् (start < सीमा) {
        यदि (!inserted) {
            पूर्णाङ्क cmp = uprv_strcmp(start, मानम्_str.दत्त());
            यदि (cmp == 0) { प्रत्यागम *एतत्; }  // Found it in attributes: Just return
            यदि (cmp > 0) {
                यदि (!new_गुणाः.is_रिक्तानि()) new_गुणाः.append('_', स्थितिः_2);
                new_गुणाः.append(मानम्_str.दत्त(), स्थितिः_2);
                inserted = सत्य;
            }
        }
        यदि (!new_गुणाः.is_रिक्तानि()) {
            new_गुणाः.append('_', स्थितिः_2);
        }
        new_गुणाः.append(start, स्थितिः_2);
        start += uprv_strlen(start) + 1;
    }
    यदि (!inserted) {
        यदि (!new_गुणाः.is_रिक्तानि()) {
            new_गुणाः.append('_', स्थितिः_2);
        }
        new_गुणाः.append(मानम्_str.दत्त(), स्थितिः_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->समुच्चयः_Keyword_मानम्(k_Attribute_कुञ्चिका, new_गुणाः.दत्त(), स्थितिः_2);
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString मानम्_str(value, स्थितिः_2);
    यदि (U_FAILURE(स्थितिः_2)) { प्रत्यागम *एतत्; }
    transform(मानम्_str.दत्त(), मानम्_str.length());
    यदि (!ultag_isUnicodeLocaleAttribute(मानम्_str.दत्त(), मानम्_str.length())) {
        स्थितिः_2 = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम *एतत्;
    }
    यदि (extensions_ == शून्यसूचकः) { प्रत्यागम *एतत्; }
    UError_संहिता local_Error_संहिता = U_ZERO_ERROR;
    स्वचालित गुणाः = extensions_->get_Keyword_मानम्<CharString>(k_Attribute_कुञ्चिका, local_Error_संहिता);
    // get failure, just return
    यदि (U_FAILURE(local_Error_संहिता)) { प्रत्यागम *एतत्; }
    // Do not have any attributes, just return.
    यदि (गुणाः.is_रिक्तानि()) { प्रत्यागम *एतत्; }

    वर्ण* p = गुणाः.दत्त();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    प्रति (int32_t i = 0; i < गुणाः.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ध्रुव वर्ण* start = गुणाः.दत्त();
    ध्रुव वर्ण* सीमा = गुणाः.दत्त() + गुणाः.length();
    CharString new_गुणाः;
    तार्किक प्राप्तम् = असत्य;
    यावत् (start < सीमा) {
        यदि (uprv_strcmp(start, मानम्_str.दत्त()) == 0) {
            प्राप्तम् = सत्य;
        } अन्यथा {
            यदि (!new_गुणाः.is_रिक्तानि()) {
                new_गुणाः.append('_', स्थितिः_2);
            }
            new_गुणाः.append(start, स्थितिः_2);
        }
        start += uprv_strlen(start) + 1;
    }
    // Found the value in attributes, set the attribute.
    यदि (प्राप्तम्) {
        extensions_->समुच्चयः_Keyword_मानम्(k_Attribute_कुञ्चिका, new_गुणाः.दत्त(), स्थितिः_2);
    }
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    स्थितिः_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    विलोपय variant_;
    variant_ = शून्यसूचकः;
    clearExtensions();
    प्रत्यागम *एतत्;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    विलोपय extensions_;
    extensions_ = शून्यसूचकः;
    प्रत्यागम *एतत्;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.समुच्चयः_गन्तव्यम्_Bogus();
  प्रत्यागम bogus;
}

शून्य LocaleBuilder::copyExtensionsFrom(ध्रुव Locale& src, UError_संहिता& error_संहिता)
{
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    LocalPointer<icu::StringEnumeration> keywords(src.createKeywords(error_संहिता));
    यदि (U_FAILURE(error_संहिता) || keywords.isNull() || keywords->count(error_संहिता) == 0) {
        // Error, or no extensions to copy.
        प्रत्यागम;
    }
    यदि (extensions_ == शून्यसूचकः) {
        extensions_ = Locale::get_मूलम्().clone();
        यदि (extensions_ == शून्यसूचकः) {
            स्थितिः_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, असत्य, error_संहिता);
}

Locale LocaleBuilder::build(UError_संहिता& error_संहिता)
{
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम makeBogusLocale();
    }
    यदि (U_FAILURE(स्थितिः_2)) {
        error_संहिता = स्थितिः_2;
        प्रत्यागम makeBogusLocale();
    }
    CharString locale_str(language_, error_संहिता);
    यदि (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_संहिता).append(StringPiece(script_), error_संहिता);
    }
    यदि (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_संहिता).append(StringPiece(region_), error_संहिता);
    }
    यदि (variant_ != शून्यसूचकः) {
        locale_str.append('-', error_संहिता).append(StringPiece(variant_->दत्त()), error_संहिता);
    }
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम makeBogusLocale();
    }
    Locale गुणनफलम्(locale_str.दत्त());
    यदि (extensions_ != शून्यसूचकः) {
        _copyExtensions(*extensions_, शून्यसूचकः, गुणनफलम्, सत्य, error_संहिता);
    }
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम makeBogusLocale();
    }
    प्रत्यागम गुणनफलम्;
}

UBool LocaleBuilder::copy_Error_गन्तव्यम्(UError_संहिता &बहिर्गमनम्_Error_संहिता) ध्रुव {
    यदि (U_FAILURE(बहिर्गमनम्_Error_संहिता)) {
        // Do not overwrite the older error code
        प्रत्यागम सत्य;
    }
    बहिर्गमनम्_Error_संहिता = स्थितिः_2;
    प्रत्यागम U_FAILURE(बहिर्गमनम्_Error_संहिता);
}

U_NAMESPACE_अन्तः
