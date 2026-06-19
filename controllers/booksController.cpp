#include "controller.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <string>
#include <trantor/utils/Logger.h>
#include "BaseRepository.h"
#include <drogon/HttpAppFramework.h>

drogon::Task<drogon::HttpResponsePtr> BooksController::addBook(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    Json::Value json_resp;

    if (!json_req) {
        json_resp["status"] = "error";
        json_resp["message"] = "Empty JSON body";
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    std::string title = (*json_req)["title"].asString();
    std::string isbn = (*json_req)["isbn"].asString();
    int year = (*json_req)["year"].asInt();
    std::string description = "";
    if ((*json_req).isMember("description"))
    {
        description = (*json_req)["description"].asString();
    }

    int publisher_id = 0;
    if ((*json_req).isMember("publisher_id"))
    {
        publisher_id = (*json_req)["publisher_id"].asInt();
    }

    if (publisher_id <= 0)
    {
        json_resp["status"] = "error";
        json_resp["message"] = "publisher_id is required";
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    std::vector<int> author_ids;
    if ((*json_req).isMember("author_ids") && (*json_req)["author_ids"].isArray()) {
        for (const auto& id_json : (*json_req)["author_ids"]) {
            author_ids.push_back(id_json.asInt());
        }
    }

    std::vector<int> category_ids;
    if ((*json_req).isMember("category_ids") && (*json_req)["category_ids"].isArray()) {
        for (const auto& id_json : (*json_req)["category_ids"]) {
            category_ids.push_back(id_json.asInt());
        }
    }

    auto db_client = drogon::app().getDbClient();
    bool success = false;

    auto trans = co_await db_client->newTransactionCoro();

    try {
        auto result = co_await trans->execSqlCoro(
            R"(
            INSERT INTO books (title, description, isbn, year, publisher_id)
            VALUES ($1, $2, $3, $4, $5)
            RETURNING id;
            )",
            title, description, isbn, year, publisher_id
        );

        if (result.empty()) {
            throw std::runtime_error("Failed to insert book, no ID returned");
        }

        int new_book_id = result[0]["id"].as<int>();

        for (int author_id : author_ids) {
            co_await trans->execSqlCoro(
                R"(
                INSERT INTO bookauthors (book_id, author_id)
                VALUES ($1, $2);
                )",
                new_book_id, author_id
            );
        }

        for (int category_id : category_ids) {
            co_await trans->execSqlCoro(
                R"(
                INSERT INTO bookcategories (book_id, category_id)
                VALUES ($1, $2);
                )",
                new_book_id, category_id
            );
        }

        success = true;
    } catch (const std::exception& ex) {
        LOG_ERROR << "Transaction failed! Error inserting book with relations: " << ex.what();
        json_resp["message"] = ex.what();
    }

    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> BooksController::deleteBooks(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    std::vector<int> id_vec;

    if (json_req && json_req->isArray()) {
        id_vec.reserve(json_req->size());
        for (const auto& id_json : *json_req) {
            id_vec.push_back(id_json["id"].asInt());
        }
    }

    bool success = co_await BaseRepository::deleteByIds("books", id_vec);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> BooksController::getBooks(drogon::HttpRequestPtr req)
{
    auto json_data = co_await BaseRepository::getAll("books", [](const drogon::orm::Row& row) {
        Json::Value book;
        book["id"] = row["id"].as<int>();
        book["title"] = row["title"].as<std::string>();
        book["isbn"] = row["isbn"].as<std::string>();
        book["year"] = row["year"].as<int>();
        book["description"] = row["description"].as<std::string>();
        book["publisher_id"] = row["publisher_id"].as<int>();
        return book;
    });

    co_return drogon::HttpResponse::newHttpJsonResponse(json_data);
}
