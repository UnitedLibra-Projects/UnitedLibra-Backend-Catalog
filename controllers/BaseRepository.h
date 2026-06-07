#pragma once
#include <drogon/drogon.h>
#include <vector>
#include <string>
#include <functional>
#include <trantor/utils/Logger.h>

class BaseRepository {
public:
   static drogon::Task<bool> deleteByIds(const std::string& tableName, const std::vector<int>& ids) 
   {
      if (ids.empty()) co_return true;
        
      auto db_client = drogon::app().getDbClient();
      try 
      {
         std::string sql = "DELETE FROM " + tableName + " WHERE id = ANY($1::int[]);";
         co_await db_client->execSqlCoro(sql, ids);
         co_return true;
      } 
      catch (const std::exception& ex) 
      {
         LOG_ERROR << "Error deleting from " << tableName << ": " << ex.what();
         co_return false;
      }
   }

   static drogon::Task<Json::Value> getAll(const std::string& tableName, 
                                            std::function<Json::Value(const drogon::orm::Row&)> rowMapper)
   {
      Json::Value json_resp;
      json_resp["data"] = Json::arrayValue;
        
      auto db_client = drogon::app().getDbClient();
      try 
      {
         std::string sql = "SELECT * FROM " + tableName;
         auto result = co_await db_client->execSqlCoro(sql);
            
         for (const auto& row : result) 
         {
            json_resp["data"].append(rowMapper(row));
         }
         json_resp["status"] = "ok";
      } 
      catch (const std::exception& ex) 
      {
         LOG_ERROR << "Error selecting from " << tableName << ": " << ex.what();
         json_resp["status"] = "error";
         json_resp["message"] = ex.what();
      }
      co_return json_resp;
    }

   static drogon::Task<bool> addDictionary(const std::string& tableName, const std::string& nameValue) 
   {
      auto db_client = drogon::app().getDbClient();
      try 
      {
         std::string sql = "INSERT INTO " + tableName + " (name) VALUES ($1);";
         co_await db_client->execSqlCoro(sql, nameValue);
         co_return true;
      } 
      catch (const std::exception& ex) 
      {
         LOG_ERROR << "Error inserting into " << tableName << ": " << ex.what();
         co_return false;
      }
   }
};
