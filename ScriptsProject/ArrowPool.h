#pragma once

#include "ScriptAPI.h"
#include <vector>

class LyrielArrowProjectile;

class ArrowPool : public Script
{
    DECLARE_SCRIPT(ArrowPool)

public:
    explicit ArrowPool(GameObject* owner);

    void Start() override;

    LyrielArrowProjectile* acquireArrow();
    void releaseArrow(LyrielArrowProjectile* arrow);

    ScriptFieldList getExposedFields() const override;

private:
    bool createArrow();
    bool registerExistingArrow(GameObject* arrowObject);

public:
    int m_maxArrows = 5;

    std::string m_arrowPrefab = "";
    std::string m_arrowChildPrefix = "Arrow_";

private:
    std::vector<LyrielArrowProjectile*> m_arrows;

};