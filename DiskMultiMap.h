//
//  DiskMultiMap.hpp
//  pj4
//
//  Created by Binyi Wu on 3/5/16.
//  Copyright © 2016 Binyi Wu. All rights reserved.
//

#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "BinaryFile.h"
#include "MultiMapTuple.h"

class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        // Your private member declarations will go here
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    
    
    BinaryFile::Offset hasher(string key);
private:
    // Your private member declarations will go here
    BinaryFile b;
    int m_bucket;
    struct DiskNode
    {
        DiskNode(string v, string c)
       {
           strcpy(value,v.c_str());
           strcpy(context,c.c_str());
       }
        char* value;
        char* context;
        BinaryFile::Offset next;
        
    };
    
    struct freememory
    {
        freememory(BinaryFile::Offset n, int sizes,int nt)
        :num(n),size(sizes),next(nt){}
        int size;
        BinaryFile::Offset num;
        int next;
    };  //For head, offset represents total bucket in use, size represents # of created slot
    
    struct Bucket
    {
        Bucket(string keyvalue)
        {
            strcpy(key,keyvalue.c_str());
        }
        char* key;
        BinaryFile::Offset next;
        BinaryFile::Offset nextbucket;
        bool used;
    };

    string m_fn;
    string m_free = "free";
};

#endif // DISKMULTIMAP_H_