#include "controller.h"
#include "BaseRepository.h"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <string>
#include <vector>
#include <trantor/utils/Logger.h>

drogon::Task<drogon::HttpResponsePtr> CopyBooksController::addCopyBooks(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    Json::Value json_resp;

    if (!json_req) {
        json_resp["status"] = "error";
        json_resp["message"] = "Empty JSON body";
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    try {
        int book_id = (*json_req)["book_id"].asInt();
        std::string picture_address = (*json_req)["picture_address"].asString();
        std::string status = (*json_req)["status"].asString();
        int count = (*json_req)["count"].asInt();
        std::string state_description = (*json_req)["state_description"].asString();

        if (!(*json_req).isMember("location") || !(*json_req)["location"].isObject()) {
            throw std::runtime_error("Missing 'location' object in request");
        }
        auto json_location = (*json_req)["location"];
        std::string hall = json_location["hall"].asString();
        std::string shelf = json_location["shelf"].asString();

        std::vector<int> publisher_ids;
        if ((*json_req).isMember("publisher_ids") && (*json_req)["publisher_ids"].isArray()) {
            for (const auto& id_json : (*json_req)["publisher_ids"]) {
                publisher_ids.push_back(id_json.asInt());
            }
        }

        auto db_client = drogon::app().getDbClient();
        auto trans = co_await db_client->newTransactionCoro();

        auto loc_result = co_await trans->execSqlCoro(
            R"(
            INSERT INTO "Locations" (hall, shelf)
            VALUES ($1, $2)
            RETURNING id;
            )",
            hall, shelf
        );
        int new_location_id = loc_result[0]["id"].as<int>();

        auto copy_result = co_await trans->execSqlCoro(
            R"(
            INSERT INTO "BookCopy" (book_id, picture_adress, status, location_id, count, state_description)
            VALUES ($1, $2, $3, $4, $5, $6)
            RETURNING id;
            )",
            book_id, picture_address, status, new_location_id, count, state_description
        );
        int new_copy_id = copy_result[0]["id"].as<int>();

        for (int pub_id : publisher_ids) {
            co_await trans->execSqlCoro(
                R"(
                INSERT INTO "BookCopyPublishers" (book_copy_id, publisher_id)
                VALUES ($1, $2);
                )",
                new_copy_id, pub_id
            );
        }

        json_resp["status"] = "ok";
        json_resp["inserted_id"] = new_copy_id;

    } catch (const std::exception& ex) {
        LOG_ERROR << "Failed to add book copy with relations: " << ex.what();
        json_resp["status"] = "error";
        json_resp["message"] = ex.what();
    }

    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> CopyBooksController::deleteCopyBooks(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    std::vector<int> id_vec;

    if (json_req && json_req->isArray()) {
        id_vec.reserve(json_req->size());
        for (const auto& id_json : *json_req) {
            id_vec.push_back(id_json["id"].asInt());
        }
    }

    bool success = co_await BaseRepository::deleteByIds("\"BookCopy\"", id_vec);

    Json::Value json_resp;
    json_resp["status"] = success ? "ok" : "error";
    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}

