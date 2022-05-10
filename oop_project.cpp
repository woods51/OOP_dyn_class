#include <iostream>
#include <initializer_list>
#include <map>
#include <functional>
#include <any>

using namespace std;

class Dyn_class
{
public:
    // Default constructor
    Dyn_class() {};

    // Constructor for single function pointer
    template <typename T>
    Dyn_class(T* arg);

    // Variadic Constuctor for initial list of functions
    template <typename T, typename... Args>
    Dyn_class(T* arg, Args&&... args);

    // Add function to map
    template <typename T>
    void add_func(T* a_func);

    // Remove function from map
    template <typename T>
    void remove_func(T* a_func);

    // Call functions in map
    template <typename T>
    T* operator()(T* a_key);

private:
    // Helper function to unpack parameters
    template<typename T, typename... Args>
    void unpack_list(T* arg, Args&&... args);

    // Unpacks last parameter
    template<typename T>
    void unpack_list(T* arg);

    // Stores all functions
    map<void*, any*> m_functions;
};

// Constructor for single function pointer
template<typename T>
Dyn_class::Dyn_class(T* arg)
{
    add_func(arg);
}

// Variadic Constuctor for initial list of functions
template <typename T, typename... Args>
Dyn_class::Dyn_class(T* arg, Args&&... args)
{
    add_func(arg);
    Dyn_class::unpack_list(args...);
}

// Helper function to unpack parameters
template<typename T, typename... Args>
void Dyn_class::unpack_list(T* arg, Args&&... args)
{
    add_func(arg);
    Dyn_class::unpack_list(args...);
}

// Helper function to unpack last parameter
template<typename T>
void Dyn_class::unpack_list(T* arg)
{
    add_func(arg);
}

// Add function to map
template <typename T>
void Dyn_class::add_func(T* a_func)
{
    // Create map value
    any* temp = new any(a_func);

    // Create map key
    m_functions[(void*)a_func] = temp;
}

// Remove function from map
template <typename T>
void Dyn_class::remove_func(T* a_key)
{
    // Searches map for key
    if (m_functions.find((void*)a_key) == m_functions.end())
    {
        // Function not found
        std::cout << "Function doesn't exist" << std::endl;
        return;
    }

    // Delete key and value of function
    delete m_functions.at((void*)a_key);
    m_functions.erase((void*)a_key);
}

// Call functions in map
template <typename T>
T* Dyn_class::operator()(T* a_key)
{
    // Searches map for key
    if (m_functions.find((void*)a_key) == m_functions.end())
    {
        // Function not found
        std::cout << "Function doesn't exist" << std::endl;
        return nullptr;
    }

    // Re-cast the function pointer back to its correct data type
    // std::any* -> T*
    return any_cast<T*>(*m_functions.at((void*)a_key));
}

// Define Test Functions
void print()
{
    std::cout << "Hello World." << std::endl;
}
int mult(int a)
{
    return 2 * a;
}
std::string message(std::string a_string)
{
    return a_string + " are fun";
}
char increment(char a_string)
{
    return a_string + 1;
}
int sum(const std::vector<int>& v)
{
    int sum = 0;
    for (const auto& value : v)
        sum += value;
    return sum;
}
bool isEven(int x) { return x % 2 == 0; }

int main()
{
    // Create lambda function
    auto lam = [](int x) { return x * 2; };

    // Create function objects
    function incr_obj = increment;
    function sum_obj = sum;

    // Using each Dyn_class constructor
    Dyn_class d1;
    Dyn_class d2(print);
    Dyn_class d3(print, mult, &incr_obj);   // inital list of functions

    // Add functions
    d3.add_func(message);
    d3.add_func(&lam);
    d3.add_func(&sum_obj);

    // Access functions
    cout << "Print func: ";
    d3(print)();

    cout << "Mult func: " << d3(mult)(4) << endl;
    cout << "Message func: " << d3(message)("Ducks") << endl;
    
    // Access lamda functions and function objects
    cout << "Lambda func: " << (*d3(&lam))(6) << endl;
    cout << "Increment func object: " << (*d3(&incr_obj))('a') << endl;

    vector<int> v1{ 1,2,3,4,5,6,7,8,9 };
    cout << "Sum func: " << (*d3(&sum_obj))(v1) << endl;
    
    // Remove functions
    d3.remove_func(print);
    auto print_func = d3(print);    // Outputs error

    // Try to access function not added to class
    auto even_func = d3(isEven);

    /*
    The following code causes the program to exit.
     
    d3(print)();
    
    Since d3(print) returns a nullptr, attemping to call this nullptr with ()
    promply exits the program.
    */
    return 0;
}