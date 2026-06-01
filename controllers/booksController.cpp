#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <string>
#include <trantor/utils/Logger.h>

drogon::Task<drogon::HttpResponsePtr> BooksController::addBook(drogon::HttpRequestPtr req)
{
   auto json_req = req->getJsonObject();
   
   std::string title = (*json_req)["title"].asString();
   std::string isbn = (*json_req)["isbn"].asString();
   int year = (*json_req)["year"].asInt();
   int publisher_id = (*json_req)["publisher_id"].asInt();

   auto db_client = drogon::app().getDbClient();

   try {
      co_await db_client->execSqlCoro(
            R"(
            INSERT INTO books (title, isbn, year, publisher_id)
            VALUES ($1, $2, $3, $4)
            )",
            title, isbn, year, publisher_id
            );
   } catch (std::exception& ex) {
      LOG_ERROR << "error: " << ex.what();
   }
   Json::Value json_resp;
   json_resp["status"] = "ok";

   auto resp = drogon::HttpResponse::newHttpJsonResponse(json_resp);

   co_return resp;
}

drogon::Task<drogon::HttpResponsePtr> BooksController::deleteBooks(drogon::HttpRequestPtr req)
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
            DELETE FROM books
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

drogon::Task<drogon::HttpResponsePtr> BooksController::getBooks(drogon::HttpRequestPtr req)
{

}
