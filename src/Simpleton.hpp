#pragma once

using namespace geode::prelude;

class Simpleton : public UserListDelegate {

protected:
	static Simpleton* instance;
public:

	static Simpleton* get() {
		if (!instance) instance = new Simpleton();
		return instance;
	}

	void getUserListFinished(cocos2d::CCArray* p0, UserListType p1);
	void getUserListFailed(UserListType p0, GJErrorCode p1);

	// while this is valid destructor code it will never run
	// ty dankmeme01
	/*
	~Simpleton() {
		if (GameLevelManager::get()->m_userListDelegate == this)
			GameLevelManager::get()->m_userListDelegate = nullptr;
	}
	*/

};