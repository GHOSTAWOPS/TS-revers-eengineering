#include "presentation/occ/OccViewerWidget.h"

#include <QApplication>

#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    tsrebar::OccViewerWidget viewer;
    if (viewer.hasViewer()) {
        std::cerr << "viewer should initialize lazily\n";
        return 1;
    }

    viewer.setSelectionMode(TopAbs_FACE);
    if (viewer.selectionMode() != TopAbs_FACE) {
        std::cerr << "selection mode was not retained\n";
        return 1;
    }

    return 0;
}

