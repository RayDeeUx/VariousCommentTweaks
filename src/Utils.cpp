#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::cocos;

namespace Utils {
	template<class T> T getSetting(const std::string& setting) { return Mod::get()->getSettingValue<T>(setting); }

	bool getBool(const std::string& setting) { return getSetting<bool>(setting); }
	
	int64_t getInt(const std::string& setting) { return getSetting<int64_t>(setting); }
	
	double getDouble(const std::string& setting) { return getSetting<double>(setting); }

	std::string getString(const std::string& setting) { return getSetting<std::string>(setting); }

	ccColor3B getColor(const std::string& setting) { return getSetting<ccColor3B>(setting); }

	ccColor4B getColorAlpha(const std::string& setting) { return getSetting<ccColor4B>(setting); }

	bool modEnabled() { return getBool("enabled"); }
	
	bool isModLoaded(const std::string& modID) { return Loader::get()->isModLoaded(modID); }

	Mod* getMod(const std::string& modID) { return Loader::get()->getLoadedMod(modID); }

	std::string getModVersion(Mod* mod) { return mod->getVersion().toNonVString(); }

	void addIgnoredUser(int accountID, std::string username) {
		log::info("ignoring user: {} (username: {})", accountID, username);
		Manager::getSharedInstance()->ignoredUsers.insert(accountID);
		std::ofstream output;
		output.open((Mod::get()->getConfigDir() / "ignoredUsers.txt"), std::ios_base::app);
		output << std::endl << fmt::format("{} # [VCT] Username: {} [VCT] #", accountID, username);
		output.close();
	}
}