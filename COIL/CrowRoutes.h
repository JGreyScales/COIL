#pragma once

#define ASIO_STANDALONE
#define NOMINMAX

#include "crow_all.h"
#include "App.h"
#include "fileFetcher.h"

class CrowRoutes
{
public:

    static int robotCounts[2];

    static crow::response JsonResponse(int code, const crow::json::wvalue& body);
    static crow::json::wvalue BuildCommandJson(PktDef* pktResponse);


    static void RegisterRoutes(crow::SimpleApp& app);

    static crow::response HandleRoot();
    static crow::response HandleDrive(const crow::request& req, int direction);
    static crow::response HandleTelemetry(const crow::request& req);
	static crow::response HandleSleep(const crow::request& req);
	static crow::response HandleRestart(const crow::request& req);

    //// called from COIL.cpp after app->sendData(...)
    //static void PublishResponse(PktDef* pkt);

    //// optional reset helper
    //static void ResetResponseState();
};