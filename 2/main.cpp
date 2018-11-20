#include "analyser.cpp"

int main(int argc, char const *argv[])
{
    ifstream stream;
    Analyser analyser;
    if (argc == 2)
    {
        stream.open(argv[1]);
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
    if (argc == 2)
    {
        analyser.receive_text(stream);
    }
    else
    {
        analyser.receive_text(cin);
    }
    cin.get();
    return 0;
}
