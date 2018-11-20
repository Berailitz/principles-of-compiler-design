#include "analyser.cpp"

int main(int argc, char const *argv[])
{
    Analyser analyser;
    if (argc == 2)
    {
        ifstream stream(argv[1]);
        analyser.create_grammar(stream);
    }
    else
    {
        analyser.create_grammar(cin);
    }
    analyser.calculate_firsts();
    analyser.calculate_follows();
    analyser.build_table();
    analyser.print_table();
    cin.get();
    return 0;
}
