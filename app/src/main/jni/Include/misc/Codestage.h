//
//
// 24 February 2024
// Created by @Horoni
//
//

#ifndef HORONI_CODESTAGE_H
#define HORONI_CODESTAGE_H

union intfloat {
    int32_t i;
    float f;
};

union longdouble {
    long l;
    double d;
};

struct EncryptedVector2 {
    intfloat x;
    intfloat y;
};

struct EncryptedVector3 {
    intfloat x;
    intfloat y;
    intfloat z;
};

#define DECLARE_ObscuredInteger(data_type, name)        \
struct Obscured##name {                                 \
    data_type key;                                      \
    data_type xoredValue;                               \
                                                        \
    data_type get() { return xoredValue ^ key; }        \
    void set(data_type val) { xoredValue = val ^ key; } \
};

DECLARE_ObscuredInteger(uint8_t, Byte);
DECLARE_ObscuredInteger(char16_t, Char);
DECLARE_ObscuredInteger(short, Short);
DECLARE_ObscuredInteger(unsigned short, UShort);
DECLARE_ObscuredInteger(int, Bool);
DECLARE_ObscuredInteger(int, Int);
DECLARE_ObscuredInteger(unsigned int, UInt);
DECLARE_ObscuredInteger(long, Long);
DECLARE_ObscuredInteger(unsigned long, ULong);

struct ObscuredFloat {
	int key;
	intfloat xoredValue;
	
	float get() {
	    intfloat IF;
	    IF.i = xoredValue.i ^ key;
	    return IF.f;
	}
	
	void set(float val) {
	    intfloat IF;
	    IF.f = val;
	    
	    intfloat IF2;
	    IF2.i = IF.i ^ key;
	    
	    xoredValue.i = IF2.i;
	}
};

struct ObscuredDouble {
	long key;
	longdouble xoredValue;
	
	double get() {
	    longdouble LD;
	    LD.l = xoredValue.l ^ key;
	    return LD.d;
	}
	
	void set(double val) {
	    longdouble LD;
	    LD.d = val;
	    
	    longdouble LD2;
	    LD2.l = LD.l ^ key;
	    
	    xoredValue.l = LD2.l;
	}
};

struct ObscuredVector2 {
    int key;
    EncryptedVector2 xoredVec;
    
    Vector2 get() {
        EncryptedVector2 EV;
        EV.x.i = xoredVec.x.i ^ key;
        EV.y.i = xoredVec.y.i ^ key;
        
        return Vector2(EV.x.f, EV.y.f);
    }
    
    void set(Vector2 val) {
        EncryptedVector2 EV;
        EV.x.f = val.x;
        EV.y.f = val.y;
        
        xoredVec.x.i = EV.x.i ^ key;
        xoredVec.y.i = EV.y.i ^ key;
    }
};

struct ObscuredVector3 {
    int key;
    EncryptedVector3 xoredVec;
    
    Vector3 get() {
        EncryptedVector3 EV;
        EV.x.i = xoredVec.x.i ^ key;
        EV.y.i = xoredVec.y.i ^ key;
        EV.z.i = xoredVec.z.i ^ key;
        
        return Vector3(EV.x.f, EV.y.f, EV.z.f);
    }
    
    void set(Vector3 val) {
        EncryptedVector3 EV;
        EV.x.f = val.x;
        EV.y.f = val.y;
        EV.z.f = val.z;
        
        xoredVec.x.i = EV.x.i ^ key;
        xoredVec.y.i = EV.y.i ^ key;
        xoredVec.z.i = EV.z.i ^ key;
    }
};

// Doesnt work (mono char is 16bit but i use array 8bit). Will be fixed soon...
struct ObscuredString {
    Mono::String* key;
    Mono::Array<uint8_t>* xoredStr;
    
    std::string get() {
        std::string keyy = key->str();
        std::vector<uint8_t> xStr = xoredStr->ToVector();
        
        std::string decryptedStr{};
        
        for (size_t i = 0; i < keyy.size(); i++)
            decryptedStr.push_back((char)(keyy[i] ^ xStr[i]));
        
        return decryptedStr;
    }
    
    void set(std::string val) {
        std::string keyy = key->str();
        std::string cryptedStr{};
        
        for (size_t i = 0; i < keyy.size(); i++)
            cryptedStr.push_back(keyy[i] ^ val[i]);
        
        key = key->Create(cryptedStr);
    }
};

#endif // HORONI_CODESTAGE_H