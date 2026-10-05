// A collection of students that can be walked with a range-based for loop and
// sorted with std::sort, without letting anybody reach the vector inside it.
#ifndef CH02_STUDENTLIST_H
#define CH02_STUDENTLIST_H

#include "Student.h"

#include <vector>

class StudentList {
public:
    void add_student(const Student& s) {
        students.push_back(s);
    }

    // the vector's own iterators, under this class's names
    using iterator = std::vector<Student>::iterator;
    using const_iterator =
        std::vector<Student>::const_iterator;

    iterator begin() { return students.begin(); }
    iterator end() { return students.end(); }
    const_iterator begin() const { return students.begin(); }
    const_iterator end() const { return students.end(); }

private:
    std::vector<Student> students;
};

#endif
