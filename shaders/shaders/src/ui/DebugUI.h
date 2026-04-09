#pragma once
#include "../core/PassManager.h"
#include "../core/Metrics.h"
#include <functional>

class PluginReloader;

void DrawDebugUI(PassManager& pipeline, Metrics& metrics, PluginReloader* reloader = nullptr);
void DrawUniformEditor(PassEntry& pass);
