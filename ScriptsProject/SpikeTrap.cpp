#include "pch.h"
#include "SpikeTrap.h"

static const ScriptFieldInfo myScriptFields[] =
{
    { "Normal Spikes", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_normalSpike), {}, {}, { ComponentType::TRANSFORM } },
    { "Spectral Spikes", ScriptFieldType::ComponentRef, offsetof(SpikeTrap, m_spectralSpike), {}, {}, { ComponentType::TRANSFORM } },
	{ "Alternative Mode", ScriptFieldType::Bool, offsetof(SpikeTrap, alternativeMode) },
    { "Active Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, a_duration), {0.0f, 50.0f, 0.1f } },
	{ "Preparing Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, p_duration), {0.0f, 50.0f, 0.1f } }
};

IMPLEMENT_SCRIPT_FIELDS(SpikeTrap, myScriptFields)

SpikeTrap::SpikeTrap(GameObject* owner)
    : Script(owner)
{
}

void SpikeTrap::Start()
{

    if (alternativeMode)
    {
		spikeType = 1;
    }

}

void SpikeTrap::Update()
{
    currentTime += dt;
    switch (state)
    {
        case SpikeTrap::WAIT:
            
            if (spikeType == 0)
            {
				normalSpikePosition.y = -0.4f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
            }
            else if (spikeType == 1)
            {
				spectralSpikePosition.y = -0.4f;
				TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
            }

            if (currentTime >= p_duration && spikeType == 0)
            {
                normalSpikePosition.y = 0.0f;
				TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                state = ACTIVE;
                currentTime = 0.0f;

            }
			else if(currentTime >= p_duration && spikeType == 1)
            {
				spectralSpikePosition.y = 0.0f;
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
                state = ACTIVE;
				currentTime = 0.0f;
            }
            break;

        case SpikeTrap::ACTIVE:
            if (currentTime >= a_duration && spikeType == 0)
            {
				normalSpikePosition.y = -0.5f;
				spectralSpikePosition.y = -0.4f;
				TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
				spikeType = 1;
                state = WAIT;
                currentTime = 0.0f;
            }
			else if (currentTime >= a_duration && spikeType == 1)
			{
				normalSpikePosition.y = -0.4f;
				spectralSpikePosition.y = -0.5f;
                TransformAPI::setPosition(m_normalSpike.getReferencedComponent(), normalSpikePosition);
                TransformAPI::setPosition(m_spectralSpike.getReferencedComponent(), spectralSpikePosition);
				spikeType = 0;
				state = WAIT;
				currentTime = 0.0f;
            }
            break;

        default:
            break;
    }

        /*const Vector3 triggerCenter = TransformAPI::getPosition(ownerTransform);

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