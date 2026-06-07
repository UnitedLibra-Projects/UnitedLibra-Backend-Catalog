#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <string>
#include <trantor/utils/Logger.h>
#include "BaseRepository.h"

drogon::Task<drogon::HttpResponsePtr> AuthorsController::addAuthors(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    std::string name = json_req ? (*json_req)["name"].asString() : "";

    bool success = co_await BaseRepository::addDictionary("authors", name);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> AuthorsController::deleteAuthors(drogon::HttpRequestPtr req)
{
    auto id_vec = parseIdsFromJson(req->getJsonObject());
    bool success = co_await BaseRepository::deleteByIds("authors", id_vec);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> AuthorsController::getAuthors(drogon::HttpRequestPtr req)
{
    auto json_data = co_await BaseRepository::getAll("authors", mapDictionaryRow);
    co_return drogon::HttpResponse::newHttpJsonResponse(json_data);
}
