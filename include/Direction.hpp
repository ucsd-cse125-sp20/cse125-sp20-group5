class Direction {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & angle;
    }
private:
    float angle; // The angle that the plant is facing??
};