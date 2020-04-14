class TowerRange {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & rangeDistance;
    }
private:
    float rangeDistance;
}