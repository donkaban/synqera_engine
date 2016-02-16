#ifndef _THE_ANIMATON_CURVE_H_
#define _THE_ANIMATON_CURVE_H_

#include "TheEngine/common.h"
#include "TheEngine/math/all.h"
#include "TheEngine/logger.h"


namespace animation {

template <typename T> using curve_atom = std::pair<float, T>;
template <typename T> class curve
{
private:
    std::vector<curve_atom<T>> items;

public:

   curve(const std::vector<float> &k, const std::vector<T> &v)
    {
        if(k.size() != v.size())
            FATAL_ERROR("[Curve] The number of key different from the number of values");
        for(uint i = 0; i < k.size(); i++)
            items.push_back(curve_atom<T>(k[i], v[i]));
    }
    curve()
    {}

    bool getValue(float time, T &start, T &end, float &factor)
    {

        for(auto i = 0u; i < items.size() - 1 ; i++)
        {
            auto & curr = items[i];
            auto & next = items[i+1];
            if(the::math::between(time,curr.first,next.first))
            {
                start  = curr.second;
                end    = next.second;
                auto keyStart = curr.first;
                auto keyEnd   = next.first;
                auto dt = keyEnd - keyStart;
                factor = (time - keyStart) / dt;
                return true;
            }
        }
        start = items.back().second;
        end   = items.back().second;
        factor    = 0;
        return false;
    }


    void serialize(std::ofstream &stream)
    {
        int size = items.size();
        stream.write(reinterpret_cast<char *>(&size), sizeof(int));
        stream.write(reinterpret_cast<char *>(&items[0]), sizeof(curve_atom<T>)*size);
    }

    void deserialize(the::filesystem::stream & stream)
    {
        items.clear();
        int size;
        stream.read(reinterpret_cast<char *>(&size), sizeof(int));
        items.resize(size);
        stream.read(reinterpret_cast<char *>(&items[0]), sizeof(curve_atom<T>)*size);
    }
};
}

#endif 