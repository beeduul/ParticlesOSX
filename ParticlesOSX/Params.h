//
//  Params.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Params__
#define __ParticlesOSX__Params__

#include <map>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>

#include <Color.h>

using namespace std;

class Params; // forward declaration for shared_ptr
typedef boost::shared_ptr<Params> ParamsPtr;

class Params {
    
    std::map<std::string, boost::variant<int, float, std::string, bool, Color>> p;
    
public:
    
    static ParamsPtr& get();
    
    float getf(std::string key) {
        return boost::get<float>(p[key]);
    }
    
    void setf(std::string key, float value) {
        p[key] = value;
    }
    
    float geti(std::string key) {
        return boost::get<int>(p[key]);
    }
    
    void seti(std::string key, int value) {
        p[key] = value;
    }
    
    bool getb(std::string key) {
        return boost::get<bool>(p[key]);;
    }
    
    void setb(std::string key, bool value) {
        p[key] = value;
        
    }
    
    Color getColor(std::string key) {
        return boost::get<Color>(p[key]);
    }
    
    void setColor(std::string key, Color value) {
        p[key] = value;
    }
    
    Params();
};

#endif /* defined(__ParticlesOSX__Params__) */
