//
//  FlowRider.h
//  Box2D
//
//  Created by Michael J Laielli on 8/31/14.
//
//

#ifndef Box2D_FlowRider_h
#define Box2D_FlowRider_h

#define START_V 4.5f
#define NUM_EMITTERS 10

#include "../Framework/ParticleEmitter.h"

// Emit particles from a circular region.
class DirectionalEmitter {
public:
    // Initialize a particle emitter.
    DirectionalEmitter() :
    m_particleSystem(NULL), m_callback(NULL), m_speed(0.0f),
    m_emitRate(1.0f), m_emitRemainder(0.0f), m_flags(b2_waterParticle),
    m_group(NULL)
    {
    }
    
    ~DirectionalEmitter()
    {
        SetGroup(NULL);
    }
    
    // Set the center of the emitter.
    void SetPosition(const b2Vec2& origin)
    {
        m_origin = origin;
    }
    
    // Get the center of the emitter.
    const b2Vec2& GetPosition() const
    {
        return m_origin;
    }
    
    // Set the size of the circle which emits particles.
    void SetSize(const b2Vec2& size)
    {
        m_halfSize = size * 0.5f;
    }
    
    // Get the size of the circle which emits particles.
    b2Vec2 GetSize() const
    {
        return m_halfSize * 2.0f;
    }
    
    // Set the starting velocity of emitted particles.
    void SetVelocity(const b2Vec2& velocity)
    {
        m_startingVelocity = velocity;
    }
    
    // Get the starting velocity.
    const b2Vec2& GetVelocity() const
    {
        return m_startingVelocity;
    }
    
    // Set the speed of particles along the direction from the center of
    // the emitter.
    void SetSpeed(const float32 speed)
    {
        m_speed = speed;
    }
    
    // Get the speed of particles along the direction from the center of
    // the emitter.
    float32 GetSpeed() const
    {
        return m_speed;
    }
    
    // Set the flags for created particles.
    void SetParticleFlags(uint32 flags)
    {
        m_flags = flags;
    }
    
    // Get the flags for created particles.
    uint32 GetParticleFlags() const
    {
        return m_flags;
    }
    
    // Set the color of particles.
    void SetColor(const b2ParticleColor& color)
    {
        m_color = color;
    }
    
    // Get the color of particles emitter.
    const b2ParticleColor& GetColor() const
    {
        return m_color;
    }
    
    // Set the emit rate in particles per second.
    void SetEmitRate(const float32 emitRate)
    {
        m_emitRate = emitRate;
    }
    
    // Get the current emit rate.
    float32 GetEmitRate() const
    {
        return m_emitRate;
    }
    
    // Set the particle system this emitter is adding particles to.
    void SetParticleSystem(b2ParticleSystem * const particleSystem)
    {
        m_particleSystem = particleSystem;
    }
    
    // Get the particle system this emitter is adding particle to.
    b2ParticleSystem* GetParticleSystem() const
    {
        return m_particleSystem;
    }
    
    // Set the callback that is called on the creation of each particle.
    void SetCallback(EmittedParticleCallback* const callback)
    {
        m_callback = callback;
    }
    
    // Get the callback that is called on the creation of each particle.
    EmittedParticleCallback* GetCallback() const
    {
        return m_callback;
    }
    
    // This class sets the group flags to b2_particleGroupCanBeEmpty so that
    // it isn't destroyed and clears the b2_particleGroupCanBeEmpty on the
    // group when the emitter no longer references it so that the group
    // can potentially be cleaned up.
    void SetGroup(b2ParticleGroup * const group)
    {
        if (m_group)
        {
            m_group->SetGroupFlags(m_group->GetGroupFlags() &
                                   ~b2_particleGroupCanBeEmpty);
        }
        m_group = group;
        if (m_group)
        {
            m_group->SetGroupFlags(m_group->GetGroupFlags() |
                                   b2_particleGroupCanBeEmpty);
        }
    }
    
    // Get the group particles should be created within.
    b2ParticleGroup* GetGroup() const
    {
        return m_group;
    }
    
