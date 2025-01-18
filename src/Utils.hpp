#pragma once
#include "Simpleton.hpp"
#include "Manager.hpp"

using namespace geode::prelude;

namespace Utils {
	template<class T> T getSetting(const std::string& setting);
	bool getBool(const std::string& setting);
	int64_t getInt(const std::string& setting);
	double getDouble(const std::string& setting);
	std::string getString(const std::string& setting);
	ccColor3B getColor(const std::string& setting);
	ccColor4B getColorAlpha(const std::string& setting);
	bool modEnabled();
	
	bool isModLoaded(const std::string& modID);
	Mod* getMod(const std::string& modID);
	std::string getModVersion(Mod* mod);

	template<typename T> bool contains(std::span<T> const& span, T const& value) {
		return std::find(span.begin(), span.end(), value) != span.end();
	}

	void writeToFile(const std::string_view fileName, int accountID, const std::string_view username);
	bool addIgnoredUser(int accountID, std::string username);
	bool addFavoriteUser(int accountID, std::string username);
	bool updateLists(Manager* manager = Manager::getSharedInstance(), Simpleton* simpleton = Simpleton::get());
}