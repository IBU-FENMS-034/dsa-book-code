#include "Student.h"

#include <iostream>
#include <stdexcept>

namespace { int live_count = 0; }

int Student::alive() { return live_count; }

Student::Student(std::string name, const int age,
                 const double gpa)
    : name(std::move(name)), age(age), gpa(new double(gpa)) {
    ++live_count;
}

// 1. Destructor. Frees what this object owns. Without it, every Student
//    leaks one double.
Student::~Student() {
    delete gpa;
    --live_count;
}

// 2. Copy constructor. The default would copy the *pointer*, leaving two
//    objects owning one double, and both would free it.
Student::Student(const Student& other)
    : name(other.name), age(other.age),
      gpa(other.gpa ? new double(*other.gpa) : nullptr) {
    ++live_count;
}

// 3. Copy assignment. Guard against self-assignment, or we free our own
//    double and then read it.
Student& Student::operator=(const Student& other) {
    if (this == &other) return *this;
    double* replacement =
        other.gpa ? new double(*other.gpa) : nullptr;
    // only after the new one is safely allocated
    delete gpa;
    name = other.name;
    age = other.age;
    gpa = replacement;
    return *this;
}

// 4. Move constructor. Steal the pointer and leave the source empty but
//    destructible. noexcept so the standard containers will use it.
Student::Student(Student&& other) noexcept
    : name(std::move(other.name)), age(other.age),
      gpa(other.gpa) {
    other.gpa = nullptr;  // it must still be safe to destroy
    ++live_count;
}

// 5. Move assignment.
Student& Student::operator=(Student&& other) noexcept {
    if (this == &other) return *this;
    delete gpa;
    name = std::move(other.name);
    age = other.age;
    gpa = other.gpa;
    other.gpa = nullptr;
    return *this;
}

double Student::get_gpa() const {
    if (!gpa) throw std::logic_error("this Student has been moved from");
    return *gpa;
}

void Student::set_gpa(const double value) {
    if (!gpa) gpa = new double(value); else *gpa = value;
}

bool Student::operator>(const Student& other) const {
    return get_gpa() > other.get_gpa();
}

void Student::display() const {
    std::cout << "Student: " << name << ", Age: " << age
              << ", GPA: " << get_gpa() << "\n";
}