    // dt is seconds that have passed, particleIndices is an optional pointer
    // to an array which tracks which particles have been created and
    // particleIndicesCount is the size of the particleIndices array.
    // This function returns the number of particles created during this
    // simulation step.
    int32 Step(const float32 dt, int32* const particleIndices,
               const int32 particleIndicesCount)
    {
        b2Assert(m_particleSystem);
        int32 numberOfParticlesCreated = 0;
        // How many (fractional) particles should we have emitted this frame?
        m_emitRemainder += m_emitRate * dt;
        
        b2ParticleDef pd;
        pd.color = m_color;
        pd.flags = m_flags;
        pd.group = m_group;
        
        // Keep emitting particles on this frame until we only have a
        // fractional particle left.
        while (m_emitRemainder > 1.0f) {
            m_emitRemainder -= 1.0f;
            
            // Randomly pick a position within the emitter's radius.
            //			const float32 angle = Random() * 2.0f * b2_pi;
            
            // Set direction
            //            const float32 angle = 0.5f * b2_pi;
            
            
            // Distance from the center of the circle.
            //			const float32 distance = Random();
            //			b2Vec2 positionOnUnitCircle(sin(angle), cos(angle));
            
            // Initial position.
            pd.position.Set(
                            m_origin.x,
                            m_origin.y);
            // Send it flying
            pd.velocity = m_startingVelocity;
            //			if (m_speed != 0.0f)
            //			{
            //				pd.velocity += positionOnUnitCircle * m_speed;
            //			}
            
            const int32 particleIndex = m_particleSystem->CreateParticle(pd);
            if (m_callback)
            {
                m_callback->ParticleCreated(m_particleSystem, particleIndex);
            }
            if (particleIndices &&
                numberOfParticlesCreated < particleIndicesCount)
            {
                particleIndices[numberOfParticlesCreated] = particleIndex;
            }
            ++numberOfParticlesCreated;
        }
        return numberOfParticlesCreated;
    }
    
private:
    // Calculate a random number 0.0f..1.0f.
    static float32 Random()
    {
        return ((float32)rand() / (float32)RAND_MAX);
    }
    
private:
    // Pointer to global world
    b2ParticleSystem *m_particleSystem;
    // Called for each created particle.
    EmittedParticleCallback *m_callback;
    // Center of particle emitter
    b2Vec2 m_origin;
    // Launch direction.
    b2Vec2 m_startingVelocity;
    // Speed particles are emitted
    float32 m_speed;
    // Half width / height of particle emitter
    b2Vec2 m_halfSize;
    // Particles per second
    float32 m_emitRate;
    // Initial color of particle emitted.
    b2ParticleColor m_color;
    // Number particles to emit on the next frame
    float32 m_emitRemainder;
    // Flags for created particles, see b2ParticleFlag.
    uint32 m_flags;
    // Group to put newly created particles in.
    b2ParticleGroup* m_group;
};


// Faucet test creates a container from boxes and continually spawning
// particles with finite lifetimes that pour into the box.
class FlowRider : public Test
{
private:
	// Assigns a random lifetime to each created particle.
	class ParticleLifetimeWave : public EmittedParticleCallback
	{
	public:
		// Initialize the randomizer to set lifetimes between minLifetime to
		// maxLifetime.
		ParticleLifetimeWave() { }
		virtual ~ParticleLifetimeWave() { }
        
		// Called for each created particle.
		virtual void ParticleCreated(b2ParticleSystem * const system,
									 const int32 particleIndex)
		{
			system->SetParticleLifetime(
                                        particleIndex, (1.8f));
		}
        
	private:
//		float32 m_minLifetime;
//		float32 m_maxLifetime;
	};
    
public:
	// Construct the world.
	FlowRider() :
    m_particleColorOffset(0.0f)
//    m_lifetimeRandomizer(k_particleLifetimeMin, k_particleLifetimeMax)
	{
		// Configure particle system parameters.
		m_particleSystem->SetRadius(0.035f);
		m_particleSystem->SetMaxParticleCount(k_maxParticleCount);
		m_particleSystem->SetDestructionByAge(true);
        
        // Ground body
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);
            
