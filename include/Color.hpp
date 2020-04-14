class Color {
public:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & r;
        ar & g;
        ar & b;
    }
private:
    float r, g, b; // RGB values?
};