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
#include <cstring>
class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        Iterator(DiskMultiMap* d,BinaryFile::Offset o);
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
               
    private:
        // Your private member declarations will go here
        bool m_valid;
        BinaryFile::Offset offset;
        DiskMultiMap* p;
       
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    //helper function, to be delete
    //void printlist(string key);
    

private:
    // Your private member declarations will go here
    BinaryFile m_file;
    int m_bucket;
    struct DiskNode
    {   DiskNode (){}
        DiskNode(string k, string v, string c,BinaryFile::Offset n)
       {
           strcpy(key,k.c_str());
           strcpy(value,v.c_str());
           strcpy(context,c.c_str());
           next = n;
       }
        char key[121];
        char value[121];
        char context[121];
        BinaryFile::Offset next;
        
    };
    
    struct head
    {
        head(int u,int t,int numbuckets)
        :numinuse(u),numintotal(t),numbucket(numbuckets){}
        int numinuse;
        int numintotal;
        int numbucket;
    };
    struct Bucket
    {   Bucket(){}
        char key[121];
        BinaryFile::Offset next;
        bool used;
    };

    string m_fn;
    BinaryFile::Offset firstfreeNode;
    
    
    BinaryFile::Offset keyhash(string key,int numbucket);
};

#endif // DISKMULTIMAP_H_