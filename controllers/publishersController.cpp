#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <string>
#include <trantor/utils/Logger.h>
#include "BaseRepository.h"

drogon::Task<drogon::HttpResponsePtr> PublishersController::addPublishers(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    std::string name = json_req ? (*json_req)["name"].asString() : "";

    bool success = co_await BaseRepository::addDictionary("publisher", name);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> PublishersController::deletePublishers(drogon::HttpRequestPtr req)
{
    auto id_vec = parseIdsFromJson(req->getJsonObject());
    bool success = co_await BaseRepository::deleteByIds("publisher", id_vec);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> PublishersController::getPublishers(drogon::HttpRequestPtr req)
{
    auto json_data = co_await BaseRepository::getAll("publisher", mapDictionaryRow);
    co_return drogon::HttpResponse::newHttpJsonResponse(json_data);
}
