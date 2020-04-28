#ifndef _TOWERRANGE_H
#define _TOWERRANGE_H

class TowerRange {
public:
    TowerRange() : rangeDistance(0) {}
    TowerRange(float rangeDistance): rangeDistance(rangeDistance) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & rangeDistance;
    }

private:
    float rangeDistance;
};

#endif
