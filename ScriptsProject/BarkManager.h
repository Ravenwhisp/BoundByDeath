#pragma once

#include "ScriptAPI.h"

#include <deque>
#include <string>
#include <vector>

class UIText;

enum class BarkPriority
{
	Low = 0,
	Normal,
	High
};

struct BarkLine
{
	std::string speaker;
	std::string text;
	float duration = 3.0f;
	BarkPriority priority = BarkPriority::Normal;
};

class BarkManager : public Script
{
	DECLARE_SCRIPT(BarkManager)

public:
	explicit BarkManager(GameObject* owner);

	void Start() override;
	void Update() override;

	bool playBarks(const std::vector<BarkLine>& barks);

	bool isPlaying() const
	{
		return m_hasCurrentBark;
	}

private:
	void startNextBark();
	void finishCurrentBark();

	void showCurrentBark();
	void clearBarkText();

	void interruptCurrentBark();

	void removeQueuedBarksBelow(
		BarkPriority priority
	);

	void pushBarksToFront(
		const std::vector<BarkLine>& barks
	);

	bool isHigherPriority(
		BarkPriority first,
		BarkPriority second
	) const;

private:
	std::deque<BarkLine> m_barkQueue;

	BarkLine m_currentBark;

	float m_timer = 0.0f;
	bool m_hasCurrentBark = false;

	UIText* m_barkText = nullptr;
};