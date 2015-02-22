#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

struct Particle;

class IParticleAffector
{
    public:
        virtual ~IParticleAffector(){};
        virtual void Affect(float dt,Particle & particleToAffect)=0;
    protected:
    private:
};

#endif // PARTICLEAFFECTOR_H