#include "gaga_player.h"

void Player::Reset() {
	m_state = kPlayerNotReady;
}

void Player::GetReady() {
	if (m_state == kPlayerNotReady) {
		m_state = kPlayerReady;
	}
}