			float32 x1 = 0.0f;
			float32 y1 = -sqrtf(2-x1*x1) + 1.4f;
			for (int32 i = 0; i < 56; ++i)
			{
				float32 x2 = x1 + 0.025f;
				float32 y2 = -(sqrtf(2-x1*x1)) + 1.4f;
                
				b2EdgeShape shape;
				shape.Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
				ground->CreateFixture(&shape, 0.0f);
                
				x1 = x2;
				y1 = y2;
			}
		}
        
        {
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2CircleShape shape;
			shape.m_p.Set(0.5f, 1);
			shape.m_radius = 0.2f;
			body->CreateFixture(&shape, 0.3f);
		}

        
//		b2Body* ground = NULL;
//		{
//			b2BodyDef bd;
//			ground = m_world->CreateBody(&bd);
//		}
//        
//		// Create the container / trough style sink.
//		{
//            b2PolygonShape shape;
//			const float32 height = k_containerHeight + k_containerThickness;
////			shape.SetAsBox(k_containerWidth - k_containerThickness,
////						   k_containerThickness, b2Vec2(0.0f, 0.0f), 0.0f);
////			ground->CreateFixture(&shape, 0.0f);
////			shape.SetAsBox(k_containerThickness, height,
////						   b2Vec2(-k_containerWidth, k_containerHeight), 0.0f);
////			ground->CreateFixture(&shape, 0.0f);
//			shape.SetAsBox(k_containerThickness, height,
//						   b2Vec2(k_containerWidth*2, k_containerHeight), 0.0f);
//			ground->CreateFixture(&shape, 0.0f);
//		}
        
		// Create ground under the container to catch overflow.
//		{
//			b2PolygonShape shape;
//			shape.SetAsBox(k_containerWidth * 5.0f, k_containerThickness,
//						   b2Vec2(0.0f, k_containerThickness * -2.0f), 0.0f);
//			ground->CreateFixture(&shape, 0.0f);
//		}
        
		// Create the faucet spout.
