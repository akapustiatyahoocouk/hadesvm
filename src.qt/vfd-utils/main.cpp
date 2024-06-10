#include "vfd-utils/API.hpp"

int main(int argc, char *argv[])
{
    QTextStream out(stdout);
    for (int i = 0; i < argc; i++)
    {
        out << argv[i] << "\n";
    }
}
