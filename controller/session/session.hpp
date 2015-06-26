#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "types/source-manager.hpp"
#include "util/once.hpp"

namespace pdal
{
    class PointContext;
    class StageFactory;
}

namespace entwine
{
    class Arbiter;
    class BBox;
    class Cache;
    class Schema;
    class Reader;
}

class ReadQuery;
class Paths;

class Session
{
public:
    Session(pdal::StageFactory& stageFactory, std::mutex& factoryMutex);
    ~Session();

    // Returns true if initialization was successful.  If false, this session
    // should not be used.
    bool initialize(
            const std::string& name,
            const Paths& paths,
            std::shared_ptr<entwine::Arbiter> arbiter,
            std::shared_ptr<entwine::Cache> cache);

    std::size_t getNumPoints();
    std::string getStats();
    std::string getSrs();
    entwine::BBox getBounds();

    // Read a full unindexed data set.
    std::shared_ptr<ReadQuery> query(
            const entwine::Schema& schema,
            bool compress);

    // Read quad-tree indexed data with a bounding box query and min/max tree
    // depths to search.
    std::shared_ptr<ReadQuery> query(
            const entwine::Schema& schema,
            bool compress,
            const entwine::BBox& bbox,
            std::size_t depthBegin,
            std::size_t depthEnd);

    const entwine::Schema& schema();

private:
    bool sourced();
    bool indexed();

    bool resolveSource();
    bool resolveIndex();

    pdal::StageFactory& m_stageFactory;
    std::mutex& m_factoryMutex;

    Once m_initOnce;
    std::unique_ptr<SourceManager> m_source;
    std::unique_ptr<entwine::Reader> m_entwine;

    std::mutex m_sourceMutex;
    std::mutex m_indexMutex;

    std::string m_name;
    std::unique_ptr<Paths> m_paths;
    std::shared_ptr<entwine::Arbiter> m_arbiter;
    std::shared_ptr<entwine::Cache> m_cache;

    // Disallow copy/assignment.
    Session(const Session&);
    Session& operator=(const Session&);
};

