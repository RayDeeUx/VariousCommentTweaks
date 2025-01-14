#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:

	bool calledAlready = false;

	std::vector<std::string> dislikedWords;
	std::unordered_set<int> ignoredUsers;

	std::string ownUsername;

	// adapted from tastyforreal
	std::unordered_map<std::string, std::string> languages = {
		{"Afrikaans", "af"}, {"Twi", "ak"}, {"Amharic", "am"}, {"Arabic", "ar"}, {"Assamese", "as"},
		{"Detect language", "auto"}, {"Aymara", "ay"}, {"Azerbaijani", "az"}, {"Belarusian", "be"},
		{"Bulgarian", "bg"}, {"Bhojpuri", "bho"}, {"Bambara", "bm"}, {"Bengali", "bn"}, {"Bosnian", "bs"},
		{"Catalan", "ca"}, {"Cebuano", "ceb"}, {"Kurdish (Sorani)", "ckb"}, {"Corsican", "co"}, {"Czech", "cs"},
		{"Welsh", "cy"}, {"Danish", "da"}, {"German", "de"}, {"Dogri", "doi"}, {"Dhivehi", "dv"}, {"Ewe", "ee"},
		{"Greek", "el"}, {"English", "en"}, {"Esperanto", "eo"}, {"Spanish", "es"}, {"Estonian", "et"},
		{"Basque", "eu"}, {"Persian", "fa"}, {"Finnish", "fi"}, {"French", "fr"}, {"Frisian", "fy"},
		{"Irish", "ga"}, {"Scots Gaelic", "gd"}, {"Galician", "gl"}, {"Guarani", "gn"}, {"Konkani", "gom"},
		{"Gujarati", "gu"}, {"Hausa", "ha"}, {"Hawaiian", "haw"}, {"Hindi", "hi"}, {"Hmong", "hmn"},
		{"Croatian", "hr"}, {"Haitian Creole", "ht"}, {"Hungarian", "hu"}, {"Armenian", "hy"},
		{"Indonesian", "id"}, {"Igbo", "ig"}, {"Ilocano", "ilo"}, {"Icelandic", "is"}, {"Italian", "it"},
		{"Hebrew", "iw"}, {"Japanese", "ja"}, {"Javanese", "jw"}, {"Georgian", "ka"}, {"Kazakh", "kk"},
		{"Khmer", "km"}, {"Kannada", "kn"}, {"Korean", "ko"}, {"Krio", "kri"}, {"Kurdish (Kurmanji)", "ku"},
		{"Kyrgyz", "ky"}, {"Latin", "la"}, {"Luxembourgish", "lb"}, {"Luganda", "lg"}, {"Lingala", "ln"},
		{"Lao", "lo"}, {"Lithuanian", "lt"}, {"Mizo", "lus"}, {"Latvian", "lv"}, {"Maithili", "mai"},
		{"Malagasy", "mg"}, {"Maori", "mi"}, {"Macedonian", "mk"}, {"Malayalam", "ml"}, {"Mongolian", "mn"},
		{"Meiteilon (Manipuri)", "mni-Mtei"}, {"Marathi", "mr"}, {"Malay", "ms"}, {"Maltese", "mt"},
		{"Myanmar (Burmese)", "my"}, {"Nepali", "ne"}, {"Dutch", "nl"}, {"Norwegian", "no"}, {"Sepedi", "nso"},
		{"Chichewa", "ny"}, {"Oromo", "om"}, {"Odia (Oriya)", "or"}, {"Punjabi", "pa"}, {"Polish", "pl"},
		{"Pashto", "ps"}, {"Portuguese", "pt"}, {"Quechua", "qu"}, {"Romanian", "ro"}, {"Russian", "ru"},
		{"Kinyarwanda", "rw"}, {"Sanskrit", "sa"}, {"Sindhi", "sd"}, {"Sinhala", "si"}, {"Slovak", "sk"},
		{"Slovenian", "sl"}, {"Samoan", "sm"}, {"Shona", "sn"}, {"Somali", "so"}, {"Albanian", "sq"},
		{"Serbian", "sr"}, {"Sesotho", "st"}, {"Sundanese", "su"}, {"Swedish", "sv"}, {"Swahili", "sw"},
		{"Tamil", "ta"}, {"Telugu", "te"}, {"Tajik", "tg"}, {"Thai", "th"}, {"Tigrinya", "ti"},
		{"Turkmen", "tk"}, {"Filipino", "tl"}, {"Turkish", "tr"}, {"Tsonga", "ts"}, {"Tatar", "tt"},
		{"Uyghur", "ug"}, {"Ukrainian", "uk"}, {"Urdu", "ur"}, {"Uzbek", "uz"}, {"Vietnamese", "vi"},
		{"Xhosa", "xh"}, {"Yiddish", "yi"}, {"Yoruba", "yo"}, {"Chinese (Simplified)", "zh-CN"},
		{"Chinese (Traditional)", "zh-TW"}, {"Zulu", "zu"}
	};

	std::unordered_map<std::string, std::string> deeplLanguages = {
		{"Arabic", "ar"}, {"Bulgarian", "bg"}, {"Czech", "cs"}, {"Danish", "da"},
		{"German", "de"}, {"Greek", "el"}, {"English", "en"}, {"Spanish", "es"},
		{"Estonian", "et"}, {"Finnish", "fi"}, {"French", "fr"}, {"Hungarian", "hu"},
		{"Indonesian", "id"}, {"Italian", "it"}, {"Japanese", "ja"}, {"Korean", "ko"},
		{"Lithuanian", "lt"}, {"Latvian", "lv"}, {"Norwegian", "nb"}, {"Dutch", "nl"},
		{"Polish", "pl"}, {"Portuguese", "pt-br"}, {"Romanian", "ro"}, {"Russian", "ru"},
		{"Slovak", "sk"}, {"Slovenian", "sl"}, {"Swedish", "sv"}, {"Turkish", "tr"},
		{"Ukrainian", "uk"}, {"Chinese (Simplified)", "zh-hans"}, {"Chinese (Traditional)", "zh-hant"}
	};

	static Manager* getSharedInstance() {
		if (!instance) {
			instance = new Manager();
		}
		return instance;
	}

};