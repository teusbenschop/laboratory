import demo;
import std;

int main() {
    func("hello");     // imported 'func' from 'demo'
    A<std::string> a;  // imported 'A' from 'demo' and 'string' from 'std'
    a.f("x");
    return 0;
}