//		{
//			b2PolygonShape shape;
//			const float32 particleDiameter =
//            m_particleSystem->GetRadius() * 2.0f;
//			const float32 faucetLength = k_faucetLength * particleDiameter;
//			// Dimensions of the faucet in world units.
//			const float32 length = faucetLength * k_spoutLength;
//			const float32 width = k_containerWidth * k_faucetWidth *
//            k_spoutWidth;
//			// Height from the bottom of the container.
//			const float32 height = (k_containerHeight * k_faucetHeight) +
//            (length * 0.5f);
//            
//			shape.SetAsBox(particleDiameter, length,
//						   b2Vec2(-width, height), 0.0f);
//			ground->CreateFixture(&shape, 0.0f);
//			shape.SetAsBox(particleDiameter, length,
//						   b2Vec2(width, height), 0.0f);
//			ground->CreateFixture(&shape, 0.0f);
//			shape.SetAsBox(width - particleDiameter, particleDiameter,
//						   b2Vec2(0.0f, height + length -
//								  particleDiameter), 0.0f);
//			ground->CreateFixture(&shape, 0.0f);
//		}
        
		// Initialize the particle emitter.
		{
            float32 spacing = 0.05f;
            float32 xpos = spacing;
			const float32 faucetLength =
            m_particleSystem->GetRadius() * 2.0f * k_faucetLength;
            for(int n=0; n<NUM_EMITTERS; ++n)
            {   xpos = spacing*float32(n+1);
                m_emitter[n].SetParticleSystem(m_particleSystem);
                m_emitter[n].SetCallback(&m_lifetimeRandomizer);
//              m_emitter[n].SetPosition(b2Vec2(k_containerWidth * k_faucetWidth,
//										 k_containerHeight * k_faucetHeight +
//										 (faucetLength * 0.5f)));
                m_emitter[n].SetPosition(b2Vec2(0.0f, xpos));
                m_emitter[n].SetVelocity(b2Vec2(START_V, 0.0f));
                m_emitter[n].SetSize(b2Vec2(0.0f, faucetLength));
                m_emitter[n].SetColor(b2ParticleColor(50, 50, 50, 255));
                m_emitter[n].SetEmitRate(50.0f);
                m_emitter[n].SetParticleFlags(TestMain::GetParticleParameterValue());
            }
		}
        
        
        
		// Don't restart the test when changing particle types.
		TestMain::SetRestartOnParticleParameterChange(false);
		// Limit the set of particle types.
		TestMain::SetParticleParameters(k_paramDef, k_paramDefCount);
	}
    
	// Run a simulation step.
	void Step(Settings* settings)
	{
		const float32 dt = 1.0f / settings->hz;
		Test::Step(settings);
        
//        const b2ParticleContact* contacts = m_particleSystem->GetContacts();
//        const int32 num_contacts = m_particleSystem->GetContactCount();
//        b2ParticleColor* colors = m_particleSystem->GetColorBuffer();
////        b2Vec2* velocities = m_particleSystem->GetVelocityBuffer();
//        
//        for( int i=0; i<num_contacts; ++i)
//        {
//            float32 w = contacts[i].GetWeight();
//            
//            //calc impact
//            
//            colors[contacts[i].GetIndexA()] *= 1.0f+w ;
//            colors[contacts[i].GetIndexB()] *= 1.0f+w ;
//        }
        
		m_particleColorOffset += dt;
		// Keep m_particleColorOffset in the range 0.0f..k_ParticleColorsCount.
		if (m_particleColorOffset >= (float32)k_ParticleColorsCount)
		{
			m_particleColorOffset -= (float32)k_ParticleColorsCount;
		}
        
		// Propagate the currently selected particle flags.
        for(int n=0; n<NUM_EMITTERS; ++n)
        {
            m_emitter[n].SetParticleFlags(TestMain::GetParticleParameterValue());
        }
        
		// If this is a color mixing particle, add some color.
		b2ParticleColor color(255, 255, 255, 255);
		if (m_emitter[0].GetParticleFlags() & b2_colorMixingParticle)
		{
			// Each second, select a different color.
			m_emitter[0].SetColor(k_ParticleColors[(int32)m_particleColorOffset %
												k_ParticleColorsCount]);
		}
		else
		{
            for(int n=0; n<NUM_EMITTERS; ++n)
            {
                m_emitter[n].SetColor(b2ParticleColor(0, 50, 255, 255));
            }
            
		}
        
		// Create the particles.
        for(int nEm=0; nEm<NUM_EMITTERS; ++nEm)
        {
            m_emitter[nEm].Step(dt, NULL, 0);
        }
        
		static const char* k_keys[] = {
			"Keys: (w) water, (q) powder",
			"      (t) tensile, (v) viscous",
			"      (c) color mixing, (s) static pressure",
			"      (+) increase flow, (-) decrease flow",
		};
		for (uint32 i = 0; i < B2_ARRAY_SIZE(k_keys); ++i)
		{
			m_debugDraw.DrawString(5, m_textLine, k_keys[i]);
			m_textLine += DRAW_STRING_NEW_LINE;
		}
	}
    
	// Allows you to set particle flags on devices with keyboards
	void Keyboard(unsigned char key)
	{
		uint32 parameter = 0;
		switch (key)
		{
            case 'w':
                parameter = b2_waterParticle;
                break;
            case 'q':
                parameter = b2_powderParticle;
                break;
            case 't':
                parameter = b2_tensileParticle;
                break;
            case 'v':
                parameter = b2_viscousParticle;
                break;
            case 'c':
                parameter = b2_colorMixingParticle;
                break;
            case 's':
                parameter = b2_staticPressureParticle;
                break;
            case '+':
            {
                float32 emitRate = m_emitter[0].GetEmitRate();
                emitRate *= k_emitRateChangeFactor;
                emitRate = b2Max(emitRate, k_emitRateMin);
                m_emitter[0].SetEmitRate(emitRate);
                break;
            }
            case '-':
            {
                float32 emitRate = m_emitter[0].GetEmitRate();
                emitRate *= 1.0f / k_emitRateChangeFactor;
                emitRate = b2Min(emitRate, k_emitRateMax);
                m_emitter[0].SetEmitRate(emitRate);
            }
                break;
            default:
                // Nothing.
                return;
		}
		TestMain::SetParticleParameterValue(parameter);
	}
    
	float32 GetDefaultViewZoom() const
	{
		return 0.1f;
	}
    
	// Create the faucet test.
	static Test* Create()
	{
		return new FlowRider;
	}
    
