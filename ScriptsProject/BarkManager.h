#pragma once

#include "ScriptAPI.h"

#include <deque>
#include <string>
#include <vector>

class UIText;
class ComponentSoundSource;

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

	// Evento de Wwise y bank donde vive. Vacio = linea sin voz, solo texto.
	std::string voiceEvent;
	std::string voiceBank;
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

	// Hablando, esperando el respiro entre lineas, o con algo encolado. Un bloque de
	// conversacion solo puede empezar si esto es falso, o se intercalaria con otro.
	bool isBusy() const
	{
		return m_hasCurrentBark
			|| m_gapTimer > 0.0f
			|| !m_barkQueue.empty();
	}

	FieldList getExposedFields() const override;

	Vector3 m_lyrielColor{ 0.510f, 0.753f, 0.686f };
	Vector3 m_deathColor{ 0.678f, 0.482f, 0.643f };
	Vector3 m_arthurColor{ 1.000f, 0.550f, 0.000f };
	Vector3 m_aelorinColor{ 0.000f, 0.550f, 1.000f };
	Vector3 m_unnamedColor{ 1.000f, 1.000f, 1.000f };

	bool m_outlineEnabled = true;
	float m_outlineSize = 0.4f;

	bool m_shadowEnabled = true;
	float m_shadowOffset = 2.0f;

	// Respiro entre linea y linea. Sin el, las frases salen pegadas y se solapa el
	// final de una voz con el principio de la siguiente.
	float m_gapBetweenLines = 0.35f;

	// Puerta global para las reactivas. Cada canal tiene su propio cooldown, pero
	// entre nueve canales se solapan y los personajes no paran de hablar peleando.
	float m_reactionCooldown = 8.0f;

	// Margen al entrar en la escena. El primer bark se posteaba con el bank aun
	// cargando y el emisor recien registrado, y se perdia el arranque de la voz.
	float m_startupGrace = 0.6f;

private:
	void startNextBark();
	void finishCurrentBark();

	void showCurrentBark();
	void clearBarkText();

	void applyTextStyle();

	void playVoice();
	void stopVoice();

	Vector3 colorForSpeaker(
		const std::string& speaker
	) const;

	void removeQueuedReactions();

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
	float m_gapTimer = 0.0f;
	float m_reactionTimer = 0.0f;
	float m_startupTimer = 0.0f;
	bool m_hasCurrentBark = false;

	UIText* m_barkText = nullptr;

	ComponentSoundSource* m_soundSource = nullptr;
	uint32_t m_voicePlayingId = 0;
};