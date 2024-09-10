#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <vector>
#include <list>
#include <algorithm>

// Associates strings (keys) with values
template <typename T>
class HashMap
{
public:
    HashMap(double max_load = 0.75) : m_hashTable(10), m_size(0), m_buckets(10), m_maxLoadFactor(max_load)  // constructor
    {
        if (max_load <= 0)
            m_maxLoadFactor = 0.75;
    }
    
    ~HashMap()  // destructor; deletes all of the items in the hashmap
    {
        
    }
    
    int size() const    // return the number of associations in the hashmap
    {
        return m_size;
    }
    
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value)
    {
        T* valuePtr = find(key);
        
        if (valuePtr == nullptr)    // key not found
        {
            if ((m_size + 1) / static_cast<double>(m_buckets) > m_maxLoadFactor)    // rehash
            {
                HashMap<T> newHm(m_buckets * 2, m_maxLoadFactor);
                
                for (const auto& li : m_hashTable)
                    for (const auto& pr : li)
                        newHm.insert(pr.first, pr.second);
                
                swap(*this, newHm); // copy and swap
            }   // newHm, which has the original hash map, is destructed here
            
            m_hashTable[hashString(key) % m_buckets].push_back(std::pair<std::string, T>(key, value));
            m_size++;
        }
        else    // key found
            *valuePtr = value;
    }
    
    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T (0 for builtin types).
    // It returns a reference to the newly created value in the map.
    T& operator[](const std::string& key)
    {
        T* valuePtr = find(key);
        
        if (valuePtr == nullptr)    // key not found
        {
            insert(key, T());
            return *find(key);
        }
        
        return *valuePtr;   // key found
    }
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value within the map.
    const T* find(const std::string& key) const
    {
        for (const auto& pr : m_hashTable[hashString(key) % m_buckets])
            if (pr.first == key)    // key found
                return &(pr.second);
        
        return nullptr; // key not found
    }
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T* find(const std::string& key) {
        const auto& hm = *this;
        return const_cast<T*>(hm.find(key));
    }
private:
    std::vector<std::list<std::pair<std::string, T>>> m_hashTable;
    int m_size;
    int m_buckets;
    double m_maxLoadFactor;
    
    std::size_t hashString(const std::string& key) const
    {
        return std::hash<std::string>()(key);
    }
    
    HashMap(int buckets, double max_load) : m_hashTable(buckets), m_size(0), m_buckets(buckets), m_maxLoadFactor(max_load)
    {
        if (max_load <= 0)
            m_maxLoadFactor = 0.75;
    }
    
    HashMap(const HashMap&) = delete;   // copy constructor
    HashMap& operator=(const HashMap&) = delete;    // assignment operator
    
    void swap(HashMap& hm1, HashMap& hm2)
    {
        std::swap(hm1.m_hashTable, hm2.m_hashTable);
        std::swap(hm1.m_size, hm2.m_size);
        std::swap(hm1.m_buckets, hm2.m_buckets);
        std::swap(hm1.m_maxLoadFactor, hm2.m_maxLoadFactor);
    }
};

#endif // HASHMAP_H
