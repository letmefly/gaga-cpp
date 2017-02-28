#ifndef __GAGA_PLAYER_H__
#define __GAGA_PLAYER_H__

#include "gaga_util.h"

enum PlayerState
{
	kPlayerNotReady,
	kPlayerReady,
	kPlayerEnterRoomOk,
	kPlayerRunning
};

class Player {
public:
	Player() {}
	~Player() {}
	void Reset();
	void GetReady();

	PROPERTY_DEF(RakNet::RakNetGUID, userGuid);
	PROPERTY_DEF(PlayerState, state);
	PROPERTY_DEF(std::string, userid);
	PROPERTY_DEF(uint32_t, playerid);
	PROPERTY_DEF(std::string, car_name);
	PROPERTY_DEF(int32_t, is_free);
	PROPERTY_DEF(int32_t, is_owned);
	PROPERTY_DEF(int32_t, stage);
	PROPERTY_DEF(std::string, decal);
	PROPERTY_DEF(std::string, decal_color);
	PROPERTY_DEF(std::string, driver_name);
	PROPERTY_DEF(std::string, driver_type);
	PROPERTY_DEF(int32_t, accel);
	PROPERTY_DEF(int32_t, speed);
	PROPERTY_DEF(int32_t, handling);
	PROPERTY_DEF(int32_t, tough);
	PROPERTY_DEF(int32_t, is_gold);
	PROPERTY_DEF(int32_t, min_stage);
	PROPERTY_DEF(int32_t, max_stage);
	PROPERTY_DEF(std::string, paint_color);
protected:

};

#endif
