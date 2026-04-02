#pragma once

#define ASIO_STANDALONE
#include "crow_all.h"

#include "App.h"
#include "PktDef.h"
#include "DriveBodyDef.h"
#include "TurnBodyDef.h"
#include "TelemetryBodyDef.h"
#include "AckBodyDef.h"
#include "HeaderDef.h"
#include "fileFetcher.h"

class initCrow
{
public:
    static void initializeCrowRoutes(crow::SimpleApp& web, App& app);
    static void startAppWorker(App& app);
};