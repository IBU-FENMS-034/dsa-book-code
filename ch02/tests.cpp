// Every listing in Chapter 2 compiles and passes these checks.
//   c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp Student.cpp && ./tests
#include "Student.h"
#include "StudentList.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) { std::printf("  FAIL  %s\n", what); ++failures; }
}

// --- pointers and references, the two ways to refer to something ----------
static void by_value(int n)      { n = 99; (void) n; }   // discarded
static void by_pointer(int* n)   { *n = 99; }
static void by_reference(int& n) { n = 99; }

int main() {
    // ---- a pointer holds an address; a reference is another name ---------
    int a = 10;
    int* p = &a;    // p holds a's address
    int& r = a;     // r IS a, under another name
    check(*p == 10, "dereferencing the pointer gives the value");
    check(r == 10,  "the reference reads the same variable");
    check(p == &r,  "a reference has the same address as its target");
    *p = 20;        // through the pointer: a becomes 20
    check(a == 20 && r == 20, "writing through a pointer changes the original");
    r = 30;         // through the reference: a becomes 30
    check(a == 30 && *p == 30, "writing through a reference does too");

    int b = 7;
    p = &b;         // a pointer can be re-pointed
                    // r = ... assigns to a, it cannot rebind r
    check(*p == 7 && a == 30, "re-pointing leaves the old target alone");

    // ---- what the three parameter styles actually do --------------------
    int v = 1; by_value(v);     check(v == 1,  "by value copies, caller unchanged");
    v = 1;     by_pointer(&v);  check(v == 99, "by pointer writes through");
    v = 1;     by_reference(v); check(v == 99, "by reference writes through");

    // ---- the five special members ---------------------------------------
    const int before = Student::alive();
    {
        Student amina("Amina", 20, 3.8);
        check(amina.get_gpa() == 3.8 && amina.get_age() == 20, "constructed");

        Student copy = amina;                 // copy constructor
        copy.set_gpa(2.1);
        check(amina.get_gpa() == 3.8, "a copy is independent of its source");
        check(copy.get_gpa() == 2.1,  "and can be changed on its own");

        Student assigned("Placeholder", 0, 0.0);
        assigned = amina;                     // copy assignment
        check(assigned.get_gpa() == 3.8 && assigned.get_age() == 20,
              "copy assignment deep-copies");
        Student& alias = assigned;            // self-assignment must be safe
        assigned = alias;                     // (via an alias, or the compiler warns)
        check(assigned.get_gpa() == 3.8, "self-assignment is harmless");

        Student moved = std::move(copy);      // move constructor
        check(moved.get_gpa() == 2.1, "the moved-to object has the value");
        check(!copy.has_gpa(),        "the moved-from object gave it up");

        // Moving into a vector is where this pays off in practice.
        std::vector<Student> cohort;
        cohort.reserve(2);
        cohort.push_back(Student("Dino", 22, 3.1));
        cohort.push_back(std::move(moved));
        check(cohort.size() == 2,           "both students stored");
        check(cohort[0].get_gpa() == 3.1,   "first survived the move");
        check(cohort[1].get_gpa() == 2.1,   "second survived the move");
    }
    check(Student::alive() == before,
          "every constructor was matched by a destructor: nothing leaked");

    // ---- operator overloading and iterators -----------------------------
    {
        Student lejla("Lejla", 21, 3.9);
        Student tarik("Tarik", 23, 3.5);
        check(lejla > tarik && !(tarik > lejla), "> compares GPAs");

        // display() writes to std::cout, so catch what it writes.
        std::ostringstream out;
        std::streambuf* old = std::cout.rdbuf(out.rdbuf());
        const Student& shown = lejla;             // display() must be const
        shown.display();
        std::cout.rdbuf(old);
        check(out.str() == "Student: Lejla, Age: 21, GPA: 3.9\n",
              "display() prints name, age and GPA");

        StudentList list;
        list.add_student(tarik);
        list.add_student(Student("Amina", 20, 3.8));
        list.add_student(lejla);

        std::sort(list.begin(), list.end(),
                  [](const Student& a, const Student& b) {
                      return a > b;
                  });

        std::vector<std::string> order;
        const StudentList& view = list;           // the const begin/end
        for (const Student& s : view) order.push_back(s.get_name());
        check(order == std::vector<std::string>{"Lejla", "Amina", "Tarik"},
              "std::sort with > puts the highest GPA first");
        check(tarik.get_gpa() == 3.5, "the list holds copies, not the originals");
    }
    check(Student::alive() == before, "sorting leaked nothing either");

    std::printf(failures ? "%d check(s) failed\n" : "all checks pass\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
