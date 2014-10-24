#include <executor/executor.h>

int main(int argc, char** argv)
{
    exec::executor e;
    return e.execute(argc, argv);
}
