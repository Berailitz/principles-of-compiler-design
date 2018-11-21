#include "analyser.cpp"

int main(int argc, char const *argv[])
{
    ifstream stream;
    Analyser analyser;
    if (argc == 2)
    {
        stream.open(argv[1]);
        analyser.create_grammar(stream);
        analyser.receive_text(stream);
    }
    else
    {
        analyser.create_grammar(cin);
        analyser.receive_text(cin);
    }
    cin.get();
    return 0;
}
