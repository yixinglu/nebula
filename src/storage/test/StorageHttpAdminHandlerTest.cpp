/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include <gtest/gtest.h>
#include <folly/json.h>
#include "webservice/Router.h"
#include "webservice/WebService.h"
#include "webservice/test/TestUtils.h"
#include "storage/http/StorageHttpAdminHandler.h"
#include "storage/test/TestUtils.h"
#include "fs/TempDir.h"

namespace nebula {
namespace storage {

using nebula::storage::TestUtils;
using nebula::fs::TempDir;

class StorageHttpAdminHandlerTestEnv : public ::testing::Environment {
public:
    void SetUp() override {
        FLAGS_ws_http_port = 0;
        FLAGS_ws_h2_port = 0;
        rootPath_ = std::make_unique<fs::TempDir>("/tmp/StorageHttpAdminHandler.XXXXXX");
        kv_ = TestUtils::initKV(rootPath_->path());
        schemaMan_ = TestUtils::mockSchemaMan();

        VLOG(1) << "Starting web service...";
        webSvc_ = std::make_unique<WebService>();
        auto& router = webSvc_->router();
        router.get("/admin").handler([this](nebula::web::PathParams&&) {
            return new storage::StorageHttpAdminHandler(schemaMan_.get(), kv_.get());
        });
        auto status = webSvc_->start();
        ASSERT_TRUE(status.ok()) << status;
    }

    void TearDown() override {
        webSvc_.reset();
        schemaMan_.reset();
        kv_.reset();
        rootPath_.reset();
        VLOG(1) << "Web service stopped";
    }

protected:
    std::unique_ptr<WebService> webSvc_;
    std::unique_ptr<kvstore::KVStore> kv_;
    std::unique_ptr<fs::TempDir> rootPath_;
    std::unique_ptr<meta::SchemaManager> schemaMan_;
};

static std::string request(const std::string& url) {
    auto request =
        folly::stringPrintf("http://%s:%d%s", FLAGS_ws_ip.c_str(), FLAGS_ws_http_port, url.c_str());
    auto resp = http::HttpClient::get(request);
    EXPECT_TRUE(resp.ok());
    return resp.value();
}

static void checkInvalidRequest(const std::string& url, const std::string& errMsg) {
    ASSERT_EQ(0, request(url).find(errMsg));
}

TEST(StoragehHttpAdminHandlerTest, TestInvalidRequest) {
    checkInvalidRequest("/admin", "Space should not be empty");
    checkInvalidRequest("/admin?space=xx", "Op should not be empty");
    checkInvalidRequest("/admin?space=xx&op=yy", "Can't find space xx");
    checkInvalidRequest("/admin?space=0&op=yy", "Unknown operation yy");
}

TEST(StoragehHttpAdminHandlerTest, TestSupportedOperations) {
    ASSERT_EQ("ok", request("/admin?space=0&op=flush"));
    ASSERT_EQ("ok", request("/admin?space=0&op=compact"));
}

}  // namespace storage
}  // namespace nebula


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    ::testing::AddGlobalTestEnvironment(new nebula::storage::StorageHttpAdminHandlerTestEnv());

    return RUN_ALL_TESTS();
}
