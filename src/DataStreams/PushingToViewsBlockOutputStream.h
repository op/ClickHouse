#pragma once

#include <DataStreams/IBlockOutputStream.h>
#include <Interpreters/QueryViewsLog.h>
#include <Parsers/IAST_fwd.h>
#include <Processors/Chain.h>
#include <Processors/ISimpleTransform.h>
#include <Storages/IStorage.h>
#include <Processors/Sinks/SinkToStorage.h>
#include <Common/Stopwatch.h>

namespace Poco
{
class Logger;
}

namespace DB
{

class ReplicatedMergeTreeSink;

struct ExceptionKeepingTransformRuntimeData;
using ExceptionKeepingTransformRuntimeDataPtr = std::shared_ptr<ExceptionKeepingTransformRuntimeData>;

struct ViewRuntimeData
{
    const ASTPtr query;
    Block sample_block;

    StorageID table_id;
    StoragePtr storage;
    StorageMetadataPtr metadata_snapshot;

    ContextPtr context;

    std::exception_ptr exception;
    QueryViewsLogElement::ViewRuntimeStats runtime_stats;

    void setException(std::exception_ptr e)
    {
        exception = e;
        runtime_stats.setStatus(QueryViewsLogElement::ViewStatus::EXCEPTION_WHILE_PROCESSING);
    }
};

/** Writes data to the specified table and to all dependent materialized views.
  */
Chain buildPushingToViewsDrain(
    const StoragePtr & storage,
    const StorageMetadataPtr & metadata_snapshot,
    ContextPtr context,
    const ASTPtr & query_ptr,
    bool no_destination,
    ExceptionKeepingTransformRuntimeDataPtr runtime_data,
    const Block & lv_storage = {});


class ExecutingInnerQueryFromViewTransform final : public ExceptionKeepingTransform
{
public:
    ExecutingInnerQueryFromViewTransform(
        const Block & header,
        ViewRuntimeData & view_data,
        const StorageID & source_storage_id_,
        const StorageMetadataPtr & source_metadata_snapshot_,
        const StoragePtr & source_storage_)
        : ExceptionKeepingTransform(header, view_data.sample_block)
        , view(view_data)
        , source_storage_id(source_storage_id_)
        , source_metadata_snapshot(source_metadata_snapshot_)
        , source_storage(source_storage_)
    {
    }

    String getName() const override { return "ExecutingInnerQueryFromView"; }

protected:
    void transform(Chunk & chunk) override;

private:
    ViewRuntimeData & view;
    const StorageID & source_storage_id;
    const StorageMetadataPtr & source_metadata_snapshot;
    const StoragePtr & source_storage;
};

}
