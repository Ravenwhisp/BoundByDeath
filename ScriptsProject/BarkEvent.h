#pragma once

#include "GameplayEventAction.h"
#include "BarkManager.h"

#include <string>
#include <vector>

class GameplayEventTrigger;

enum class BarkPlayMode
{
	Sequence = 0,
	RandomOne,
	RandomSequence
};

class BarkEvent : public GameplayEventAction
{
	DECLARE_SCRIPT(BarkEvent)

public:
	explicit BarkEvent(GameObject* owner);

	void Update() override;

	void executeEvent(
		GameplayEventTrigger* trigger
	) override;

	FieldList getExposedFields() const override;

	bool play();

	// Para quien necesite esperar su turno en vez de encolarse detras de otro bloque.
	bool isBarkLineBusy() const;

private:
	BarkManager* findBarkManager() const;

	std::vector<BarkLine>
		buildValidBarks() const;

	std::vector<std::vector<BarkLine>>
		buildValidRandomSequences() const;

	int selectRandomIndex(
		int count,
		int lastIndex
	) const;

	bool passesTriggerChance();

	std::string voiceBankName() const;

	BarkLine buildBarkLine(
		const std::string& speakerText,
		const std::string& durationText,
		const std::string& text,
		int sourceIndex,
		bool& valid
	) const;

private:
	std::vector<std::string> m_barks;

	int m_playMode =
		static_cast<int>(
			BarkPlayMode::Sequence
			);

	int m_priority =
		static_cast<int>(
			BarkPriority::Normal
			);

	float m_defaultDuration = 3.0f;

	// Codigo del bloque al que pertenece esta zona: L1, L2, L3 o SIT. De el y del
	// nombre del GameObject sale el nombre del evento de voz, que es la convencion
	// con la que se generaron los audios.
	std::string m_voiceLevel;

	float m_cooldown = 0.0f;
	float m_triggerChance = 1.0f;

	float m_cooldownRemaining = 0.0f;

	int m_lastRandomIndex = -1;
	int m_lastRandomSequenceIndex = -1;
};