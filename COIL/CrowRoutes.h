#pragma once

#define ASIO_STANDALONE
#define NOMINMAX

#include "crow_all.h"

#include "App.h"

class initCrow
{
public:
    static void initializeCrowRoutes(crow::SimpleApp& web, App& app);
    static void startAppWorker(App& app);
};