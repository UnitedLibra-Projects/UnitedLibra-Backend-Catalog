#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <string>
#include <trantor/utils/Logger.h>

drogon::Task<drogon::HttpResponsePtr> AuthorsController::addAuthors(drogon::HttpRequestPtr req)
{
   auto json_req = req->getJsonObject();
   
   std::string name = (*json_req)["name"].asString();

   auto db_client = drogon::app().getDbClient();

   try {
      co_await db_client->execSqlCoro(
            R"(
            INSERT INTO publishers (name)
            VALUES ($1)
            )",
            name
            );
   } catch (std::exception& ex) {
      LOG_ERROR << "error: " << ex.what();
   }
   Json::Value json_resp;
   json_resp["status"] = "ok";

   auto resp = drogon::HttpResponse::newHttpJsonResponse(json_resp);

   co_return resp;
}

drogon::Task<drogon::HttpResponsePtr> AuthorsController::deleteAuthors(drogon::HttpRequestPtr req)
{
   auto json_req = req->getJsonObject();

   std::vector<int> id_vec;
   id_vec.reserve(json_req->size());

   for(auto id_json: *json_req)
   {
      id_vec.push_back(id_json["id"].asInt());
   }

   auto db_client = drogon::app().getDbClient();

   try {
      co_await db_client->execSqlCoro(
            R"(
            DELETE FROM publishers
            WHERE id = ANY($1::int[]);
            )",
            id_vec
            );
   } catch (std::exception& ex) {
      LOG_ERROR << "error: " << ex.what();
   }

   Json::Value json_resp;
   json_resp["status"] = "ok";

   auto resp = drogon::HttpResponse::newHttpJsonResponse(json_resp);

   co_return resp;
}
