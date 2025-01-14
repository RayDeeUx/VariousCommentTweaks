#pragma once

class TranslateMenu : public FLAlertLayer {
private:
	std::string urlEncoded = "";
protected:
	bool init(const std::string& text);
	void onOpenModSettings(CCObject*);
	void onCloseTranslateMenu(CCObject*);
	void onBoogleTranslate(CCObject*);
	void onLibreTranslate(CCObject*);
	void onDeepLTranslate(CCObject*);
	static std::string encodeToURL(const std::string&);
public:
	static TranslateMenu* create(const std::string& text);
};