drogon::Task<drogon::HttpResponsePtr> CopyBooksController::getCopyBooks(drogon::HttpRequestPtr req)
{
    Json::Value json_resp;
    json_resp["data"] = Json::arrayValue;

    std::string sort_by = req->getParameter("sort_by");
   std::string order = req->getParameter("order");

   if (sort_by.empty()) {
       sort_by = "title";
   }
   if (order.empty()) {
       order = "ASC";
   }


    std::string order_clause = "ORDER BY b.title ASC"; 
    if (sort_by == "author") {
        order_clause = "ORDER BY min(a.name) ASC";
    } else if (sort_by == "category") {
        order_clause = "ORDER BY min(c.name) ASC";
    } else if (sort_by == "publisher") {
        order_clause = "ORDER BY min(p.name) ASC";
    }

    auto db_client = drogon::app().getDbClient();

    try {
        std::string sql = R"(
            SELECT 
                bc.id AS copy_id,
                bc.book_id,
                bc.picture_adress,
                bc.status,
                bc.count,
                bc.state_description,
                b.title AS book_title,
                b.isbn AS book_isbn,
                b.year AS book_year,
                l.hall AS location_hall,
                l.shelf AS location_shelf,
                COALESCE(json_agg(DISTINCT jsonb_build_object('id', a.id, 'name', a.name)) 
                         FILTER (WHERE a.id IS NOT NULL), '[]') AS authors,
                COALESCE(json_agg(DISTINCT jsonb_build_object('id', c.id, 'name', c.name)) 
                         FILTER (WHERE c.id IS NOT NULL), '[]') AS categories,
                COALESCE(json_agg(DISTINCT jsonb_build_object('id', p.id, 'name', p.name)) 
                         FILTER (WHERE p.id IS NOT NULL), '[]') AS publishers
            FROM "BookCopy" bc
            JOIN books b ON bc.book_id = b.id
            LEFT JOIN "Locations" l ON bc.location_id = l.id
            LEFT JOIN "BookAuthors" ba ON b.id = ba.book_id
            LEFT JOIN authors a ON ba.author_id = a.id
            LEFT JOIN "BookCategories" bcg ON b.id = bcg.book_id
            LEFT JOIN categories c ON bcg.category_id = c.id
            LEFT JOIN "BookCopyPublishers" bcp ON bc.id = bcp.book_copy_id
            LEFT JOIN publisher p ON bcp.publisher_id = p.id
            GROUP BY bc.id, b.id, l.id
        )" + order_clause + ";";

        auto result = co_await db_client->execSqlCoro(sql);

        for (const auto& row : result) {
            Json::Value copy;
            copy["id"] = row["copy_id"].as<int>();
            copy["book_id"] = row["book_id"].as<int>();
            copy["picture_address"] = row["picture_adress"].as<std::string>();
            copy["status"] = row["status"].as<std::string>();
            copy["count"] = row["count"].as<int>();
            copy["state_description"] = row["state_description"].as<std::string>();

            copy["book_title"] = row["book_title"].as<std::string>();
            copy["book_isbn"] = row["book_isbn"].as<std::string>();
            copy["book_year"] = row["book_year"].as<int>();
            
            Json::Value loc;
            loc["hall"] = row["location_hall"].as<std::string>();
            loc["shelf"] = row["location_shelf"].as<std::string>();
            copy["location"] = loc;

            Json::Reader reader;
            
            Json::Value authors_json;
            reader.parse(row["authors"].as<std::string>(), authors_json);
            copy["authors"] = authors_json;

            Json::Value categories_json;
            reader.parse(row["categories"].as<std::string>(), categories_json);
            copy["categories"] = categories_json;

            Json::Value publishers_json;
            reader.parse(row["publishers"].as<std::string>(), publishers_json);
            copy["publishers"] = publishers_json;

            json_resp["data"].append(copy);
        }
        json_resp["status"] = "ok";

    } catch (const std::exception& ex) {
        LOG_ERROR << "Failed to get sorted copy books: " << ex.what();
        json_resp["status"] = "error";
        json_resp["message"] = ex.what();
    }

    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}
