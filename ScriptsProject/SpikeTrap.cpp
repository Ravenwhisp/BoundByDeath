#include "pch.h"
#include "SpikeTrap.h"

static const ScriptFieldInfo myScriptFields[] =
{
    { "Normal Spikes", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_normalSpike), {}, {}, { ComponentType::TRANSFORM } },
    { "Spectral Spikes", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_spectralSpike), {}, {}, { ComponentType::TRANSFORM } },
	{ "Starting Mode", ScriptFieldType::Int, offsetof(SpikeTrap, startingMode) },
    { "Active Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, a_duration), {0.0f, 50.0f, 0.1f } },
    { "Wait Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, w_duration), {0.0f, 50.0f, 0.1f } },
	{ "Preparing Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, p_duration), {0.0f, 50.0f, 0.1f } },
    { "First Target", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_firstTarget), {}, {}, { ComponentType::TRANSFORM } },
    { "Second Target", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_secondTarget), {}, {}, { ComponentType::TRANSFORM } }
};

IMPLEMENT_SCRIPT_FIELDS(SpikeTrap, myScriptFields)

SpikeTrap::SpikeTrap(GameObject* owner)
    : Script(owner)
{
}

void SpikeTrap::Start()
{
}

void SpikeTrap::Update()
{
    currentTime += dt;
    switch (state)
    {
        case SpikeTrap::WAIT:
            
            if (startingMode == 0)
            {
				normalSpikePosition.y = -0.4f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
            }
            else if (startingMode == 1)
            {
				spectralSpikePosition.y = -0.4f;
				TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
            }

            if (currentTime >= w_duration && startingMode == 0)
            {
                normalSpikePosition.y = 0.0f;
				TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                state = F_ACTIVE;
                currentTime = 0.0f;

            }
			else if(currentTime >= w_duration && startingMode == 1)
            {
				spectralSpikePosition.y = 0.0f;
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = F_ACTIVE;
				currentTime = 0.0f;
            }
            break;

        case SpikeTrap::F_ACTIVE:
            if (currentTime >= a_duration && startingMode == 0)
            {
				normalSpikePosition.y = -0.5f;
				spectralSpikePosition.y = -0.4f;
				TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = PREPARING;
                currentTime = 0.0f;
            }
			else if (currentTime >= a_duration && startingMode == 1)
			{
				normalSpikePosition.y = -0.4f;
				spectralSpikePosition.y = -0.5f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
				state = PREPARING;
				currentTime = 0.0f;
            }
            break;
        case SpikeTrap::PREPARING:
            if (currentTime >= p_duration && startingMode == 0)
            {
                normalSpikePosition.y = -0.5f;
                spectralSpikePosition.y = 0.0f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = S_ACTIVE;
                currentTime = 0.0f;
            }
            else if (currentTime >= p_duration && startingMode == 1)
            {
                normalSpikePosition.y = 0.0f;
                spectralSpikePosition.y = -0.5f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = S_ACTIVE;
                currentTime = 0.0f;
            }

        case SpikeTrap::S_ACTIVE:
            if (currentTime >= a_duration)
            {
				normalSpikePosition.y = -0.6f;
				spectralSpikePosition.y = -0.6f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = WAIT;
				currentTime = 0.0f;
            }

        default:
            break;
    }

     /*   const Vector3 triggerCenter = TransformAPI::getPosition(transform);

        Transform* firstTarget = m_firstTarget.getReferencedComponent();
        if (firstTarget && containsPoint(triggerCenter, TransformAPI::getPosition(firstTarget)))
        {
            return;
        }

        Transform* secondTarget = m_secondTarget.getReferencedComponent();
        if (secondTarget && containsPoint(triggerCenter, TransformAPI::getPosition(secondTarget)))
        {
        }*/

    }

bool SpikeTrap::containsPoint(const Vector3& triggerCenter, const Vector3& point) const
{
    const float halfX = m_xWidth * 0.5f;
    const float halfZ = m_zWidth * 0.5f;

    return point.x >= triggerCenter.x - halfX &&
        point.x <= triggerCenter.x + halfX &&
        point.z >= triggerCenter.z - halfZ &&
        point.z <= triggerCenter.z + halfZ;
}

void SpikeTrap::TrapLoop()
{
    
        
}


IMPLEMENT_SCRIPT(SpikeTrap)