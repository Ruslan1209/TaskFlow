
// Tag.h
#pragma once
#include <string>
using namespace std;

class Tag {
private:
    string name_;
public:
    Tag() : name_("") {}
    explicit Tag(const string& name) : name_(name) {}

    string name() const { return name_; }
    bool matches(const string& other) const { return name_ == other; }
};