drogon::Task<drogon::HttpResponsePtr> CopyBooksController::getBookCover(drogon::HttpRequestPtr req)
{
    std::string id_str = req->getParameter("id");
    Json::Value json_resp;

    if (id_str.empty()) {
        json_resp["status"] = "error";
        json_resp["message"] = "Missing 'id' parameter";
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    auto db_client = drogon::app().getDbClient();
    drogon::HttpResponsePtr resp;

    try {
        int copy_id = std::stoi(id_str);

        auto result = co_await db_client->execSqlCoro(
            R"(
            SELECT picture_adress FROM "BookCopy" WHERE id = $1;
            )",
            copy_id
        );

        if (result.empty()) {
            resp = drogon::HttpResponse::newNotFoundResponse();
            co_return resp;
        }

        std::string local_path = result[0]["picture_adress"].as<std::string>();

        if (local_path.empty()) {
            json_resp["status"] = "error";
            json_resp["message"] = "No cover image registered for this book copy";
            co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
        }

        if (!std::filesystem::exists(local_path)) {
            LOG_WARN << "File not found on disk: " << local_path;
            json_resp["status"] = "error";
            json_resp["message"] = "Image file missing on server storage";
            co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
        }

        resp = drogon::HttpResponse::newFileResponse(local_path);

    } catch (const std::exception& ex) {
        LOG_ERROR << "Failed to fetch book cover: " << ex.what();
        json_resp["status"] = "error";
        json_resp["message"] = ex.what();
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    co_return resp;
}
drogon::Task<drogon::HttpResponsePtr> CopyBooksController::changeCopyBooks(drogon::HttpRequestPtr req)
{
    auto json_req = req->getJsonObject();
    Json::Value json_resp;

    if (!json_req) {
        json_resp["status"] = "error";
        json_resp["message"] = "Empty JSON body";
        co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
    }

    try {
        int copy_id = (*json_req)["id"].asInt(); 
        std::string new_status = (*json_req)["status"].asString(); 
        int count_to_move = (*json_req)["count"].asInt(); 

        if (count_to_move <= 0) {
            throw std::runtime_error("Count must be greater than 0");
        }

        auto db_client = drogon::app().getDbClient();
        auto trans = co_await db_client->newTransactionCoro();

        auto select_res = co_await trans->execSqlCoro(
            R"(
            SELECT book_id, picture_adress, location_id, count, state_description 
            FROM "BookCopy" 
            WHERE id = $1;
            )",
            copy_id
        );

        if (select_res.empty()) {
            throw std::runtime_error("Original BookCopy row not found");
        }

        auto row = select_res[0];
        int current_count = row["count"].as<int>();

        if (current_count < count_to_move) {
            throw std::runtime_error("Not enough books in stock to change status. Available: " + std::to_string(current_count));
        }

        co_await trans->execSqlCoro(
            R"(
            UPDATE "BookCopy" 
            SET count = count - $1 
            WHERE id = $2;
            )",
            count_to_move, copy_id
        );

        int book_id = row["book_id"].as<int>();
        std::string picture_address = row["picture_adress"].as<std::string>();
        int location_id = row["location_id"].as<int>();
        std::string state_description = row["state_description"].as<std::string>();

        auto insert_res = co_await trans->execSqlCoro(
            R"(
            INSERT INTO "BookCopy" (book_id, picture_adress, status, location_id, count, state_description)
            VALUES ($1, $2, $3, $4, $5, $6)
            RETURNING id;
            )",
            book_id, picture_address, new_status, location_id, count_to_move, state_description
        );
        int new_copy_id = insert_res[0]["id"].as<int>();

        auto pub_res = co_await trans->execSqlCoro(
            R"(
            SELECT publisher_id FROM "BookCopyPublishers" WHERE book_copy_id = $1;
            )",
            copy_id
        );

        for (const auto& pub_row : pub_res) {
            int publisher_id = pub_row["publisher_id"].as<int>();
            co_await trans->execSqlCoro(
                R"(
                INSERT INTO "BookCopyPublishers" (book_copy_id, publisher_id)
                VALUES ($1, $2);
                )",
                new_copy_id, publisher_id
            );
        }

        json_resp["status"] = "ok";
        json_resp["message"] = "Status split successfully";
        json_resp["new_copy_id"] = new_copy_id;

    } catch (const std::exception& ex) {
        LOG_ERROR << "Failed to change copy books status: " << ex.what();
        json_resp["status"] = "error";
        json_resp["message"] = ex.what();
    }

    co_return drogon::HttpResponse::newHttpJsonResponse(json_resp);
}
