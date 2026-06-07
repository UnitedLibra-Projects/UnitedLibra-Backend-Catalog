#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <string>
#include <trantor/utils/Logger.h>
#include "BaseRepository.h"

drogon::Task<drogon::HttpResponsePtr> CategoriesController::addCategories(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    std::string name = json_req ? (*json_req)["name"].asString() : "";

    bool success = co_await BaseRepository::addDictionary("categories", name);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> CategoriesController::deleteCategories(drogon::HttpRequestPtr req)
{
    auto id_vec = parseIdsFromJson(req->getJsonObject());
    bool success = co_await BaseRepository::deleteByIds("categories", id_vec);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> CategoriesController::getCategories(drogon::HttpRequestPtr req)
{
    auto json_data = co_await BaseRepository::getAll("categories", mapDictionaryRow);
    co_return drogon::HttpResponse::newHttpJsonResponse(json_data);
}
