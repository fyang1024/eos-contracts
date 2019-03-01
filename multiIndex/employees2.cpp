#include <iostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>

using namespace ::boost;
using namespace ::boost::multi_index;


struct employee {
    int id;
    std::string name;

    employee(int id, const std::string& name): id(id), name(name){}
    bool operator<(const employee& e) const {return id < e.id;}
};

typedef multi_index_container<
    employee,
    indexed_by<
        ordered_unique<identity<employee>>,
        ordered_non_unique<member<employee, std::string, &employee::name>>
    >
> employee_set;

void print_by_name(const employee_set& es) {
    const employee_set::nth_index<1>::type& name_index = es.get<1>();
    for(auto l : name_index) {
        std::cout << l.name << ' ' << l.id << std::endl;
    }
}

void print_by_id(const employee_set& es) {
    const employee_set::nth_index<0>::type& id_index = es.get<0>();
    for(auto l : id_index) {
        std::cout << l.name << ' ' << l.id << std::endl;
    }
}

int main() {
    employee_set employees;

    employees.insert({1, "bob"});
    employees.insert({2, "susan"});
    employees.insert({1, "bob"});
    employees.insert({2, "bob"});
    employees.insert({4, "bob"});
    employees.insert({1, "alice"});
    employees.insert({3, "alice"});

    print_by_name(employees);
    print_by_id(employees);
}
