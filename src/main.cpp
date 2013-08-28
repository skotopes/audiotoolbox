/*
 *  main.cpp
 *  main
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include "audiotoolbox.h"

int main(int argc, char **argv) {
    AudioToolbox at(argc, argv);
    return at.run();
}
