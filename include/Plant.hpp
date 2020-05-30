#ifndef _PLANT_H
#define _PLANT_H

#include "GameObject.hpp"
#include "Player.hpp"
#include "TowerRange.hpp"
#include "ServerParams.h"
#include <unordered_map>
#include <boost/serialization/assume_abstract.hpp>

class Plant : public GameObject {
public:
    enum class PlantType;
    enum class GrowStage;

    Plant(): GameObject(), range(nullptr), growExpireTime(0), growProgressTime(0), plantType(PlantType::CORN) {}

    Plant(Position* position, Direction* direction, Animation* animation, unsigned int objectId, float boundingBoxRadius,
        TowerRange* range, PlantType plantType, GrowStage growStage) : GameObject(position, direction, animation, objectId, boundingBoxRadius) {
        this->range = range;
        this->growExpireTime = 0;
        this->growProgressTime = 0;
        this->plantType = plantType;
        this->growStage = growStage;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & plantType;
        ar & growStage;
        ar & growExpireTime;
        ar & growProgressTime;
        ar & coolDownExpireTime;
        ar & cooldownTime;
        ar & range;
        ar & currAttackTime;
        ar & attackInterval;
        ar & currSprayTime;
        ar & pesticideSprayTime;
    }

    ~Plant() {
        delete range;
    }

    static Plant* buildPlant(ServerParams& config, PlantType type) {
        Plant* plant = new Plant();
        plant->animation = new Animation(0, 0);
        plant->growStage = GrowStage::SEED;
        plant->currAttackTime = 0.0f;
        plant->plantType = type;
        plant->aliveTime = 0.0f;
        plant->currSprayTime = 0.0f;
        plant->currFertilizeTime = 0.0f;
        switch (type) {
        case PlantType::CORN:
            plant->range = new TowerRange(config.cornAttackRange);
            plant->boundingBoxRadius = config.cornBoundingBoxRadius;
            plant->growExpireTime = config.cornGrowExpireTime;
            plant->attackPower = config.cornAttackPower;
            plant->deltaAttack = config.cornFertilizerDeltaAttack;
            plant->attackInterval = config.cornAttackInterval;
            plant->activeTime = config.cornActiveTime;
            plant->deathTime = config.cornDeathTime;
            plant->pesticideSprayTime = config.cornPesticideSprayTime;
            plant->fertilizerCompleteTime = config.cornFertilizerCompleteTime;
            plant->coolDownExpireTime = config.cornCooldownExpireTime;
            break;
        case PlantType::CACTUS:
            plant->range = new TowerRange(config.cactusAttackRange);
            plant->boundingBoxRadius = config.cactusBoundingBoxRadius;
            plant->growExpireTime = config.cactusGrowExpireTime;
            plant->attackPower = config.cactusAttackPower;
            plant->deltaAttack = config.cactusFertilizerDeltaAttack;
            plant->attackInterval = config.cactusAttackInterval;
            plant->activeTime = config.cactusActiveTime;
            plant->deathTime = config.cactusDeathTime;
            plant->pesticideSprayTime = config.cactusPesticideSprayTime;
            plant->fertilizerCompleteTime = config.cactusFertilizerCompleteTime;
            plant->coolDownExpireTime = config.cactusCooldownExpireTime;
            break;
        case PlantType::PLAYER:
            // TODO: player plant wouldn't use any of this, should we use corn as default
            plant->range = new TowerRange(config.cornAttackRange);
            plant->boundingBoxRadius = config.cornBoundingBoxRadius;
            plant->growExpireTime = config.cornGrowExpireTime;
            plant->attackPower = config.cornAttackPower;
            plant->deltaAttack = config.cornFertilizerDeltaAttack;
            plant->attackInterval = config.cornAttackInterval;
            plant->activeTime = config.cornActiveTime;
            plant->deathTime = config.cornDeathTime;
            plant->pesticideSprayTime = config.cornPesticideSprayTime;
            plant->fertilizerCompleteTime = config.cornFertilizerCompleteTime;
            plant->coolDownExpireTime = config.cornCooldownExpireTime;
            break;
        }
        return plant;
    }

    static enum class PlantType { CORN = 0, CACTUS = 1, PLAYER = 2};
    static enum class GrowStage { SEED = 0, SAPLING = 1, BABY = 2, GROWN = 3 };

    TowerRange* range; // Represents how far plant can reach to attack

    PlantType plantType;
    GrowStage growStage;
    float growExpireTime;     // Time(s) need to grow in current stage
    float growProgressTime;   // Time(s) of current grow stage
    float coolDownExpireTime;   // Time(s) need to wait for cooldown
    float cooldownTime;   // Time(s) for cooldown time between grow interactions (watering, etc.

    int attackPower;
    int deltaAttack;
    float currAttackTime;
    float attackInterval;

    float aliveTime;
    float activeTime; // time until bugs attacking plants
    float deathTime; // time until death, starting from activeTime

    float currSprayTime;
    float pesticideSprayTime;

    float currFertilizeTime;
    float fertilizerCompleteTime;

    Player* playerPlant; // only used for PLAYER plants
};


// ++growStage;
inline Plant::GrowStage& operator++(Plant::GrowStage& growStage) {
    switch (growStage) {
    case Plant::GrowStage::SEED:      return growStage = Plant::GrowStage::SAPLING;
    case Plant::GrowStage::SAPLING:   return growStage = Plant::GrowStage::BABY;
    case Plant::GrowStage::BABY:      return growStage = Plant::GrowStage::GROWN;
    case Plant::GrowStage::GROWN:     return growStage = Plant::GrowStage::GROWN;
    }
}

// growStage++;
inline Plant::GrowStage operator++(Plant::GrowStage& growStage, int) {
    Plant::GrowStage result(growStage);
    ++growStage;
    return result;
}
#endif
