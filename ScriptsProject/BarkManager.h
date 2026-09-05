#pragma once

#include "ScriptAPI.h"

#include <queue>
#include <string>
#include <vector>

class UIText;

enum class BarkSpeaker
{
	Death = 0,
	Lyriel
};

struct BarkLine
{
	BarkSpeaker speaker = BarkSpeaker::Death;
	std::string text;
	float duration = 3.0f;
};

class BarkManager : public Script
{
	DECLARE_SCRIPT(BarkManager)

public:
	explicit BarkManager(GameObject* owner);

	void Start() override;
	void Update() override;

	void playBarks(const std::vector<BarkLine>& barks);

	bool isPlaying() const { return m_hasCurrentBark; }

private:
	void startNextBark();
	void finishCurrentBark();

	void showCurrentBark();
	void clearBarkText();

	const char* getSpeakerName(BarkSpeaker speaker) const;

private:
	std::queue<BarkLine> m_barkQueue;

	BarkLine m_currentBark;

	float m_timer = 0.0f;
	bool m_hasCurrentBark = false;

	UIText* m_barkText = nullptr;
};