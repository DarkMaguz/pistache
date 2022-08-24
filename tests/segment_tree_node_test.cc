/*
 * segment_tree_node_test.cc
 *
 *  Created on: Aug 24, 2022
 *      Author: Peter Balling
 */

#define __cplusplus 201703L

#include <algorithm>
#include <gtest/gtest.h>

#include <pistache/common.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;
using namespace Pistache::Rest;

bool match(const SegmentTreeNode& routes, const std::string& req)
{
    const auto& s = SegmentTreeNode::sanitizeResource(req);
    std::shared_ptr<Route> route;
    std::tie(route, std::ignore, std::ignore) = routes.findRoute({ s.data(), s.size() });
    return route != nullptr;
}

const Rest::SegmentTreeNode node_builder(const std::string& resource)
{
    Rest::SegmentTreeNode node;
    const auto& s = Rest::SegmentTreeNode::sanitizeResource(resource);
    node.addRoute(std::string_view { s.data(), s.length() }, nullptr, nullptr);
    return node;
}

TEST(segment_tree_node_test, test_node_chaching_postbuild)
{
    typedef std::pair<Http::Method, const std::string> nodePair_t;
    typedef std::unordered_multimap<Http::Method, const std::string> nodeCache_t;

    nodeCache_t nodeCache;
    nodeCache.insert(nodePair_t(Http::Method::Get, "/tinkywinky"));
    nodeCache.insert(nodePair_t(Http::Method::Get, "/dipsy"));
    nodeCache.insert(nodePair_t(Http::Method::Get, "/laa-laa"));

    auto isAuthRequired = [nodeCache](const Http::Method& methode, const std::string& resource) {
        auto range        = nodeCache.equal_range(methode);
        bool authRequired = false;
        for (auto it = range.first; it != range.second; it++)
        {
            if (match(node_builder(it->second), resource))
            {
                authRequired = true;
            }
        }
        return authRequired;
    };

    ASSERT_TRUE(isAuthRequired(Http::Method::Get, "/tinkywinky"));
    ASSERT_FALSE(isAuthRequired(Http::Method::Put, "/tinkywinky"));
}

TEST(segment_tree_node_test, test_node_chaching_prebuild)
{
    typedef std::pair<Http::Method, const Rest::SegmentTreeNode> nodePair_t;
    typedef std::unordered_multimap<Http::Method, const Rest::SegmentTreeNode> nodeCache_t;

    nodeCache_t nodeCache;
    nodeCache.insert(nodePair_t(Http::Method::Get, node_builder("/tinkywinky")));
    nodeCache.insert(nodePair_t(Http::Method::Get, node_builder("/dipsy")));
    nodeCache.insert(nodePair_t(Http::Method::Get, node_builder("/laa-laa")));

    auto isAuthRequired = [nodeCache](const Http::Method& methode, const std::string& resource) {
        auto range        = nodeCache.equal_range(methode);
        bool authRequired = false;
        for (auto it = range.first; it != range.second; it++)
        {
            if (match(it->second, resource))
            {
                authRequired = true;
            }
        }
        return authRequired;
    };

    ASSERT_TRUE(isAuthRequired(Http::Method::Get, "/tinkywinky"));
    ASSERT_FALSE(isAuthRequired(Http::Method::Put, "/tinkywinky"));
}
