#ifndef POVEXPORT_H
#define POVEXPORT_H

#include <vector>
#include <image.h>
#include "marching.h"

void povExport(const vector<TRIANGLE>& tri, const string& fn);
void povExportDepth(const Image& depth, const Image& depthvalid, const string& fn, double factor);
void cvExportDepth(const Image& depth, const Image& depthvalid, const string& fn, double factor);
void cvExport(const vector<TRIANGLE>& tri, const string& fn);
void tvExport(const vector<TRIANGLE>& tri, const string& fn);
#endif
