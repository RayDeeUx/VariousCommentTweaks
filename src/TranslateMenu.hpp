#pragma once

class TranslateMenu : public FLAlertLayer {
protected:
	bool init(const std::string& text);
	void onOpenModSettings(CCObject*);
	void onCloseTranslateMenu(CCObject*);
public:
	static TranslateMenu* create(const std::string&);
	static std::pair<std::string, std::string> findLanguageCodes(const std::unordered_map<std::string, std::string>&, const bool);
	static void encodeToURL(const std::string&);
	void onBoogleTranslate(CCObject*);
	void onLibreTranslate(CCObject*);
	void onDeepLTranslate(CCObject*);
};