#include "audiotoolbox.h"

int main(int argc, char **argv) {
    AudioToolbox at(argc, argv);
    return at.run();
}