private:
	// Used to cycle through particle colors.
	float32 m_particleColorOffset;
	// Particle emitters.
	DirectionalEmitter m_emitter[NUM_EMITTERS];
	// Callback which sets the lifetime of emitted particles.
	ParticleLifetimeWave m_lifetimeRandomizer;
    
private:
	// Minimum lifetime of particles in seconds.
	static const float32 k_particleLifetimeMin;
	// Maximum lifetime of particles in seconds.
	static const float32 k_particleLifetimeMax;
	// Height of the container.
	static const float32 k_containerHeight;
	// Width of the container.
	static const float32 k_containerWidth;
	// Thickness of the container's walls and bottom.
	static const float32 k_containerThickness;
	// Width of the faucet relative to the container width.
	static const float32 k_faucetWidth;
	// Height of the faucet relative to the base as a fraction of the
	// container height.
	static const float32 k_faucetHeight;
	// Length of the faucet as a fraction of the particle diameter.
	static const float32 k_faucetLength;
	// Spout height as a fraction of the faucet length.  This should be
	// greater than 1.0f).
	static const float32 k_spoutLength;
	// Spout width as a fraction of the *faucet* width.  This should be greater
	// than 1.0f).
	static const float32 k_spoutWidth;
	// Maximum number of particles in the system.
	static const int32 k_maxParticleCount;
	// Factor that is used to increase / decrease the emit rate.
	// This should be greater than 1.0f.
	static const float32 k_emitRateChangeFactor;
	// Minimum emit rate of the faucet in particles per second.
	static const float32 k_emitRateMin;
	// Maximum emit rate of the faucet in particles per second.
	static const float32 k_emitRateMax;
    
	// Selection of particle types for this test.
	static const ParticleParameter::Value k_paramValues[];
	static const ParticleParameter::Definition k_paramDef[];
	static const uint32 k_paramDefCount;
    
    void changeColorsAccordingToContactWeight( const b2ParticleContact* tacts )
    {
        
    }
};

const float32 FlowRider::k_particleLifetimeMin = 30.0f;
const float32 FlowRider::k_particleLifetimeMax = 50.0f;
const float32 FlowRider::k_containerHeight = 0.2f;
const float32 FlowRider::k_containerWidth = 1.0f;
const float32 FlowRider::k_containerThickness = 0.05f;
const float32 FlowRider::k_faucetWidth = 0.1f;
const float32 FlowRider::k_faucetHeight = 15.0f;
const float32 FlowRider::k_faucetLength = 2.0f;
const float32 FlowRider::k_spoutWidth = 1.1f;
const float32 FlowRider::k_spoutLength = 2.0f;
const int32 FlowRider::k_maxParticleCount = 1000;
const float32 FlowRider::k_emitRateChangeFactor = 1.05f;
const float32 FlowRider::k_emitRateMin = 1.0f;
const float32 FlowRider::k_emitRateMax = 240.0f;

const ParticleParameter::Value FlowRider::k_paramValues[] =
{
	{ b2_waterParticle, ParticleParameter::k_DefaultOptions, "water" },
	{ b2_waterParticle, ParticleParameter::k_DefaultOptions |
        ParticleParameter::OptionStrictContacts, "water (strict)" },
	{ b2_viscousParticle, ParticleParameter::k_DefaultOptions, "viscous" },
	{ b2_powderParticle, ParticleParameter::k_DefaultOptions, "powder" },
	{ b2_tensileParticle, ParticleParameter::k_DefaultOptions, "tensile" },
	{ b2_colorMixingParticle, ParticleParameter::k_DefaultOptions,
		"color mixing" },
	{ b2_staticPressureParticle, ParticleParameter::k_DefaultOptions,
		"static pressure" },
};

const ParticleParameter::Definition FlowRider::k_paramDef[] =
{
	{ FlowRider::k_paramValues, B2_ARRAY_SIZE(FlowRider::k_paramValues) },
};

const uint32 FlowRider::k_paramDefCount = B2_ARRAY_SIZE(FlowRider::k_paramDef);


#endif
