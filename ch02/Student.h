// The running example for Chapter 2. It holds a dynamically allocated member
// on purpose: that is what forces the five special member functions to exist.
#ifndef CH02_STUDENT_H
#define CH02_STUDENT_H

#include <string>
#include <utility>

class Student {
public:
    Student(std::string name, int age, double gpa);

    ~Student();                                     // 1. destructor
    Student(const Student& other);                  // 2. copy constructor
    Student& operator=(const Student& other);       // 3. copy assignment
    Student(Student&& other) noexcept;              // 4. move constructor
    Student& operator=(Student&& other) noexcept;   // 5. move assignment

    const std::string& get_name() const { return name; }
    int get_age() const { return age; }
    double get_gpa() const;
    void set_gpa(double value);
    bool has_gpa() const { return gpa != nullptr; }

    bool operator>(const Student& other) const;     // compares GPAs
    void display() const;

    // How many Student objects are alive. Only here so the tests can prove
    // that every constructor is matched by a destructor.
    static int alive();

private:
    std::string name;
    int age;
    // owned: this object allocates it and frees it
    double* gpa;
};

#endif
