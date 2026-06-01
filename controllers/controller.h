#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/coroutine.h>

class BooksController: public drogon::HttpController<BooksController>
{
public:
   METHOD_LIST_BEGIN
   METHOD_ADD(BooksController::addBook, "/books/add-book", drogon::Post);
   METHOD_ADD(BooksController::deleteBooks, "/books/delete-book", drogon::Post);
   METHOD_ADD(BooksController::getBooks, "/books/get-book", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addBook(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getBooks(drogon::HttpRequestPtr req);
};

class CopyBooksController: public drogon::HttpController<CopyBooksController>
{
public:
   METHOD_LIST_BEGIN
   METHOD_ADD(CopyBooksController::addCopyBooks, "/books/add-copyBooks", drogon::Post);
   METHOD_ADD(CopyBooksController::deleteCopyBooks, "/books/delete-copyBooks", drogon::Post);
   METHOD_ADD(CopyBooksController::getCopyBooks, "/books/get-copyBooks", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addCopyBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteCopyBooks(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getCopyBooks(drogon::HttpRequestPtr req);
};

class PublishersController: public drogon::HttpController<PublishersController>
{
public:
   METHOD_LIST_BEGIN
   METHOD_ADD(PublishersController::addPublishers, "/books/add-publishers", drogon::Post);
   METHOD_ADD(PublishersController::deletePublishers, "/books/delete-publishers", drogon::Post);
   METHOD_ADD(PublishersController::getPublishers, "/books/get-publishers", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addPublishers(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deletePublishers(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getPublishers(drogon::HttpRequestPtr req);
};

class AuthorsController: public drogon::HttpController<AuthorsController>
{
public:
   METHOD_LIST_BEGIN
   METHOD_ADD(AuthorsController::addAuthors, "/books/add-authors", drogon::Post);
   METHOD_ADD(AuthorsController::deleteAuthors, "/books/delete-authors", drogon::Post);
   METHOD_ADD(AuthorsController::getAuthors, "/books/get-authors", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addAuthors(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteAuthors(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getAuthors(drogon::HttpRequestPtr req);
};

class CategoriesController: public drogon::HttpController<CategoriesController>
{
public:
   METHOD_LIST_BEGIN
   METHOD_ADD(CategoriesController::addCategories, "/books/add-categories", drogon::Post);
   METHOD_ADD(CategoriesController::deleteCategories, "/books/delete-categories", drogon::Post);
   METHOD_ADD(CategoriesController::getCategories, "/books/get-categories", drogon::Get);
   METHOD_LIST_END

   drogon::Task<drogon::HttpResponsePtr> addCategories(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> deleteCategories(drogon::HttpRequestPtr req);
   drogon::Task<drogon::HttpResponsePtr> getCategories(drogon::HttpRequestPtr req);
};
