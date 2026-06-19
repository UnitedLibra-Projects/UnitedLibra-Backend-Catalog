#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/coroutine.h>

static std::vector<int> parseIdsFromJson(const std::shared_ptr<Json::Value>& json_req) 
{
   std::vector<int> id_vec;
   if (json_req && json_req->isArray()) 
   {
      id_vec.reserve(json_req->size());
      for (const auto& id_json : *json_req) 
      {
         id_vec.push_back(id_json["id"].asInt());
      }
   }
   return id_vec;
}

static Json::Value mapDictionaryRow(const drogon::orm::Row& row) 
{
   Json::Value item;
   item["id"] = row["id"].as<int>();
   item["name"] = row["name"].as<std::string>();
   return item;
}

class BooksController: public drogon::HttpController<BooksController>
{
public:
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(BooksController::addBook, "/books/add-book", drogon::Post);
   ADD_METHOD_TO(BooksController::deleteBooks, "/books/delete-book", drogon::Post);
   ADD_METHOD_TO(BooksController::getBooks, "/books/get-book", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addBook(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getBooks(drogon::HttpRequestPtr req);
};

class CopyBooksController: public drogon::HttpController<CopyBooksController>
{
public:
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(CopyBooksController::addCopyBooks, "/books/add-copyBooks", drogon::Post);
   ADD_METHOD_TO(CopyBooksController::deleteCopyBooks, "/books/delete-copyBooks", drogon::Post);
   ADD_METHOD_TO(CopyBooksController::getCopyBooks, "/books/get-copyBooks", drogon::Get);
   ADD_METHOD_TO(CopyBooksController::getBookCover, "/books/get-cover", drogon::Get);
   ADD_METHOD_TO(CopyBooksController::changeCopyBooks, "/books/change-copyBooks", drogon::Post);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addCopyBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteCopyBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getCopyBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getBookCover(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> changeCopyBooks(drogon::HttpRequestPtr req);
};

class PublishersController: public drogon::HttpController<PublishersController>
{
public:
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(PublishersController::addPublishers, "/books/add-publishers", drogon::Post);
   ADD_METHOD_TO(PublishersController::deletePublishers, "/books/delete-publishers", drogon::Post);
   ADD_METHOD_TO(PublishersController::getPublishers, "/books/get-publishers", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addPublishers(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deletePublishers(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getPublishers(drogon::HttpRequestPtr req);
};

class AuthorsController: public drogon::HttpController<AuthorsController>
{
public:
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(AuthorsController::addAuthors, "/books/add-authors", drogon::Post);
   ADD_METHOD_TO(AuthorsController::deleteAuthors, "/books/delete-authors", drogon::Post);
   ADD_METHOD_TO(AuthorsController::getAuthors, "/books/get-authors", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addAuthors(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteAuthors(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getAuthors(drogon::HttpRequestPtr req);
};

class CategoriesController: public drogon::HttpController<CategoriesController>
{
public:
   METHOD_LIST_BEGIN
   ADD_METHOD_TO(CategoriesController::addCategories, "/books/add-categories", drogon::Post);
   ADD_METHOD_TO(CategoriesController::deleteCategories, "/books/delete-categories", drogon::Post);
   ADD_METHOD_TO(CategoriesController::getCategories, "/books/get-categories", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addCategories(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteCategories(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getCategories(drogon::HttpRequestPtr req);
};
